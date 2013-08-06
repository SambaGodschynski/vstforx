                                        /*
 * ===========================================================================================================
 * PeakTracker.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#include "PeakTracker.h"

namespace {
    const long FRX_REFRESH_RATE = 30;
}

namespace processing{
//============================================================================================================
//PeakTracker
//Tranformiert Signal Lautstaerke in Parameter wert.
//============================================================================================================
PeakTracker::PeakTracker( frx::processing::IHostInfo::Ptr hostInfo ) : ProcessAdapter(hostInfo,1,1) {
	setName ("PeakTracker");
	inputNodes[0]->setName  ( getName() + " InputNode");
	outputNodes[0]->setName ( getName() + " OutputNode");
	out = Parameter::create();
	out->setReadOnly(true);
	*out = 0.0f;
	out->setName ("peak tracker output");
	offset = Parameter::create();
	offset->setName ("offset");
}
//------------------------------------------------------------------------------------------------------------
void PeakTracker::timerCallback(void*, const Timer::Event &ev) {
    *out = signalAverage + *offset;
}
//------------------------------------------------------------------------------------------------------------
void PeakTracker::initTimerIfNeeded() {
    if (!timer) {
        timer = Timer::create(FRX_REFRESH_RATE);
        timer->setNumRepetitions(-1);
        timer->addTrackedEventListener(
            boost::bind(&PeakTracker::timerCallback, this, _1, _2),
            getPtr()
        );
        timer->start();
    }
}
//------------------------------------------------------------------------------------------------------------
void PeakTracker::processAdapter( Processor::Int numSamples ) {
    initTimerIfNeeded();
	Frames *frame = getInputNode(0)->popFrame();
	int i = numSamples;
	VstNumber *r = (*frame)[0];
	VstNumber *l = (*frame)[1];
    signalAverage = 0.f;
	while ( --i >= 0 ){
		signalAverage += fabs( ( *(r) + *(l) )/2.0f );
		*(r++) = 0.0f;
		*(l++) = 0.0f;
	}
	signalAverage = signalAverage / (float)frame->getSize();
	outputNodes[0]->pushAndCopy(frame, numSamples);
}
}// namespace processing
