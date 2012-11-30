                                        /*
 * ===========================================================================================================
 * PeakTracker.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#include "PeakTracker.h"


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
	*out = 0.0f;
	out->setName ("peak tracker output");
	offset = Parameter::create();
	offset->setName ("offset");
	TOLOG ( "+" + getName() );
}
//------------------------------------------------------------------------------------------------------------
void PeakTracker::processAdapter( Processor::Int numSamples ) {
	Frames *frame = getInputNode(0)->popFrame();
	int i = numSamples;
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
	outputNodes[0]->pushAndCopy(frame, numSamples);
}
}// namespace processing
