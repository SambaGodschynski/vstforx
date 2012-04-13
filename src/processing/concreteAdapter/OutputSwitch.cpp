                                        /*
 * ===========================================================================================================
 * OutputSwitch.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#include "OutputSwitch.h"


namespace processing{
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
Switch ( initStates, hostInfo->getSampleRate() ), outpMatrix( OutputMatrix(initStates, (Frames*)NULL) )
{
	setName ( "OutputSwitch" );
	selector = Parameter::create();
	selector->setName ("selector switch");
	selector->addValueChangedListener ( 
		boost::bind(&OutputSwitch::valueChanged, this, _1, _2)
	);
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
inline void OutputSwitch::_processFrames ( Frames *iFrame, OutputMatrix &fr, Processor::Int numSamples ) {
	VstNumber *l = (*iFrame)[0];
	VstNumber *r = (*iFrame)[1];
	for ( int i=0; i<numSamples; ++i ){
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
void OutputSwitch::processAdapter( Processor::Int numSamples ) {
	TRY_TO_LOCK_TIMED (mutex); // gleichzeitigen zugriff von addOutputNode blocken
	Frames *frame = getInputNode(0)->popFrame();
	Frames iFrame;
	iFrame.copyIntoFrom ( *frame, numSamples );
	aNode->pushAndCopy ( frame, numSamples );
	size_t steps = getNumStates();
	for ( int i=0; i<steps; ++i ) { // bilde InputFrames auf Matrix ab.
		if ( !outputNodes[i]->isActive() ) {
			outpMatrix[i] = NULL;
			continue;
		}
		outpMatrix[i] = aNode->popFrame();
	}
	// berechne OutputFrames
	_processFrames( &iFrame, outpMatrix, numSamples );
	for ( int i=0; i<steps; ++i ) {
		if ( !outputNodes[i]->isActive() ) continue;
		outputNodes[i]->pushAndCopy ( outpMatrix[i], numSamples ); // knoten Frames zuweisen
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
	selector->addValueChangedListener ( 
		boost::bind(&OutputSwitch::valueChanged, this, _1, _2)
	);

}
}// namespace processing
