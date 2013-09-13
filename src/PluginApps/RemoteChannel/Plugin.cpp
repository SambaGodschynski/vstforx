/*
 * Plugin.cpp
 *
 *  Created on: Mon Oct  8 12:58:07 2012
 *      Author: Johannes Unger
 */

#include "Plugin.hpp"
#include <stdlib.h>
#include <sstream>

namespace frx { namespace processing {
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
}
//-----------------------------------------------------------------------------
Plugin::~Plugin() {

}
//-----------------------------------------------------------------------------
void Plugin::process(float **in, float **out, int numSamples) {
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
}} // namespace(s)
