                                        /*
 * ===========================================================================================================
 * InputStep.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#include "InputStep.h"


namespace processing{
//============================================================================================================
//	Klasse InputStep:
//	Hatt mehrere Ausgaenge. Zordung des Input-Signals ist zustands abhaengig.
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void InputStep::typeChanged ( void *src, const float &v ){
	Parameter *pP = (Parameter*) src;
	int n = com::mapInteger ( v, 2);
	switch (n){
		case 0 : 
			pP->setDisplay ("fix");
			cStep->setValueTranslator ( &fixTimeValue );
			break;
		case 1:
			pP->setDisplay ("sync");
			cStep->setValueTranslator ( sync );
			break;
	}
	cStep->resetParameterLabel();
}
//------------------------------------------------------------------------------------------------------------
void InputStep::init(){
	// Type Parameter:
	type->setName ("Step Type");
	type->setDisplay("fix");
	parameterMap.push_back (type);
	Parameter::ParameterListenerFunction f = boost::bind( 
			&InputStep::typeChanged, this, _1, _2 
	);
	type->addValueChangedListener (f);
	// Adapter Nodes:
	for ( int i=0; i<cStep->getNumSteps(); ++i ) {
		inputNodes[i]->setName("StepInputNode["+com::MyString(i+1)+"]");
		if ( i<cStep->getNumSteps() ) {
			parameterMap.push_back ( cStep->getParameter(i));
			parameterMap.push_back ( cStep->Switch::getDurationParameterIN(i) );
			parameterMap.push_back ( cStep->Switch::getCurveTypeParameterIN(i) );
			parameterMap.push_back ( cStep->Switch::getDurationParameterOUT(i) );
			parameterMap.push_back ( cStep->Switch::getCurveTypeParameterOUT(i) );
		}
	}
    parameterMap.push_back(cStep->getStepIndicator());
}
//------------------------------------------------------------------------------------------------------------
ProcessorNode::Ptr InputStep::addInputNode(){
	TRY_TO_LOCK_TIMED (mutex);
	InputNode::Ptr neu = ProcessAdapter::createInputNode ( 
		"stepinput inputNode("+com::MyString(getNumInputNodes()+2) + ")"
	);
	cStep->addState();
	cStep->setNumSteps (cStep->getNumSteps()+1);
	size_t step = cStep->getNumSteps()-1;
	parameterMap.push_back ( cStep->getParameter(step) );
	parameterMap.push_back ( cStep->Switch::getDurationParameterIN(step) );
	parameterMap.push_back ( cStep->Switch::getCurveTypeParameterIN(step) );
	parameterMap.push_back ( cStep->Switch::getDurationParameterOUT(step) );
	parameterMap.push_back ( cStep->Switch::getCurveTypeParameterOUT(step) );
	inputMatrix.push_back ( NULL );
	cStep->reset();
	return neu;
}
//------------------------------------------------------------------------------------------------------------
InputStep::InputStep(frx::processing::IHostInfo::Ptr hostInfo, int initSteps) : 
ProcessAdapter( hostInfo, initSteps, 1 ), 
type ( Parameter::create() ),
inputMatrix ( InputMatrix ( initSteps, (Frames*)NULL ) ),
fixTimeValue( 10.f, hostInfo->getSampleRate() )
{
	setName ( "StepInputAdapter" );
	getOutputNode(0)->setName ( getName() + " OutputNode");
	cStep = new Step( &fixTimeValue, initSteps, hostInfo->getSampleRate() );
	cStep->stateChangedDelegate = 
		boost::bind(&InputStep::stateChangedHandler, this, _1, _2);
	sync = new SyncTranslator ( hostInfo );
	init();
	cStep->setNumSteps (initSteps);
	tmpFrame.setSize ( hostInfo->getBlockSize() );
}
//------------------------------------------------------------------------------------------------------------
void InputStep::reset(){
	frx::processing::IHostInfo::Ptr hI = hostInfo.lock();
	if (!hI) {
		SAMBAG_THROW(sambag::com::exceptions::IllegalStateException,
			"Hostinfo == NULL"
		);
	}
	cStep->reset();
	using namespace frx::processing;
	// ermittle anzahl der samples bis zu naechsten 1/4 note
	TimeInfo *inf = hI->getHostTimeInfo( TimeInfo::FrxPpqPos || TimeInfo::FrxTempo );
	double m = inf->ppqPos - ((int)inf->ppqPos) ; //ziffern hintern komma.
	if ( m == 0.0 ) return;
	if ( m < 0.5 ) { // dauer verkuerzen
		double rest = m / 4.0; // 1.0 == in ppqPos 1/4
		cStep->setOffset ( -note2Sample ( rest, inf->tempo, inf->sampleRate ) );
	} else { //dauer verlaengern 
		double rest = (1.0 - m) / 4.0; 
		cStep->setOffset ( note2Sample ( rest, inf->tempo, inf->sampleRate ) );
	}
}
//------------------------------------------------------------------------------------------------------------
inline void InputStep::processFrames ( InputMatrix &fr, Processor::Int numSamples ) {
	com::VstNumber *l = tmpFrame[0];
	com::VstNumber *r = tmpFrame[1];
	for ( size_t i=0; i<numSamples; ++i ){
		*l = 0.0f; *r = 0.0f;
		cStep->skimDuration();
		for ( int j=0; j<cStep->getNumSteps(); j++ ){
			float fac = cStep->getFaderValueAndIncT(j); // mit jedem lesezugriff wird fader::t erhoet!
			if (!fr[j]) {
				continue; // !!Wichtig
			}
			(*l) += (*fr[j])[0][i] * fac;
			(*r) += (*fr[j])[1][i] * fac;
		}
		++l; ++r;
	}
}
//------------------------------------------------------------------------------------------------------------
void InputStep::processAdapter( Processor::Int numSamples ) {
	frx::processing::IHostInfo::Ptr hI = hostInfo.lock();
	if (!hI) {
		SAMBAG_THROW(sambag::com::exceptions::IllegalStateException,
			"Hostinfo == NULL"
		);
	}
	TRY_TO_LOCK_TIMED (mutex); // gleichzeitigen zugriff von addOutputNode blocken
	frx::processing::TimeInfo *inf = hI->getHostTimeInfo(0);
	ClockEdge::EdgeValue t = transport.in (inf->transportIsPlaying());
	if (t == ClockEdge::HIGH ){ // Transport: play flanke
		reset();
	}
	for ( int i=0; i<cStep->getNumSteps(); ++i ) { // bilde InputFrames auf Matrix ab.
		if ( !inputNodes[i]->isActive() ) {
			inputMatrix[i] = NULL;
			continue;
		}
		inputMatrix[i] = inputNodes[i]->popFrame();
	}
	// berechne OutputFrames
	processFrames( inputMatrix, numSamples );
	outputNodes[0]->pushAndCopy ( &tmpFrame, numSamples );
}
//------------------------------------------------------------------------------------------------------------
InputStep::~InputStep(){
	delete cStep;
	delete sync;
}
//------------------------------------------------------------------------------------------------------------
void InputStep::save(com::oArchive &ar, const unsigned int version) const {
	ar.register_type< FixTimeValue >();
	ar.register_type< SyncTranslator >();
	ar << boost::serialization::base_object< ProcessAdapter > ( *this );
	ar << parameterMap;
	ar << type;
	ar << fixTimeValue;
	ar << sync;
	ar << cStep;
}
//------------------------------------------------------------------------------------------------------------
void InputStep::load(com::iArchive &ar, const unsigned int version) {
	ar.register_type< FixTimeValue >();
	ar.register_type< SyncTranslator >();
	ar >> boost::serialization::base_object< ProcessAdapter > ( *this );
	ar >> parameterMap;
	ar >> type;
	ar >> fixTimeValue;
	ar >> sync;
	ar >> cStep;
	cStep->stateChangedDelegate = 
		boost::bind(&InputStep::stateChangedHandler, this, _1, _2);
	frx::processing::IHostInfo::Ptr hI = hostInfo.lock();
	if (!hI) {
		SAMBAG_THROW(sambag::com::exceptions::IllegalStateException,
			"Hostinfo == NULL"
		);
	}
	tmpFrame.setSize ( hI->getBlockSize() );
	Parameter::ParameterListenerFunction f = boost::bind( 
			&InputStep::typeChanged, this, _1, _2 
	);
	type->addValueChangedListener ( f );
	inputMatrix = InputMatrix( cStep->getNumSteps(), (Frames*)NULL );
}
}// namespace processing
