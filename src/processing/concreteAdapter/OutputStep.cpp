                                        /*
 * ===========================================================================================================
 * OutputStep.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#include "OutputStep.h"


namespace processing{
//============================================================================================================
//	Klasse OutputStep:
//	Hatt mehrere Ausgaenge. Zordung des Input-Signals ist zustands abhaengig.
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void OutputStep::typeChanged ( void *src, const float &v ){
	Parameter *pP = (Parameter*) src;
	int n = mapInteger ( v, 2);
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
void OutputStep::init(){
	// Type Parameter:
	type->setName ("Step Type");
	type->setDisplay("fix");
	parameterMap.push_back (type);
	Parameter::ParameterListenerFunction f = boost::bind( 
			&OutputStep::typeChanged, this, _1, _2 
	);
	type->addValueChangedListener (f);
	// Adapter Nodes:
	for ( int i=0; i<cStep->getNumSteps(); ++i ) {
		outputNodes[i]->setName("StepOutputode["+MyString(i+1)+"]");
		if ( i<cStep->getNumSteps() ) {
			parameterMap.push_back ( cStep->getParameter(i) );
			parameterMap.push_back ( cStep->Switch::getDurationParameterIN(i) );
			parameterMap.push_back ( cStep->Switch::getCurveTypeParameterIN(i) );
			parameterMap.push_back ( cStep->Switch::getDurationParameterOUT(i) );
			parameterMap.push_back ( cStep->Switch::getCurveTypeParameterOUT(i) );
		}
	}
}
//------------------------------------------------------------------------------------------------------------
ProcessorNode::Ptr OutputStep::addOutputNode(){
	TRY_TO_LOCK_TIMED (mutex);
	OutputNode::Ptr neu = ProcessAdapter::createOutputNode ( 
		"StepOutput outputNode("+MyString(getNumOutputNodes()+2) + ")"
	);
	cStep->addState();
	cStep->setNumSteps (cStep->getNumSteps()+1); //TODO: addStep?
	size_t step = cStep->getNumSteps()-1;
	parameterMap.push_back ( cStep->getParameter(step) );
	parameterMap.push_back ( cStep->Switch::getDurationParameterIN(step) );
	parameterMap.push_back ( cStep->Switch::getCurveTypeParameterIN(step) );
	parameterMap.push_back ( cStep->Switch::getDurationParameterOUT(step) );
	parameterMap.push_back ( cStep->Switch::getCurveTypeParameterOUT(step) );
	outpMatrix.push_back ( NULL );
	cStep->reset();
	return neu;
}
//------------------------------------------------------------------------------------------------------------
OutputStep::OutputStep(frx::processing::IHostInfo::Ptr hostInfo, int initSteps) : 
ProcessAdapter( hostInfo, 1, initSteps ), 
type ( Parameter::create() ),
outpMatrix ( OutputMatrix ( initSteps, (Frames*)NULL ) ),
fixTimeValue( 10.f, hostInfo->getSampleRate() )
{
	setName ( "StepOutputAdapter" );
	getInputNode(0)->setName ( getName() + " InputNode");
	cStep = new Step( &fixTimeValue, initSteps, hostInfo->getSampleRate() );
	cStep->stateChangedDelegate = 
		boost::bind(&OutputStep::stateChangedHandler, this, _1, _2);
	sync = new SyncTranslator ( hostInfo );
	init();
	cStep->setNumSteps (initSteps);
	TOLOG ( "+" + getName() );
}
//------------------------------------------------------------------------------------------------------------
void OutputStep::reset(){
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
	double m = inf->ppqPos -  ((int)inf->ppqPos) ; //ziffern hintern komma.
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
inline void OutputStep::processFrames ( Frames *iFrame, OutputMatrix &fr, Processor::Int numSamples ) {
	VstNumber *l = (*iFrame)[0];
	VstNumber *r = (*iFrame)[1];
	for ( size_t i=0; i<numSamples; ++i ) {
		cStep->skimDuration();
		for ( int j=0; j<cStep->getNumSteps(); j++ ){
			float fac = cStep->getFaderValueAndIncT(j); // mit jedem lesezugriff wird fader::t erhoet!
			if (!fr[j]) continue; // !!Wichtig
			(*fr[j])[0][i] = *l * fac;
			(*fr[j])[1][i] = *r * fac;
		}
		++l; ++r;
	}
}
//------------------------------------------------------------------------------------------------------------
void OutputStep::processAdapter( Processor::Int numSamples ) {
	frx::processing::IHostInfo::Ptr hI = hostInfo.lock();
	if (!hI) {
		SAMBAG_THROW(sambag::com::exceptions::IllegalStateException,
			"Hostinfo == NULL"
		);
	}
	TRY_TO_LOCK_TIMED (mutex); // gleichzeitigen zugriff von addOutputNode blocken
	frx::processing::TimeInfo *inf = hI->getHostTimeInfo(0);
	ClockEdge::EdgeValue t = transport.in ( inf->transportIsPlaying() );
	if (t == ClockEdge::HIGH ){ // Transport: play flanke
		reset();
	}
	Frames *frame = getInputNode(0)->popFrame();
	Frames iFrame;
	iFrame.copyIntoFrom ( *frame, numSamples );
	aNode->pushAndCopy ( frame, numSamples );
	for ( int i=0; i<cStep->getNumSteps(); ++i ) { // bilde InputFrames auf Matrix ab.
		if ( !outputNodes[i]->isActive() ) {
			outpMatrix[i] = NULL;
			continue;
		}
		outpMatrix[i] = aNode->popFrame();
	}
	// berechne OutputFrames
	processFrames( &iFrame, outpMatrix, numSamples );
	for ( int i=0; i<cStep->getNumSteps(); ++i ) 
		outputNodes[i]->pushAndCopy ( outpMatrix[i], numSamples ); // knoten Frames zuweisen
}
//------------------------------------------------------------------------------------------------------------
OutputStep::~OutputStep(){
	delete cStep;
	delete sync;
	TOLOG ( "-" + getName() );
}
//------------------------------------------------------------------------------------------------------------
void OutputStep::save(com::oArchive &ar, const unsigned int version) const {
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
void OutputStep::load(com::iArchive &ar, const unsigned int version) {
	ar.register_type< FixTimeValue >();
	ar.register_type< SyncTranslator >();
	ar >> boost::serialization::base_object< ProcessAdapter > ( *this );
	ar >> parameterMap;
	ar >> type;
	ar >> fixTimeValue;
	ar >> sync;
	ar >> cStep;
	cStep->stateChangedDelegate = 
		boost::bind(&OutputStep::stateChangedHandler, this, _1, _2);
	Parameter::ParameterListenerFunction f = boost::bind( 
			&OutputStep::typeChanged, this, _1, _2 
	);
	type->addValueChangedListener ( f );
	outpMatrix = OutputMatrix( cStep->getNumSteps(), (Frames*)NULL );
}
}// namespace processing
