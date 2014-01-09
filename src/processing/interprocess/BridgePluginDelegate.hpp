/*
 * BridgePluginDelegate.hpp
 *
 *  Created on: Fri Dec 13 13:46:37 2013
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_BRIDGEPLUGINDELEGATE_H
#define SAMBAG_BRIDGEPLUGINDELEGATE_H

#include <boost/shared_ptr.hpp>
#include <string>
#include <processing/IHostInfo.h>
#include <processing/pluginTypes/PluginImpl.hpp>
#include <processing/parameter/Parameter.h>

namespace frx { namespace processing { namespace interprocess {
//=============================================================================
/** 
  * @class BridgePluginDelegate.
  * @brief Plugin interface used by bridge.
  */
class BridgePluginDelegate {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<BridgePluginDelegate> Ptr;
protected:
    //-------------------------------------------------------------------------
    BridgePluginDelegate() {}
private:
    //-------------------------------------------------------------------------
    APluginImpl *plugin;
    //-------------------------------------------------------------------------
    IHostInfo::Ptr hostInfo;
    //-------------------------------------------------------------------------
    APluginImpl::Parameters parameters;
public:
    //-------------------------------------------------------------------------
    ~BridgePluginDelegate();
    //-------------------------------------------------------------------------
    /**
     * @brief loads plugin
     * @throws
     */
    static Ptr create(size_t blockSize, float sampleRate, const std::string &location);
    //-------------------------------------------------------------------------
    APluginImpl * getPluginImpl() const {
        return plugin;
    }
    //-------------------------------------------------------------------------
    IHostInfo::Ptr getHostInfo() const {
        return hostInfo;
    }
    //-------------------------------------------------------------------------
    size_t getBlockSize() const {
        return hostInfo->getBlockSize();
    }
    //-------------------------------------------------------------------------
    float getSampleRate() const {
        return hostInfo->getSampleRate();
    }
    //-------------------------------------------------------------------------
    size_t getNumInputChannels() const {
        return plugin->getNumInputChannels();
    }
    //-------------------------------------------------------------------------
    size_t getNumOutputChannels() const {
        return plugin->getNumOutputChannels();
    }
    //-------------------------------------------------------------------------
}; // BridgePluginDelegate
}}} // namespace(s)

#endif /* SAMBAG_BRIDGEPLUGINDELEGATE_H */
