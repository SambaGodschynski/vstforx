/*
 * Plugin.cpp
 *
 *  Created on: Mon Oct  8 12:58:07 2012
 *      Author: Johannes Unger
 */

#include "Plugin.hpp"
#include <stdlib.h>
#include <sstream>
#include <boost/tuple/tuple.hpp>
#include <sambag/com/Common.hpp>
#include <cstring>
#include <processing/interprocess/RemoteChannelManager.hpp>
#include <com/one4All.h>

extern void globAddRemoteChannelSender(size_t);

namespace {
	frx::processing::FrxAsyncDSPTimer::WorkerThreadHolder _timerThreadHolder;
	int _instances = 0;

	std::string createName() {
		std::stringstream ss;
		struct tm * timeinfo;
		char buffer [80];
		time_t rawtime;
		time (&rawtime);
		timeinfo = localtime (&rawtime);
		strftime (buffer,80,"%m-%d-%y++%H:%M:%S",timeinfo);
		ss<<globGetProductName()<<"_"<<buffer;
		return ss.str();
	}
}

namespace frx { namespace processing { namespace remoteChannel {
using namespace interprocess;

//=============================================================================
// class Plugin 
//=============================================================================
//-----------------------------------------------------------------------------
Plugin::Plugin() : 
blockSize(0),
sampleRate(0.f),
trackingDummyPtr(new TrackingDummy())
{
	using namespace frx::processing;
	if (_instances++ == 0) {
		_timerThreadHolder = 
			FrxAsyncDSPTimer::startWorkerThread();
	}
}
//-----------------------------------------------------------------------------
Plugin::~Plugin() {
	using ::frx::processing::interprocess::RemoteChannelManager;
	if (--_instances == 0) {
		frx::processing::FrxAsyncDSPTimer::closeAllTimer();
		_timerThreadHolder.reset();
	}
}
//-----------------------------------------------------------------------------
void Plugin::open() {
}	
//-----------------------------------------------------------------------------
void Plugin::close() {
    destroyStream();
    if (ioChangedTimer) {
        ioChangedTimer->stop();
    }
    trackingDummyPtr.reset();
	ioChangedTimer.reset();
}
//-----------------------------------------------------------------------------
void Plugin::destroyStream() {
    using namespace interprocess;
    if (channelId.empty()) {
        return;
    }
    RemoteChannelManager &rm = RemoteChannelManager::instance();
    rm.removeChannel(channelId);
    stream.reset();
}
//-----------------------------------------------------------------------------
void Plugin::process(float **in, float **out, int numSamples) {
    SAMBAG_TRY_TO_LOCK_TIMED(mutex);
    if (!stream) {
        return;
    }
    stream->write(in, numSamples);
    // write into delay stream
	dcStream.add(in, numSamples, blockSize);
	// and back
	dcStream.flush(numSamples, out);
}
//-----------------------------------------------------------------------------
void Plugin::processEvents(sambag::dsp::IMidiEvents::Ptr ev) {
}
//-----------------------------------------------------------------------------
namespace {
	void __onIOChanged(sambag::dsp::IHost *host, size_t blockSize) 
	{
		host->delayChanged(blockSize);
	}
}
void Plugin::setBlockSize(int blockSize) {
	using namespace frx::processing;
    using frx::processing::FrxAsyncDSPTimer;
    using sambag::com::events::EventSender;
	this->blockSize = blockSize;
	updateConfiguration();
	dcStream.setSize(blockSize, blockSize);
	if (!ioChangedTimer) {
		ioChangedTimer = FrxAsyncDSPTimer::create(500);
		ioChangedTimer->EventSender<FrxAsyncDSPTimer::Event>::
		addTrackedEventListener(
			boost::bind( &__onIOChanged, getHost(),  blockSize),
            trackingDummyPtr
		);
	}
	ioChangedTimer->stop();
	ioChangedTimer->start();
}
//-----------------------------------------------------------------------------
void Plugin::setSampleRate(float sampleRate)  {
	this->sampleRate = sampleRate; 
	updateConfiguration();
}
//-----------------------------------------------------------------------------
void Plugin::updateConfiguration() {
    if (blockSize==0) {
        return;
    }
    if (stream) {
        if ((int)stream->getBlockSize() == blockSize) {
            return;
        }
        destroyStream();
    }
	if (name.empty()) {
		name = createName();
	}
    RemoteChannelManager &rm = RemoteChannelManager::instance();
    std::string sId = rm.createUniqueName();
    stream = interprocess::Stream::create(sId,
        blockSize,
        this->getHost()->getNumOutputs(),
        this->getHost()->getNumParameter()
    );
	try {
		globAddRemoteChannelSender( rm.getNumChannels() );
	} catch (const std::exception &ex) {
			::com::osMessageBox ( 
				"Error", std::string(ex.what()), ::com::MSG_ALERT
			);
		return; // don't register sender
	}

    if (channelId.empty()) {
        channelId = rm.addChannel( boost::make_tuple(sId, name) );
    } else {
        rm.addChannel( channelId, boost::make_tuple(sId, name) );
    }
}
//-----------------------------------------------------------------------------
void Plugin::setParameterValue(int index, float value) {
    if (!stream) {
        return;
    }
    if (index >= (int)stream->getNumParameter()) {
        return;
    }
    stream->getParameter()[index] = value;
}
//-----------------------------------------------------------------------------
void Plugin::getParameterValue(int index, float &outValue) {
    if (!stream) {
        return;
    }
    if (index >= (int)stream->getNumParameter()) {
        return;
    }
    outValue = (float)stream->getParameter()[index];
}
//-----------------------------------------------------------------------------
void Plugin::getParameterName (int index, std::string &outStr) const
{
    std::stringstream ss;
    ss<<"sender.param"<<index+1;
    outStr = ss.str();
}
//-----------------------------------------------------------------------------
int Plugin::getChunk(void **data) {
    size_t size = channelId.length();
	if (size==0) {
		return 0;
	}
	std::stringstream ss;
	ss<<channelId<<" "<<name;
	chunk = ss.str();
    *data = (void*) chunk.c_str();
    SAMBAG_LOG_INFO<<"serialize id"<<channelId;
	return chunk.size();
}
//-----------------------------------------------------------------------------
int Plugin::setChunk(void *data, int byteSize) {
	if (byteSize==0) {
		return 0;
	}
    SAMBAG_TRY_TO_LOCK_TIMED(mutex);
    destroyStream();
	std::stringstream ss;
	try {
		ss<<((char*)data);
		ss>>channelId>>name;
	} catch (...) {
		SAMBAG_LOG_ERR<<"deserialing failed";
		return 0;
	}
    SAMBAG_LOG_INFO<<"deserialize id"<<channelId;
    updateConfiguration();
    return byteSize;
}
//-----------------------------------------------------------------------------
int Plugin::getLatency() const {
    return blockSize;
}
}}} // namespace(s)
