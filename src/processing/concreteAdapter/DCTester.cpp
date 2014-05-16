/*
 * ============================================================================
 * DCTester
 *      Author: Johannes Unger
 * ============================================================================
 */

#include "DCTester.hpp"
namespace {

    const size_t DCTesterMaxDelayFrames = 32768;

}
namespace processing{
//=============================================================================
// class DCTester
//=============================================================================
//-----------------------------------------------------------------------------
void DCTester::processAdapter( Processor::Int numSamples ) {
	Frames *fr = getInputNode(0)->popFrame();
    size_t delayframes = DCTester::getProcessDelay();
    stream.addFrame(fr, numSamples, delayframes);
    stream.flush(numSamples, fr->getData());
	outputNodes[0]->pushAndCopy( fr, numSamples );
}
//-----------------------------------------------------------------------------
void DCTester::valueChanged ( void *src, const float &value ) {
    namespace sce=sambag::com::events;
    sce::EventSender<sce::PropertyChanged>::notifyListeners(this,
        sce::PropertyChanged("process delay", (size_t)0, getProcessDelay())
    );
}
//-----------------------------------------------------------------------------
void DCTester::setupStream(frx::processing::IHostInfo::Ptr hI) {
    stream.setSize(hI->getBlockSize(), DCTesterMaxDelayFrames);
}
//-----------------------------------------------------------------------------
void DCTester::hostBaseConfigChanged() {
    frx::processing::IHostInfo::Ptr hI = hostInfo.lock();
    if (!hI) {
        SAMBAG_THROW(sambag::com::exceptions::IllegalStateException,
            "Hostinfo == NULL"
        );
    }
    setupStream(hI);
}
//-----------------------------------------------------------------------------
size_t DCTester::getProcessDelay() const {
    return (size_t)(*delay * (float)DCTesterMaxDelayFrames);
}
}// namespace processing

