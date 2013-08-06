                                        /*
 * ===========================================================================================================
 * ADSRTrigger.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#include "ADSRTrigger.h"

namespace {
    const long FRX_REFRESH_RATE = 30;
}

namespace processing{
//============================================================================================================
//ADSRTrigger
//============================================================================================================
const string states[] = {"_a", "_d", "_s", "_r"};
//------------------------------------------------------------------------------------------------------------
void ADSRTrigger::timerCallback(void*, const Timer::Event &ev) {
	*out = com::getMin<VstNumber>( 1.0f, com::getMax<VstNumber> ( 0.0f, adsr->process() ) );
	out->setLabel ( states[ adsr->getState() ] );
}
//------------------------------------------------------------------------------------------------------------
void ADSRTrigger::initTimerIfNeeded() {
    if (!timer) {
        timer = Timer::create(FRX_REFRESH_RATE);
        timer->setNumRepetitions(-1);
        timer->addTrackedEventListener(
            boost::bind(&ADSRTrigger::timerCallback, this, _1, _2),
            getPtr()
        );
        timer->start();
    }
}
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
}
//------------------------------------------------------------------------------------------------------------
void ADSRTrigger::processAdapter( Processor::Int numSamples ) {
    initTimerIfNeeded();
	Frames *frame = getInputNode(0)->popFrame();
	int i = numSamples;
	VstNumber *r = (*frame)[0];
	VstNumber *l = (*frame)[1];
	VstNumber signalAverage = 0.0f;
	while ( --i >= 0 ){
		signalAverage += fabs( ( *(r) + *(l) )/2.0f );
		*(r++) = 0.0f;
		*(l++) = 0.0f;
		adsr->process();
	}
	signalAverage = signalAverage / (float)frame->getSize();
    adsr->setInput ( signalAverage );
	// outputnode
	outputNodes[0]->pushAndCopy(frame, numSamples);
}
//------------------------------------------------------------------------------------------------------------
ADSRTrigger::~ADSRTrigger () { 
	delete adsr; 
}
}// namespace processing
