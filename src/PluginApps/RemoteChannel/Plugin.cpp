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
    using namespace interprocess;
    RemoteChannelManager &rm = RemoteChannelManager::instance();
    //rm.removeChannel(channelId);
}
//-----------------------------------------------------------------------------
Plugin::~Plugin() {
}
//-----------------------------------------------------------------------------
void Plugin::process(float **in, float **out, int numSamples) {
    if (!stream) {
        return;
    }
    stream->write(in);
    for (int i=0; i<numSamples; ++i) {
        out[0][i] = in[0][i];
        out[1][i] = in[1][i];
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
        stream->resize(blockSize, this->getHost()->getNumOutputs());
        return;
    }
    RemoteChannelManager &rm = RemoteChannelManager::instance();
    /*std::string name = rm.createUniqueName();
    stream = interprocess::Stream::create(name,
        blockSize,
        this->getHost()->getNumOutputs()
    );
    channelId = rm.addChannel( boost::make_tuple(name) );*/
}
//-----------------------------------------------------------------------------
void Plugin::setParameterValue(int index, float value) {
}
//-----------------------------------------------------------------------------
void Plugin::getParameterValue(int index, float &outValue) {
}
//-----------------------------------------------------------------------------
void Plugin::getParameterName (int index, std::string &outStr) const
{
}
//-----------------------------------------------------------------------------
void Plugin::hostParameterChanged(void *src, float value, int index) {
}
//-----------------------------------------------------------------------------
int Plugin::getChunk(void **data) {
}
//-----------------------------------------------------------------------------
int Plugin::setChunk(void *data, int byteSize) {
}
//-----------------------------------------------------------------------------
int Plugin::getLatency() const {
    return 0;
}
}}} // namespace(s)
