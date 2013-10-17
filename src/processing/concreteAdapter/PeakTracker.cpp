                                        /*
 * ===========================================================================================================
 * PeakTracker.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#include "PeakTracker.h"

namespace {
    // check if inertia parameter connection is working after
    // changing this value
    const unsigned int FRX_REFRESHING_SAMPLES = 1024;
}

namespace processing{
//============================================================================================================
//PeakTracker
//Tranformiert Signal Lautstaerke in Parameter wert.
//============================================================================================================
PeakTracker::PeakTracker( frx::processing::IHostInfo::Ptr hostInfo ) :
    ProcessAdapter(hostInfo,1,1), sampleCounter(0)
{
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
void PeakTracker::processAdapter( Processor::Int numSamples ) {
	Frames *frame = getInputNode(0)->popFrame();
	int i = numSamples;
	com::VstNumber *r = (*frame)[0];
	com::VstNumber *l = (*frame)[1];
    com::VstNumber signalAverage = 0.f;
	while ( --i >= 0 ){
		signalAverage += fabs( ( *(r) + *(l) )/2.0f );
		*(r++) = 0.0f;
		*(l++) = 0.0f;
        ++sampleCounter;
	}
	signalAverage = signalAverage / (float)frame->getSize();
    if (sampleCounter >= FRX_REFRESHING_SAMPLES) {
        sampleCounter = 0;
        *out = signalAverage + *offset;
    }
	outputNodes[0]->pushAndCopy(frame, numSamples);
}
}// namespace processing
