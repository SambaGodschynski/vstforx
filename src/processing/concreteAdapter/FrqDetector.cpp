/*
 * ============================================================================
 * FrqDetector.cpp
 *      Author: Johannes Unger
 * ============================================================================
 */
#include "FrqDetector.hpp"
#include <processing/ModelFactory.hpp>
#include "processing/dspTools.h"
#include <sstream>

namespace processing{
//=============================================================================
//FrqDetector
//=============================================================================
//-----------------------------------------------------------------------------
const double FrqDetector::MaxFrq = 2000;
//-----------------------------------------------------------------------------
void FrqDetector::init() {
    worker = NULL;
    running = false;
    buffer = Buffer(MaxBuffer);
}
//-----------------------------------------------------------------------------
void FrqDetector::startWorker() {
    if (worker!=NULL || running) {
        return;
    }
    running = true;
    worker = new boost::thread(boost::bind(&FrqDetector::doWork, this));
}
//-----------------------------------------------------------------------------
void FrqDetector::stopWorker() {
    if (worker==NULL || !running) {
        return;
    }
    running = false;
    worker->join();
    delete worker;
    worker = NULL;
}
//-----------------------------------------------------------------------------
void FrqDetector::hostBaseConfigChanged() {
    sampleRate = getHostInfo()->getSampleRate();
}
//-----------------------------------------------------------------------------
void FrqDetector::serialize ( ::com::iArchive &ar, const unsigned int version ){
	ar >> boost::serialization::base_object < ProcessAdapter > ( *this );
	ar >> frq;
}
//-----------------------------------------------------------------------------
void FrqDetector::serialize ( ::com::oArchive &ar, const unsigned int version ){
	ar << boost::serialization::base_object < ProcessAdapter > ( *this );
	ar << frq;
}
//-----------------------------------------------------------------------------
FrqDetector::FrqDetector( frx::processing::IHostInfo::Ptr hostInfo ) :
    ProcessAdapter(hostInfo)
{
    init();
    sampleRate = hostInfo->getSampleRate();
	setName ( "FrqDetector" );
	inputNodes[0]->setName  ( getName() + " InputNode");
	outputNodes[0]->setName ( getName() + " OutputNode");
	frq = Parameter::create();
	frq->setReadOnly(true);
	*frq = 0.0f;
	frq->setName ("Frq");
}
//-----------------------------------------------------------------------------
void FrqDetector::updateDisplay(double f) {
    std::stringstream ss;
    ss<<f<<" Hz";
    frq->setDisplay(ss.str());
}
//-----------------------------------------------------------------------------
void FrqDetector::doWork() {
    while(running) {
        if(buffer.size() < WindowSize) {
            sleep(Idle);
            continue;
        }
        T tmp[WindowSize];
        for (size_t i=0; i<WindowSize; ++i) {
            SAMBAG_BEGIN_SYNCHRONIZED(mutex)
               tmp[i] = buffer.front();
               buffer.pop_front();
            SAMBAG_END_SYNCHRONIZED
        }
        double f = detectFrequency(&tmp[0], sampleRate, WindowSize);
        *frq = f / MaxFrq;
        updateDisplay(f);
        sleep(SleepTime);
    }
}
//-----------------------------------------------------------------------------
void FrqDetector::processAdapter( Processor::Int numSamples ) {
    if (!running) {
        startWorker();
    }
	Frames &frame = *(getInputNode(0)->popFrame());
    for (size_t i=0; i<numSamples; ++i) {
        // mix stereo to one
        T value = (frame[0][i] + frame[1][i]) / 2.0;
        SAMBAG_BEGIN_SYNCHRONIZED(mutex)
            buffer.push_back(value);
        SAMBAG_END_SYNCHRONIZED
        // silence
        frame[0][i] = frame[1][i] = 0.;
    }
	// outputnode
	outputNodes[0]->pushAndCopy(&frame, numSamples);
}
//-----------------------------------------------------------------------------
FrqDetector::~FrqDetector () {
    try {
        stopWorker();
    } catch(...) {
    }
 }
}// namespace processing
