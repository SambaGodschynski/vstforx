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

namespace processing {
    class Plugin;
    typedef boost::shared_ptr<Plugin> PluginPtr;
} // namespace(s)

namespace frx { namespace processing { namespace interprocess {
//=============================================================================
/** 
  * @class BridgePluginDelegate.
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
    ::processing::PluginPtr plugin;
    //-------------------------------------------------------------------------
    IHostInfo::Ptr hostInfo;
public:
    //-------------------------------------------------------------------------
    /**
     * @brief loads plugin
     * @throws
     */
    static Ptr create(size_t blockSize, float sampleRate, const std::string &location);
    //-------------------------------------------------------------------------
    size_t getBlockSize() const;
    //-------------------------------------------------------------------------
    float getSampleRate() const;
    //-------------------------------------------------------------------------
    size_t getNumInputChannels() const;
    //-------------------------------------------------------------------------
    size_t getNumOutputChannels() const;
    //-------------------------------------------------------------------------
    std::string getLocation() const;
    //-------------------------------------------------------------------------
    ::processing::PluginPtr getPlugin() const {
        return plugin;
    }
    //-------------------------------------------------------------------------
    IHostInfo::Ptr getHostInfo() const {
        return hostInfo;
    }
}; // BridgePluginDelegate
}}} // namespace(s)

#endif /* SAMBAG_BRIDGEPLUGINDELEGATE_H */
