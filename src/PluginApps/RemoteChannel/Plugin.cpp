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

namespace frx { namespace processing { namespace remoteChannel {
using namespace interprocess;

//=============================================================================
// class Plugin 
//=============================================================================
//-----------------------------------------------------------------------------
Plugin::Plugin() : 
blockSize(0),
sampleRate(0.f)
{
}
//-----------------------------------------------------------------------------
void Plugin::open() {
}	
//-----------------------------------------------------------------------------
void Plugin::close() {
    destroyStream();
    chunk.reset();
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
Plugin::~Plugin() {
}
//-----------------------------------------------------------------------------
void Plugin::process(float **in, float **out, int numSamples) {
    SAMBAG_TRY_TO_LOCK_TIMED(mutex);
    if (!stream) {
        return;
    }
    stream->write(in, numSamples);
    // write output
    float *o0 = out[0];
    float *o1 = out[1];
    float *i0 = in[0];
    float *i1 = in[1];
    while(--numSamples >= 0) {
        *(o0++) = *(i0++);
        *(o1++) = *(i1++);
    }
}
//-----------------------------------------------------------------------------
void Plugin::processEvents(sambag::dsp::IMidiEvents *ev) {
}
//-----------------------------------------------------------------------------
void Plugin::setBlockSize(int blockSize) {
	this->blockSize = blockSize;
	updateConfiguration();
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
    RemoteChannelManager &rm = RemoteChannelManager::instance();
    std::string name = rm.createUniqueName();
    stream = interprocess::Stream::create(name,
        blockSize,
        this->getHost()->getNumOutputs(),
        this->getHost()->getNumParameter()
    );
    if (channelId.empty()) {
        channelId = rm.addChannel( boost::make_tuple(name) );
    } else {
        rm.addChannel( channelId, boost::make_tuple(name) );
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
    chunk = Chunk( new char[size] );
    strcpy(chunk.get(), channelId.c_str());
    *data = chunk.get();
    SAMBAG_LOG_INFO<<"serialize id"<<channelId;
    return size;
}
//-----------------------------------------------------------------------------
int Plugin::setChunk(void *data, int byteSize) {
    SAMBAG_TRY_TO_LOCK_TIMED(mutex);
    destroyStream();
    channelId = std::string((char*)data);
    SAMBAG_LOG_INFO<<"deserialize id"<<channelId;
    updateConfiguration();
    return byteSize;
}
//-----------------------------------------------------------------------------
int Plugin::getLatency() const {
    return 0;
}
}}} // namespace(s)
