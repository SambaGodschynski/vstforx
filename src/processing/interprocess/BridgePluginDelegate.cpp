/*
 * BridgePluginDelegate.cpp
 *
 *  Created on: Fri Dec 13 13:46:37 2013
 *      Author: Johannes Unger
 */

#include "BridgePluginDelegate.hpp"

namespace frx { namespace processing { namespace interprocess {
//=============================================================================
//  Class BridgePluginDelegate
//=============================================================================
//-----------------------------------------------------------------------------
BridgePluginDelegate::Ptr
BridgePluginDelegate::create(size_t blockSize,
    float sampleRate, const std::string &location)
{
    return BridgePluginDelegate::Ptr();
}
//-----------------------------------------------------------------------------
size_t BridgePluginDelegate::getBlockSize() const {
    return 0;
}
//-----------------------------------------------------------------------------
float BridgePluginDelegate::getSampleRate() const {
    return 0;
}
//-----------------------------------------------------------------------------
size_t BridgePluginDelegate::getNumChannels() const {
    return 0;
}
//-----------------------------------------------------------------------------
const std::string & BridgePluginDelegate::getLocation() const {
    return "";
}

}}} // namespace(s)
