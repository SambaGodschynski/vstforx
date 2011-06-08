#include "ConcreteProcessAdapter.h"
#include <stack>

namespace processing{
//============================================================================================================
// class VolumeAdapter
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void VolumeAdapter::_processAdapter( Processor::Int sampleFrames ) { 
	Frame *fr = getInputNode(0)->popFrame();
	VstInt32 i = sampleFrames;
	VstNumber *r = (*fr)[0];
	VstNumber *l = (*fr)[1];
	while ( --i >= 0 ){
		float f = fader; // mit jedem lesezugriff wird fader::t erhoet!
		*(r++) *= f;
		*(l++) *= f;
	}
	outputNodes[0]->pushAndCopy( fr, sampleFrames );
}
//============================================================================================================
// class PanAdapter
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
PanAdapter::PanAdapter ( IHostInfo *hostInfo ) : ProcessAdapter( hostInfo, 1, 1 ) {
	setName ("PanAdapter");
	getOutputNode(0)->setName ("PanAdapter_Output");
	getInputNode(0)->setName ("PanAdapter_Input");
	TOLOG ( "+" + getName() );
	
	pan = Parameter::create();
	pan->setName ("Pan");
	pan->setValue (0.5);
}
//------------------------------------------------------------------------------------------------------------
void PanAdapter::_processAdapter( Processor::Int sampleFrames ) {
	Frame *fr = getInputNode(0)->popFrame();
	float *l = (*fr)[0];
	float *r = (*fr)[1];
	Frame::Int c = sampleFrames;
	while ( c-- > 0 ) {
		*l = *l * ( 1.0f - *pan );
		*r = *r * *pan;
		l++; r++;
	}
	getOutputNode(0)->pushAndCopy ( fr, sampleFrames );
}
//============================================================================================================
// Klasse Switch:
// verwaltet N FadeValue und dazugehoerige Parameter
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
Switch::Switch( size_t _numStates, float sampleRate ) :
numStates(0), // wird ueber _addState erhoet
state(0),
nFader( FadeValues(_numStates) ),
nDurationIN( Parameters(_numStates) ),
nDurationOUT( Parameters(_numStates) ),
nCurveTypeIN( Parameters(_numStates) ),
nCurveTypeOUT( Parameters(_numStates) )
{
	for ( size_t i=0; i<_numStates; ++i ) {
		_addState();
	}
	setSampleRate( sampleRate ); 
}
//--------------------------------------------------------------------------------------------------------
void Switch::_addState() {
	Parameter::ParameterListenerFunction dI = boost::bind( 
		&Switch::durationINChanged, this, _1, _2 
	);
	Parameter::ParameterListenerFunction dO = boost::bind( 
		&Switch::durationOUTChanged, this, _1, _2 
	);
	Parameter::ParameterListenerFunction cT = boost::bind( 
		&Switch::curveTypeChanged, this, _1, _2 
	);
	size_t i = getNumStates();
	++numStates;
	// prepare duration parameter
	nDurationIN[i] = Parameter::create(i);
	parameterMap.push_back( nDurationIN[i] );
	nDurationIN[i]->setName("fade-in duration " + MyString(i+1) );
	nDurationIN[i]->setLabel("ms");
	nDurationIN[i]->setMin(FLT_MIN);
	nDurationIN[i]->addValueChangedListenerF ( dI );
	nDurationOUT[i] = Parameter::create(i);
	parameterMap.push_back( nDurationOUT[i] );
	nDurationOUT[i]->setName("fade-out duration " + MyString(i+1) );
	nDurationOUT[i]->setLabel("ms");
	nDurationOUT[i]->setMin(FLT_MIN);
	nDurationIN[i]->addValueChangedListenerF ( dO );
	// prepare curve type parameter
	nCurveTypeIN[i] = Parameter::create(i);
	parameterMap.push_back( nCurveTypeIN[i] );
	nCurveTypeIN[i]->setName("fade-in curve type " + MyString(i+1) );
	nCurveTypeIN[i]->addValueChangedListenerF ( cT );
	nCurveTypeOUT[i] = Parameter::create(i);
	parameterMap.push_back( nCurveTypeOUT[i] );
	nCurveTypeOUT[i]->setName("fade-out curve type " + MyString(i+1) );
	nCurveTypeOUT[i]->addValueChangedListenerF ( cT );
	// init
	*nDurationIN[i] = 0.01f;
	*nDurationOUT[i] = 0.01f;
	*nCurveTypeIN[i] = 0.0f;
	*nCurveTypeOUT[i] = 0.0f;
}
//--------------------------------------------------------------------------------------------------------
void Switch::addState() {
	nFader.push_back ( FadeValue() );
	nDurationIN.push_back ( Parameter::Ptr() );
	nDurationOUT.push_back ( Parameter::Ptr() );
	nCurveTypeIN.push_back ( Parameter::Ptr() );
	nCurveTypeOUT.push_back ( Parameter::Ptr() );
	_addState();
}
//--------------------------------------------------------------------------------------------------------
void Switch::durationINChanged ( void *src, const float &v ) {
}
//--------------------------------------------------------------------------------------------------------
void Switch::durationOUTChanged ( void *src, const float &v ) {
}
//--------------------------------------------------------------------------------------------------------
void Switch::curveTypeChanged ( void *src, const float &v ) {
	Parameter *p = (Parameter*) src;
	int t = mapInteger ( *p, FadeValue::NUM_FADE_TYPES );
	p->setDisplay ("type " + MyString(t+1) );
}
//------------------------------------------------------------------------------------------------------------
Switch::~Switch() {
}
//============================================================================================================
//	Klasse OutputSwitch :
//	Schaltet mehrere Ausgaenge
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
ProcessorNode::Ptr OutputSwitch::addOutputNode() {
	TRY_TO_LOCK_TIMED (mutex);
	OutputNode::Ptr neu = ProcessAdapter::createOutputNode ( 
		"outputswitch outputNode("+MyString(getNumOutputNodes()+2) + ")"
	);
	addState();
	size_t states = getNumStates();
	parameterMap.push_back ( getDurationParameterIN ( states-1 ) ); 
	parameterMap.push_back ( getCurveTypeParameterIN ( states-1 ) ); 
	parameterMap.push_back ( getDurationParameterOUT ( states-1 ) ); 
	parameterMap.push_back ( getCurveTypeParameterOUT ( states-1 ) ); 
	outpMatrix.push_back ( NULL );
	return neu;
}
//------------------------------------------------------------------------------------------------------------
void OutputSwitch::valueChanged ( void *src, const float &val ) {
	Switch::setState ( mapInteger ( val, getNumStates() ) );
}
//------------------------------------------------------------------------------------------------------------
OutputSwitch::OutputSwitch( IHostInfo *hostInfo, int initStates ) : 
ProcessAdapter( hostInfo, 1, initStates ), 
Switch ( initStates, hostInfo->getSampleRate() ), outpMatrix( OutputMatrix(initStates, (Frame*)NULL) )
{
	setName ( "OutputSwitch" );
	selector = Parameter::create();
	selector->setName ("selector switch");
	selector->addValueChangedListener ( this );
	parameterMap.push_back ( selector );
	getInputNode(0)->setName ( getName() + " InputNode" );
	for ( size_t i=0; i<getNumOutputNodes(); ++i ) {
		parameterMap.push_back ( getDurationParameterIN ( i ) ); 
		parameterMap.push_back ( getCurveTypeParameterIN ( i ) ); 
		parameterMap.push_back ( getDurationParameterOUT ( i ) ); 
		parameterMap.push_back ( getCurveTypeParameterOUT ( i ) ); 
		getOutputNode(i)->setName ( getName() + " OutputNode" );
	}
	*selector = 0.0f;
	TOLOG ( "+" + getName() );
}
//--------------------------------------------------------------------------------------------------------
inline void OutputSwitch::_processFrames ( Frame *iFrame, OutputMatrix &fr, Processor::Int sampleFrames ) {
	VstNumber *l = (*iFrame)[0];
	VstNumber *r = (*iFrame)[1];
	for ( int i=0; i<sampleFrames; ++i ){
		for ( size_t j=0; j<getNumStates(); j++ ){
			float fac = getFaderValueAndIncT(j); // mit jedem lesezugriff wird fader::t erhoet!
			if (!fr[j]) continue; // !!Wichtig
			(*fr[j])[0][i] = *l * fac;
			(*fr[j])[1][i] = *r * fac;
		}
		++l; ++r;
	}
}
//------------------------------------------------------------------------------------------------------------
void OutputSwitch::_processAdapter( Processor::Int sampleFrames ) {
	TRY_TO_LOCK_TIMED (mutex); // gleichzeitigen zugriff von addOutputNode blocken
	Frame *frame = getInputNode(0)->popFrame();
	Frame iFrame;
	iFrame.copyIntoFrom ( *frame, sampleFrames );
	aNode->pushAndCopy ( frame, sampleFrames );
	size_t steps = getNumStates();
	for ( int i=0; i<steps; ++i ) { // bilde InputFrames auf Matrix ab.
		if ( !outputNodes[i]->isActive() ) {
			outpMatrix[i] = NULL;
			continue;
		}
		outpMatrix[i] = aNode->popFrame();
	}
	// berechne OutputFrames
	_processFrames( &iFrame, outpMatrix, sampleFrames );
	for ( int i=0; i<steps; ++i ) {
		if ( !outputNodes[i]->isActive() ) continue;
		outputNodes[i]->pushAndCopy ( outpMatrix[i], sampleFrames ); // knoten Frame zuweisen*/
	}
}
//------------------------------------------------------------------------------------------------------------
OutputSwitch::~OutputSwitch(){
	TOLOG ( "-" + getName() );
}
//------------------------------------------------------------------------------------------------------------
void OutputSwitch::save(com::oArchive &ar, const unsigned int version) const {
	ar << boost::serialization::base_object< ProcessAdapter > ( *this );
	ar << boost::serialization::base_object< Switch > ( *this );
	ar << parameterMap;
	ar << selector;
}
//------------------------------------------------------------------------------------------------------------
void OutputSwitch::load(com::iArchive &ar, const unsigned int version) {
	ar >> boost::serialization::base_object< ProcessAdapter > ( *this );
	ar >> boost::serialization::base_object< Switch > ( *this );
	ar >> parameterMap;
	ar >> selector;
	Switch::setSampleRate( hostInfo->getSampleRate() );
	outpMatrix = OutputMatrix ( getNumStates(), NULL );
	selector->addValueChangedListener ( this );

}
//============================================================================================================
//	Klasse InputSwitch :
//	Schaltet mehrere Ausgaenge
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
ProcessorNode::Ptr InputSwitch::addInputNode() {
	TRY_TO_LOCK_TIMED (mutex);
	InputNode::Ptr neu = ProcessAdapter::createInputNode ( 
		"Inputswitch InputNode("+MyString(getNumInputNodes()+2) + ")"
	);
	addState();
	size_t states = getNumStates();
	parameterMap.push_back ( getDurationParameterIN ( states-1 ) ); 
	parameterMap.push_back ( getCurveTypeParameterIN ( states-1 ) ); 
	parameterMap.push_back ( getDurationParameterOUT ( states-1 ) ); 
	parameterMap.push_back ( getCurveTypeParameterOUT ( states-1 ) ); 
	inputMatrix.push_back ( NULL );
	return neu;
}
//------------------------------------------------------------------------------------------------------------
void InputSwitch::valueChanged ( void *src, const float &val ) {
	Switch::setState ( mapInteger ( val, getNumStates() ) );
}
//------------------------------------------------------------------------------------------------------------
InputSwitch::InputSwitch( IHostInfo *hostInfo, int initStates ) : 
ProcessAdapter( hostInfo, initStates, 1 ), 
Switch ( initStates, hostInfo->getSampleRate() ), inputMatrix( InputMatrix(initStates, (Frame*)NULL) )
{
	setName ( "InputSwitch" );
	selector = Parameter::create();
	selector->setName ("selector switch");
	selector->addValueChangedListener ( this );
	parameterMap.push_back ( selector );
	getOutputNode(0)->setName ( getName() + " OutputNode" );
	for ( size_t i=0; i<getNumInputNodes(); ++i ) {
		parameterMap.push_back ( getDurationParameterIN ( i ) ); 
		parameterMap.push_back ( getCurveTypeParameterIN ( i ) ); 
		parameterMap.push_back ( getDurationParameterOUT ( i ) ); 
		parameterMap.push_back ( getCurveTypeParameterOUT ( i ) ); 
		getInputNode(i)->setName ( getName() + " InputNode" );
	}
	*selector = 0.0f;
	TOLOG ( "+" + getName() );

	tmpFrame.setSize ( hostInfo->getBlockSize() );
}
//--------------------------------------------------------------------------------------------------------
inline void InputSwitch::_processFrames ( InputMatrix &fr, Processor::Int sampleFrames ) {
	VstNumber *l = tmpFrame[0];
	VstNumber *r = tmpFrame[1];
	for ( int i=0; i<sampleFrames; ++i ){
		*l = 0.0f; *r = 0.0f;
		for ( int j=0; j<getNumStates(); j++ ){
			float fac = getFaderValueAndIncT(j); // mit jedem lesezugriff wird fader::t erhoet!
			if (!fr[j]) continue; // !!Wichtig
			 (*l) += (*fr[j])[0][i] * fac;
			 (*r) += (*fr[j])[1][i] * fac;
		}
		++l; ++r;
	}
}
//------------------------------------------------------------------------------------------------------------
void InputSwitch::_processAdapter( Processor::Int sampleFrames ) {
	TRY_TO_LOCK_TIMED (mutex); // gleichzeitigen zugriff von addOutputNode blocken

	for ( int i=0; i<getNumStates(); ++i ) { // bilde InputFrames auf Matrix ab.
		if ( !inputNodes[i]->isActive() ) {
			inputMatrix[i] = NULL;
			continue;
		}
		inputMatrix[i] = inputNodes[i]->popFrame();
	}
	// berechne OutputFrames
	_processFrames( inputMatrix, sampleFrames );
	outputNodes[0]->pushAndCopy ( &tmpFrame, sampleFrames );
}
//------------------------------------------------------------------------------------------------------------
InputSwitch::~InputSwitch() {
	TOLOG ( "-" + getName() );
}
//------------------------------------------------------------------------------------------------------------
void InputSwitch::save(com::oArchive &ar, const unsigned int version) const {
	ar << boost::serialization::base_object< ProcessAdapter > ( *this );
	ar << boost::serialization::base_object< Switch > ( *this );
	ar << parameterMap;
	ar << selector;
}
//------------------------------------------------------------------------------------------------------------
void InputSwitch::load(com::iArchive &ar, const unsigned int version) {
	ar >> boost::serialization::base_object< ProcessAdapter > ( *this );
	ar >> boost::serialization::base_object< Switch > ( *this );
	ar >> parameterMap;
	ar >> selector;
	Switch::setSampleRate( hostInfo->getSampleRate() );
	inputMatrix = InputMatrix ( getNumStates(), NULL );
	selector->addValueChangedListener ( this );

}
//============================================================================================================
// Klasse Step:
// Verwaltet Step zustaende und zugehoerige Multiplikations Faktoren.
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
Step::Step( ValueTranslator* tr, size_t initSteps, float sampleRate ) : 
currTranslator(tr), Switch (initSteps, sampleRate ),  steps(initSteps), nDuration(initSteps) 
{ 
	Parameter::ParameterListenerFunction f = boost::bind( 
			&Step::durationParameterChanged, this, _1, _2 
		);
	for (int i=0; i<steps; ++i) {
		setFaderValue ( i, 0.0f );
		nDuration[i] = Parameter::create(); 
		Parameter::Ptr p = getParameter(i);
		p->setName ( "Step " + MyString(i+1) + " duration." );
		p->addValueChangedListenerF (f);
		p->setValue(0.35f);
	}
	setDuration();
}
//------------------------------------------------------------------------------------------------------------
Step::~Step() {
}
//------------------------------------------------------------------------------------------------------------
void Step::addState() {
	Switch::addState();
	
	Parameter::ParameterListenerFunction f = boost::bind( 
		&Step::durationParameterChanged, this, _1, _2 
	);
	nDuration.push_back ( Parameter::Ptr() );
	size_t i = getNumStates() - 1;
	nDuration[i] = Parameter::create(); 
	Parameter::Ptr p = getParameter(i);
	p->setName ( "Step " + MyString(i+1) + " duration." );
	p->addValueChangedListenerF (f);
	p->setValue(0.35f);

}
//------------------------------------------------------------------------------------------------------------
void Step::resetLabel(){
	for (int i=0; i<steps; ++i) {
		Parameter::Ptr p = getParameter(i);
		p->setValue(p->getValue());
	}
}
//------------------------------------------------------------------------------------------------------------
void Step::setDuration() {
	duration = currTranslator->translate ( *nDuration[getState()] );
}
//------------------------------------------------------------------------------------------------------------
void Step::durationParameterChanged ( void *src, const float &v ){
	Parameter *p = (Parameter*) src;
	p->setDisplay ( currTranslator->translateAsString(v) );
}
//============================================================================================================
//	Klasse StepOutputAdapter:
//	Hatt mehrere Ausgaenge. Zordung des Input-Signals ist zustands abhaengig.
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void StepOutputAdapter::typeChanged ( void *src, const float &v ){
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
	cStep->resetLabel();
}
//------------------------------------------------------------------------------------------------------------
void StepOutputAdapter::init(){
	// Type Parameter:
	type->setName ("Step Type");
	type->setDisplay("fix");
	parameterMap.push_back (type);
	Parameter::ParameterListenerFunction f = boost::bind( 
			&StepOutputAdapter::typeChanged, this, _1, _2 
	);
	type->addValueChangedListenerF (f);
	// Adapter Nodes:
	for ( int i=0; i<steps; ++i ) {
		outputNodes[i]->setName("StepOutputNode["+MyString(i+1)+"]");
		if ( i<steps ) {
			parameterMap.push_back ( cStep->getParameter(i) );
			parameterMap.push_back ( cStep->Switch::getDurationParameterIN(i) );
			parameterMap.push_back ( cStep->Switch::getCurveTypeParameterIN(i) );
			parameterMap.push_back ( cStep->Switch::getDurationParameterOUT(i) );
			parameterMap.push_back ( cStep->Switch::getCurveTypeParameterOUT(i) );
		}
	}
}
//------------------------------------------------------------------------------------------------------------
ProcessorNode::Ptr StepOutputAdapter::addOutputNode(){
	TRY_TO_LOCK_TIMED (mutex);
	OutputNode::Ptr neu = ProcessAdapter::createOutputNode ( 
		"StepOutput outputNode("+MyString(getNumOutputNodes()+2) + ")"
	);
	cStep->addState();
	cStep->setNumSteps (++steps);
	parameterMap.push_back ( cStep->getParameter(steps-1) );
	parameterMap.push_back ( cStep->Switch::getDurationParameterIN(steps-1) );
	parameterMap.push_back ( cStep->Switch::getCurveTypeParameterIN(steps-1) );
	parameterMap.push_back ( cStep->Switch::getDurationParameterOUT(steps-1) );
	parameterMap.push_back ( cStep->Switch::getCurveTypeParameterOUT(steps-1) );
	outpMatrix.push_back ( NULL );
	cStep->reset();
	return neu;
}
//------------------------------------------------------------------------------------------------------------
StepOutputAdapter::StepOutputAdapter(IHostInfo *hostInfo, int initSteps) : 
ProcessAdapter( hostInfo, 1, initSteps ), 
steps(initSteps),
type ( Parameter::create() ),
outpMatrix ( OutputMatrix ( initSteps, (Frame*)NULL ) ),
fixTimeValue( hostInfo->getSampleRate() )
{
	setName ( "StepOutputAdapter" );
	getInputNode(0)->setName ( getName() + " InputNode");
	cStep = new Step( &fixTimeValue, initSteps, hostInfo->getSampleRate() );
	sync = new SyncTranslator ( hostInfo );
	init();
	cStep->setNumSteps (initSteps);
	TOLOG ( "+" + getName() );
}
//------------------------------------------------------------------------------------------------------------
void StepOutputAdapter::reset(){
  	cStep->reset();
	// ermittle anzahl der samples bis zu naechsten 1/4 note
	VstTimeInfo *inf = hostInfo->getVstTimeInfo( kVstPpqPosValid || kVstTempoValid );
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
inline void StepOutputAdapter::processFrames ( Frame *iFrame, OutputMatrix &fr, Processor::Int sampleFrames ) {
	VstNumber *l = (*iFrame)[0];
	VstNumber *r = (*iFrame)[1];
	for ( int i=0; i<sampleFrames; ++i ) {
		skimStepDuration();
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
void StepOutputAdapter::_processAdapter( Processor::Int sampleFrames ) {
	TRY_TO_LOCK_TIMED (mutex); // gleichzeitigen zugriff von addOutputNode blocken
	VstTimeInfo *inf = hostInfo->getVstTimeInfo(0);
	ClockEdge::EdgeValue t = transport.in ( isFlag(inf->flags,kVstTransportPlaying) );
	if (t == ClockEdge::HIGH ){ // Transport: play flanke
		reset();
	}
	Frame *frame = getInputNode(0)->popFrame();
	Frame iFrame;
	iFrame.copyIntoFrom ( *frame, sampleFrames );
	aNode->pushAndCopy ( frame, sampleFrames );
	for ( int i=0; i<steps; ++i ) { // bilde InputFrames auf Matrix ab.
		if ( !outputNodes[i]->isActive() ) {
			outpMatrix[i] = NULL;
			continue;
		}
		outpMatrix[i] = aNode->popFrame();
	}
	// berechne OutputFrames
	processFrames( &iFrame, outpMatrix, sampleFrames );
	for ( int i=0; i<steps; ++i ) outputNodes[i]->pushAndCopy ( outpMatrix[i], sampleFrames ); // knoten Frame zuweisen
}
//------------------------------------------------------------------------------------------------------------
StepOutputAdapter::~StepOutputAdapter(){
	delete cStep;
	delete sync;
	TOLOG ( "-" + getName() );
}
//------------------------------------------------------------------------------------------------------------
void StepOutputAdapter::save(com::oArchive &ar, const unsigned int version) const {
	ar << boost::serialization::base_object< ProcessAdapter > ( *this );
	ar << parameterMap;
	ar << type;
	ar << steps;
	ar << fixTimeValue;
	ar << sync;
	ar.register_type< FixTimeValue<10> >();
	ar << cStep;
}
//------------------------------------------------------------------------------------------------------------
void StepOutputAdapter::load(com::iArchive &ar, const unsigned int version) {
	ar >> boost::serialization::base_object< ProcessAdapter > ( *this );
	ar >> parameterMap;
	ar >> type;
	ar >> steps;
	ar >> fixTimeValue;
	ar >> sync;
	ar >> cStep;
	Parameter::ParameterListenerFunction f = boost::bind( 
			&StepOutputAdapter::typeChanged, this, _1, _2 
	);
	type->addValueChangedListenerF ( f );
	outpMatrix = OutputMatrix( steps, (Frame*)NULL );
}
//============================================================================================================
//	Klasse StepInputAdapter:
//	Hatt mehrere Ausgaenge. Zordung des Input-Signals ist zustands abhaengig.
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void StepInputAdapter::typeChanged ( void *src, const float &v ){
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
	cStep->resetLabel();
}
//------------------------------------------------------------------------------------------------------------
void StepInputAdapter::init(){
	// Type Parameter:
	type->setName ("Step Type");
	type->setDisplay("fix");
	parameterMap.push_back (type);
	Parameter::ParameterListenerFunction f = boost::bind( 
			&StepInputAdapter::typeChanged, this, _1, _2 
	);
	type->addValueChangedListenerF (f);
	// Adapter Nodes:
	for ( int i=0; i<steps; ++i ) {
		inputNodes[i]->setName("StepInputNode["+MyString(i+1)+"]");
		if ( i<steps ) {
			parameterMap.push_back ( cStep->getParameter(i));
			parameterMap.push_back ( cStep->Switch::getDurationParameterIN(i) );
			parameterMap.push_back ( cStep->Switch::getCurveTypeParameterIN(i) );
			parameterMap.push_back ( cStep->Switch::getDurationParameterOUT(i) );
			parameterMap.push_back ( cStep->Switch::getCurveTypeParameterOUT(i) );
		}
	}
}
//------------------------------------------------------------------------------------------------------------
ProcessorNode::Ptr StepInputAdapter::addInputNode(){
	TRY_TO_LOCK_TIMED (mutex);
	InputNode::Ptr neu = ProcessAdapter::createInputNode ( 
		"stepinput inputNode("+MyString(getNumInputNodes()+2) + ")"
	);
	cStep->addState();
	cStep->setNumSteps (++steps);
	parameterMap.push_back ( cStep->getParameter(steps-1) );
	parameterMap.push_back ( cStep->Switch::getDurationParameterIN(steps-1) );
	parameterMap.push_back ( cStep->Switch::getCurveTypeParameterIN(steps-1) );
	parameterMap.push_back ( cStep->Switch::getDurationParameterOUT(steps-1) );
	parameterMap.push_back ( cStep->Switch::getCurveTypeParameterOUT(steps-1) );
	inputMatrix.push_back ( NULL );
	cStep->reset();
	return neu;
}
//------------------------------------------------------------------------------------------------------------
StepInputAdapter::StepInputAdapter(IHostInfo *hostInfo, int initSteps) : 
ProcessAdapter( hostInfo, initSteps, 1 ), 
steps(initSteps),
type ( Parameter::create() ),
inputMatrix ( InputMatrix ( initSteps, (Frame*)NULL ) ),
fixTimeValue( hostInfo->getSampleRate() )
{
	setName ( "StepInputAdapter" );
	getOutputNode(0)->setName ( getName() + " OutputNode");
	cStep = new Step( &fixTimeValue, initSteps, hostInfo->getSampleRate() );
	sync = new SyncTranslator ( hostInfo );
	init();
	cStep->setNumSteps (initSteps);
	TOLOG ( "+" + getName() );
	tmpFrame.setSize ( hostInfo->getBlockSize() );
}
//------------------------------------------------------------------------------------------------------------
void StepInputAdapter::reset(){
	cStep->reset();
	// ermittle anzahl der samples bis zu naechsten 1/4 note
	VstTimeInfo *inf = hostInfo->getVstTimeInfo( kVstPpqPosValid || kVstTempoValid );
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
inline void StepInputAdapter::processFrames ( InputMatrix &fr, Processor::Int sampleFrames ) {
	VstNumber *l = tmpFrame[0];
	VstNumber *r = tmpFrame[1];
	for ( int i=0; i<sampleFrames; ++i ){
		*l = 0.0f; *r = 0.0f;
		if ( cStep->skimDuration() <= 0 ) {
			cStep->nextStep();
			cStep->setDuration();
		}
		for ( int j=0; j<cStep->getNumSteps(); j++ ){
			float fac = cStep->getFaderValueAndIncT(j); // mit jedem lesezugriff wird fader::t erhoet!
			if (!fr[j]) continue; // !!Wichtig
			 (*l) += (*fr[j])[0][i] * fac;
			 (*r) += (*fr[j])[1][i] * fac;
		}
		++l; ++r;
	}
}
//------------------------------------------------------------------------------------------------------------
void StepInputAdapter::_processAdapter( Processor::Int sampleFrames ) {
	TRY_TO_LOCK_TIMED (mutex); // gleichzeitigen zugriff von addOutputNode blocken
	VstTimeInfo *inf = hostInfo->getVstTimeInfo(0);
	ClockEdge::EdgeValue t = transport.in ( isFlag(inf->flags,kVstTransportPlaying) );
	if (t == ClockEdge::HIGH ){ // Transport: play flanke
		reset();
	}
	for ( int i=0; i<steps; ++i ) { // bilde InputFrames auf Matrix ab.
		if ( !inputNodes[i]->isActive() ) {
			inputMatrix[i] = NULL;
			continue;
		}
		inputMatrix[i] = inputNodes[i]->popFrame();
	}
	// berechne OutputFrames
	processFrames( inputMatrix, sampleFrames );
	outputNodes[0]->pushAndCopy ( &tmpFrame, sampleFrames );
}
//------------------------------------------------------------------------------------------------------------
StepInputAdapter::~StepInputAdapter(){
	delete cStep;
	delete sync;
	TOLOG ( "-" + getName() );
}
//------------------------------------------------------------------------------------------------------------
void StepInputAdapter::save(com::oArchive &ar, const unsigned int version) const {
	ar << boost::serialization::base_object< ProcessAdapter > ( *this );
	ar << parameterMap;
	ar << type;
	ar << steps;
	ar << fixTimeValue;
	ar << sync;
	ar.register_type< FixTimeValue<10> >();
	ar << cStep;
}
//------------------------------------------------------------------------------------------------------------
void StepInputAdapter::load(com::iArchive &ar, const unsigned int version) {
	ar >> boost::serialization::base_object< ProcessAdapter > ( *this );
	ar >> parameterMap;
	ar >> type;
	ar >> steps;
	ar >> fixTimeValue;
	ar >> sync;
	ar >> cStep;
	tmpFrame.setSize ( hostInfo->getBlockSize() );
	Parameter::ParameterListenerFunction f = boost::bind( 
			&StepInputAdapter::typeChanged, this, _1, _2 
	);
	type->addValueChangedListenerF ( f );
	inputMatrix = InputMatrix( steps, (Frame*)NULL );
}
//============================================================================================================
//PeakTracker
//Tranformiert Signal Lautstaerke in Parameter wert.
//============================================================================================================
PeakTracker::PeakTracker( IHostInfo *hostInfo ) : ProcessAdapter(hostInfo,1,1) {
	setName ("PeakTracker");
	inputNodes[0]->setName  ( getName() + " InputNode");
	outputNodes[0]->setName ( getName() + " OutputNode");
	out = Parameter::create();
	*out = 0.0f;
	out->setName ("peak tracker output");
	offset = Parameter::create();
	offset->setName ("offset");
	TOLOG ( "+" + getName() );
}
//------------------------------------------------------------------------------------------------------------
void PeakTracker::_processAdapter( Processor::Int sampleFrames ) {
	Frame *frame = getInputNode(0)->popFrame();
	VstInt32 i = sampleFrames;
	VstNumber *r = (*frame)[0];
	VstNumber *l = (*frame)[1];
	VstNumber average = 0.0f;
	while ( --i >= 0 ){
		average += fabs( ( *(r) + *(l) )/2.0f );
		*(r++) = 0.0f;
		*(l++) = 0.0f;
	}
	average = average / (float)frame->getSize();
	*out = average + *offset;
	outputNodes[0]->pushAndCopy(frame, sampleFrames);
}
//============================================================================================================
//ADSRTrigger
//============================================================================================================
const string states[] = {"_a", "_d", "_s", "_r"};
//------------------------------------------------------------------------------------------------------------
ADSRTrigger::ADSRTrigger( IHostInfo *hostInfo ) : ProcessAdapter(hostInfo) {
	setName ( "ADSRTrigger" );
	inputNodes[0]->setName  ( getName() + " InputNode");
	outputNodes[0]->setName ( getName() + " OutputNode");
	out = Parameter::create();
	*out = 0.0f;
	out->setName ("ADSR Output");
	adsr = new ADSR (hostInfo, 5.0f);
	TOLOG ( "+" + getName() );
}
//------------------------------------------------------------------------------------------------------------
void ADSRTrigger::_processAdapter( Processor::Int sampleFrames ) {
	Frame *frame = getInputNode(0)->popFrame();
	VstInt32 i = sampleFrames;
	VstNumber *r = (*frame)[0];
	VstNumber *l = (*frame)[1];
	VstNumber average = 0.0f;
	while ( --i >= 0 ){
		average += fabs( ( *(r) + *(l) )/2.0f );
		*(r++) = 0.0f;
		*(l++) = 0.0f;
		adsr->process();
	}
	average = average / (float)frame->getSize();
	// fuetter adsr
	adsr->setInput ( average );
	*out = com::getMin<VstNumber>( 1.0f, com::getMax<VstNumber> ( 0.0f, adsr->process() ) );
	out->setLabel ( states[ adsr->getState() ] );
	// outputnode
	outputNodes[0]->pushAndCopy(frame, sampleFrames);
}
//============================================================================================================
// MidiProcessor:
// One MidiProcessor per channel.
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
MidiProcessor::MidiProcessor ( IHostInfo *iHost ) : ProcessAdapter( iHost, 0, 0 ), midiParameters( NUM_OUT_PARAM, Parameter::Ptr() )
{
	setName ("midi_receiver");
	initParameter ( PITCH_BEND , "pitchbend" );
	midiParameters[PITCH_BEND]->setValue(0.5f);
	for ( size_t i=NUM_NO_CC_PARAMETER; i<NUM_OUT_PARAM; ++i ) {
		size_t c = i - NUM_NO_CC_PARAMETER;
		initParameter ( i, "cc(" + MyString(c) + ")" + musicalValues::getCCName(c) );
	}
	TOLOG ( "+" + getName() );
}
//------------------------------------------------------------------------------------------------------------
void MidiProcessor::processMidiEvents ( VstEvents *ev ) {
	for ( size_t i=0; i<ev->numEvents; ++i ) {
		VstMidiEvent *midiEv = (VstMidiEvent*)ev->events[i];
		Byte status = ( midiEv->midiData[0] & 0xf0 ) >> 4;
		size_t gv=0, cc=0;
		switch ( status ) {
			case 0xE /*pitchbend*/ :
				gv = ( midiEv->midiData[2] << 7 ) | midiEv->midiData[1];
				midiParameters[PITCH_BEND]->setValue( gv / 16383.0f );
				break;
			case 0xB /*cc*/        :
				cc = midiEv->midiData[1];
				if ( cc >= NUM_CC ) continue;
				gv = midiEv->midiData[2];
				midiParameters[ cc + NUM_NO_CC_PARAMETER  ]->setValue( gv/127.0f );
				break;
		}
	}
}
}//namespace processing
