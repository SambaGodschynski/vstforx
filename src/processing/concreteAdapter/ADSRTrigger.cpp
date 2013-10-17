                                        /*
 * ===========================================================================================================
 * ADSRTrigger.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#include "ADSRTrigger.h"

namespace {
    // check if inertia parameter connection is working after
    // changing this value
    const unsigned int FRX_REFRESHING_SAMPLES = 2048;
}


namespace processing{
//============================================================================================================
//ADSRTrigger
//============================================================================================================
const std::string states[] = {"_a", "_d", "_s", "_r"};
//------------------------------------------------------------------------------------------------------------
ADSRTrigger::ADSRTrigger( frx::processing::IHostInfo::Ptr hostInfo ) :
    ProcessAdapter(hostInfo), sampleCounter(0)
{
	setName ( "ADSRTrigger" );
	inputNodes[0]->setName  ( getName() + " InputNode");
	outputNodes[0]->setName ( getName() + " OutputNode");
	out = Parameter::create();
	out->setReadOnly(true);
	*out = 0.0f;
	out->setName ("ADSR Output");
	adsr = new ADSR (hostInfo, 5.0f);
}
//------------------------------------------------------------------------------------------------------------
void ADSRTrigger::processAdapter( Processor::Int numSamples ) {
	Frames *frame = getInputNode(0)->popFrame();
	int i = numSamples;
	com::VstNumber *r = (*frame)[0];
	com::VstNumber *l = (*frame)[1];
	com::VstNumber signalAverage = 0.0f;
	while ( --i >= 0 ){
		signalAverage += fabs( ( *(r) + *(l) )/2.0f );
		*(r++) = 0.0f;
		*(l++) = 0.0f;
		adsr->process();
        ++sampleCounter;
	}
	signalAverage = signalAverage / (float)frame->getSize();
    adsr->setInput ( signalAverage );
    
    if (sampleCounter >= FRX_REFRESHING_SAMPLES) {
        sampleCounter = 0;
        *out = com::getMin<com::VstNumber>( 1.0f, com::getMax<com::VstNumber> ( 0.0f, adsr->process() ) );
        out->setLabel ( states[ adsr->getState() ] );
    }
    
	// outputnode
	outputNodes[0]->pushAndCopy(frame, numSamples);
}
//------------------------------------------------------------------------------------------------------------
ADSRTrigger::~ADSRTrigger () { 
	delete adsr; 
}
}// namespace processing
