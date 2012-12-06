                                        /*
 * ===========================================================================================================
 * ADSRTrigger.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#include "ADSRTrigger.h"


namespace processing{
//============================================================================================================
//ADSRTrigger
//============================================================================================================
const string states[] = {"_a", "_d", "_s", "_r"};
//------------------------------------------------------------------------------------------------------------
ADSRTrigger::ADSRTrigger( frx::processing::IHostInfo::Ptr hostInfo ) : ProcessAdapter(hostInfo) {
	setName ( "ADSRTrigger" );
	inputNodes[0]->setName  ( getName() + " InputNode");
	outputNodes[0]->setName ( getName() + " OutputNode");
	out = Parameter::create();
	out->setReadOnly(true);
	*out = 0.0f;
	out->setName ("ADSR Output");
	adsr = new ADSR (hostInfo, 5.0f);
	TOLOG ( "+" + getName() );
}
//------------------------------------------------------------------------------------------------------------
void ADSRTrigger::processAdapter( Processor::Int numSamples ) {
	Frames *frame = getInputNode(0)->popFrame();
	int i = numSamples;
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
	outputNodes[0]->pushAndCopy(frame, numSamples);
}
}// namespace processing
