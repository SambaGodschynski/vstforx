                                        /*
 * ===========================================================================================================
 * InputSwitch.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#include "InputSwitch.h"


namespace processing{
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
Switch ( initStates, hostInfo->getSampleRate() ), inputMatrix( InputMatrix(initStates, (Frames*)NULL) )
{
	setName ( "InputSwitch" );
	selector = Parameter::create();
	selector->setName ("selector switch");
	selector->addValueChangedListener ( 
		boost::bind(&InputSwitch::valueChanged, this, _1, _2)
	);
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
inline void InputSwitch::_processFrames ( InputMatrix &fr, Processor::Int numSamples ) {
	VstNumber *l = tmpFrame[0];
	VstNumber *r = tmpFrame[1];
	for ( int i=0; i<numSamples; ++i ){
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
void InputSwitch::processAdapter( Processor::Int numSamples ) {
	TRY_TO_LOCK_TIMED (mutex); // gleichzeitigen zugriff von addOutputNode blocken

	for ( int i=0; i<getNumStates(); ++i ) { // bilde InputFrames auf Matrix ab.
		if ( !inputNodes[i]->isActive() ) {
			inputMatrix[i] = NULL;
			continue;
		}
		inputMatrix[i] = inputNodes[i]->popFrame();
	}
	// berechne OutputFrames
	_processFrames( inputMatrix, numSamples );
	outputNodes[0]->pushAndCopy ( &tmpFrame, numSamples );
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
	selector->addValueChangedListener ( 
		boost::bind(&InputSwitch::valueChanged, this, _1, _2)
	);

}
}// namespace processing
