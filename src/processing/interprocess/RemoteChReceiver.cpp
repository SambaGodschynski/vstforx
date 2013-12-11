/*
 * RemoteChReceiver.cpp
 *
 *  Created on: Tue Sep 17 21:10:30 2013
 *      Author: Johannes Unger
 */

#include "RemoteChReceiver.hpp"
#include "RemoteChannelManager.hpp"
#include "Stream.hpp"
#include <fstream>
#include <sambag/com/exceptions/IllegalStateException.hpp>

namespace {
    const int PARAM_OBSERVER_INTERVAL_MS=30;
    const int REOPEN_STREAM_INTERVAL_MS=2000;
	const int AUDIO_BUFFER_RESERVE_FACTOR=3;
}

namespace frx { namespace processing { namespace interprocess {
//=============================================================================
//  Class RemoteChReceiver
//=============================================================================
//-----------------------------------------------------------------------------
RemoteChReceiver::RemoteChReceiver(frx::processing::IHostInfo::Ptr hostInfo,
                                   const std::string &rcId,
                                   size_t numOutputs) :
    pr::ProcessAdapter( hostInfo, 0, numOutputs ),
    blocksRead(Stream::UndefinedNumBlocks),
    streamId(rcId)
{
    setName ("RemoteChannel.Receiver");
}
//-----------------------------------------------------------------------------
std::string RemoteChReceiver::getStatusMessage() const {
    if (!errMsg.empty()) {
        return errMsg;
    }
    return ipStream ? "[connected]" : "[sender not available]";
}
//-----------------------------------------------------------------------------
void RemoteChReceiver::setAudioSettings(frx::processing::IHostInfo::Ptr hI) {
    size_t bs = hI->getBlockSize();
    frames.setSize( bs );
    frames.setZero( bs );
}
//-----------------------------------------------------------------------------
void RemoteChReceiver::openStream() {
    RemoteChannelManager &rm = RemoteChannelManager::instance();
    ipStream = rm.getStream(streamId);
    if (!ipStream) {
        SAMBAG_THROW(sambag::com::exceptions::IllegalStateException,
            "stream == NULL");
    }
    setAudioSettings(getHostInfo());
    initParameters(ipStream->getNumParameter());
    try {
        initStream();
    } catch ( const std::exception &ex ) {
        errMsg = ex.what();
        streamLost();
    }
}
//-----------------------------------------------------------------------------
void RemoteChReceiver::notifyStatusChanged() {
    sce::EventSender<sce::PropertyChanged>::notifyListeners(
        this, sce::PropertyChanged("status message", std::string(), getStatusMessage())
    );
}
//-----------------------------------------------------------------------------
void RemoteChReceiver::reOpenStream() {
    if (ipStream) {
        return;
    }
    frx::processing::IHostInfo::Ptr hostInfo = this->hostInfo.lock();
    RemoteChannelManager &rm = RemoteChannelManager::instance();
    
    ipStream = rm.getStream(streamId);
    
    if (ipStream) {
        try {
            initStream();
            initParameterObserver();
        } catch (const std::exception &ex) {
            errMsg = ex.what();
            ipStream.reset();
        }
    }
    
    if (!ipStream) {
        if (!openStreamTimer) {
            openStreamTimer = FrxAsyncDSPTimer::create(REOPEN_STREAM_INTERVAL_MS);
            openStreamTimer->addTrackedEventListener(
                boost::bind(&RemoteChReceiver::reOpenStream, this),
                getPtr()
            );
        }
        openStreamTimer->stop();
        openStreamTimer->start();
        notifyStatusChanged();
        return; // come back later
    } else {
        if (openStreamTimer) {
            openStreamTimer->stop();
            openStreamTimer.reset();
        }
    }
 }
//-----------------------------------------------------------------------------
void RemoteChReceiver::initStream() {
    if (!ipStream) {
        SAMBAG_THROW(sambag::com::exceptions::IllegalStateException,
        "stream == NULL");
    }
    frx::processing::IHostInfo::Ptr hostInfo = this->hostInfo.lock();
    size_t sBs = ipStream->getBlockSize();
    size_t hBs = hostInfo->getBlockSize();
    if (sBs!=hBs) {
        std::stringstream ss;
        ss<<"Different Blocksizes: sender("<<sBs<<"), receiver("<<hBs<<")";
        SAMBAG_THROW(sambag::com::exceptions::IllegalStateException,
        ss.str());
    }
    if (!errMsg.empty()) {
        errMsg.clear();
    }
    notifyStatusChanged();
}
//-----------------------------------------------------------------------------
void RemoteChReceiver::streamLost() {
    SAMBAG_TRY_TO_LOCK_RECURSIVE(mutex);
    ipStream.reset();
    reOpenStream();
}
//-----------------------------------------------------------------------------
void RemoteChReceiver::processAdapter( pr::Processor::Int numSamples ) {
    using namespace ::processing;
    if (!ipStream) {
        _nullProcess(numSamples);
        return;
    }
    // reading from ip stream
    float **data = frames.getData();
    int res = ipStream->read(data, blocksRead);
    if (res<0) { // we are behind the last written block
        blocksRead-=res; // blocksRead + numBlocksBehind (res is negative)
        ipStream->read(data, blocksRead);
    }
    if (res>0) { // we are before the last written block
		if (abff.size() >= numSamples) { // we have audiodata left
			abff.readOut(data, numSamples);
			outputNodes[0]->pushAndCopy(&frames, numSamples);
			return;
		}
        if (::time(NULL) - ipStream->getLastWrittenTime() >= 1)
        {
            streamLost();
            _nullProcess(numSamples);
            return;
        }
        blocksRead-=res; // blocksRead + numBlocksBefore (res is positive)
        ipStream->read(data, blocksRead);
    }
    size_t bs = getHostInfo()->getBlockSize();
	if (numSamples==bs) {
		outputNodes[0]->pushAndCopy( &frames, numSamples );
		return;
	}
	// SPECIAL CASE: numsamples < blocksize. Fruity Loops has such behaviour
	if (abff.getCapacity() < bs) { // prepare buffer if needed
		abff.setCapacity(bs*AUDIO_BUFFER_RESERVE_FACTOR);
	}

	// TODO: why does this not work?
	// float *tmp[] = { &data[0][numSamples], &data[1][numSamples] };
    // abff.writeIn(&tmp[0], bs-numSamples-1);
	// outputNodes[0]->pushAndCopy( &frames, numSamples );
	// return;

    // write whole block into buffer
    abff.writeIn(data, bs);
    // read numsamples from buffer
	abff.readOut(data, numSamples);
    outputNodes[0]->pushAndCopy( &frames, numSamples );
}
//-----------------------------------------------------------------------------
RemoteChReceiver::Ptr
RemoteChReceiver::create(frx::processing::IHostInfo::Ptr hostInfo,
    const std::string &rcId)
{
    RemoteChannelManager &rm = RemoteChannelManager::instance();
    Stream::Ptr ipStream = rm.getStream(rcId);
    if (!ipStream) {
        return RemoteChReceiver::Ptr();
    }
    size_t numChannels = ipStream->getNumChannels();
    if (numChannels==0) {
        return RemoteChReceiver::Ptr();
    }
    size_t numOutputs = ::com::numChannels2Xputs(numChannels);
    Ptr neu( new RemoteChReceiver(hostInfo, rcId, numOutputs) );
    neu->self = neu;
    neu->openStream();
    neu->initParameterObserver();
    return neu;
}
//-----------------------------------------------------------------------------
void RemoteChReceiver::initParameterObserver() {
    parameterObserver = FrxAsyncDSPTimer::create(PARAM_OBSERVER_INTERVAL_MS);
    parameterObserver->setNumRepetitions(-1);
    parameterObserver->addTrackedEventListener(
        boost::bind(&RemoteChReceiver::onParameterObserver, this),
        getPtr()
    );
    parameterObserver->start();
}
//-----------------------------------------------------------------------------
void RemoteChReceiver::onParameterObserver()
{
    if (!ipStream) {
        return;
    }
    SAMBAG_TRY_TO_LOCK_RECURSIVE(mutex);
    if(ipStream->getNumParameter()!=parameters.size()) {
        return;
    }
    
    Stream::ValueType *pvalue = ipStream->getParameter();
    size_t n = ipStream->getNumParameter();
    size_t i = 0;
    while(n-- > 0) {
        if (parameters[i]->getValue() != *pvalue) {
            parameters[i]->setValue(*(pvalue));
        }
        ++i;
        ++pvalue;
    }
}
//-----------------------------------------------------------------------------
void RemoteChReceiver::initParameters(size_t num) {
    parameters.reserve(num);
    for (size_t i=0; i<num; ++i) {
        prp::Parameter::Ptr p = prp::Parameter::create();
        p->setName("receiver.param"+sambag::com::toString(i+1));
        parameters.push_back(p);
    }
}
//-----------------------------------------------------------------------------
prp::Parameter::Ptr RemoteChReceiver::getParameter (size_t index) const
{
    return parameters.at(index);
}
//-----------------------------------------------------------------------------
size_t RemoteChReceiver::getNumParameter () const {
    return parameters.size();
}
//-----------------------------------------------------------------------------
size_t RemoteChReceiver::getProcessDelay() const {
	if (!getHostInfo()) {
		return 0;
	}
	return getHostInfo()->getBlockSize();
}
}}} // namespace(s)