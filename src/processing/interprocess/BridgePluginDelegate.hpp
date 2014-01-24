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
#include <sambag/disco/components/Timer.hpp>
#include <gui/HandyNamespaces.hpp>

namespace frx { namespace gui { namespace components { namespace interprocess {
    class WindowSessionHost;
    typedef boost::shared_ptr<WindowSessionHost> WindowSessionHostPtr;
}}}}
namespace frx { namespace processing { namespace interprocess {
class PluginSessionHost;
typedef boost::shared_ptr<PluginSessionHost> PluginSessionHostPtr;
typedef boost::weak_ptr<PluginSessionHost> PluginSessionHostWPtr;
namespace fgci = frx::gui::components::interprocess;
//=============================================================================
/** 
  * @class BridgePluginDelegate.
  * @brief Plugin interface used by bridge.
  */
class BridgePluginDelegate : public sce::EventSender<sce::PropertyChanged> {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<BridgePluginDelegate> Ptr;
protected:
    //-------------------------------------------------------------------------
    BridgePluginDelegate();
    //-------------------------------------------------------------------------
    void onIdleTimer(void *src, const sdc::TimerEvent &ev);
    //-------------------------------------------------------------------------
    void onPluginPropertyChanged(void*, const sce::PropertyChanged &ev);
private:
    //-------------------------------------------------------------------------
    sdc::Timer::Ptr idleTimer;
    //-------------------------------------------------------------------------
    APluginImpl *plugin;
    //-------------------------------------------------------------------------
    IHostInfo::Ptr hostInfo;
    //-------------------------------------------------------------------------
    APluginImpl::Parameters parameters;
    //-------------------------------------------------------------------------
    ::processing::PluginInfo pluginInfo;
    //-------------------------------------------------------------------------
    fgci::WindowSessionHostPtr windowSession;
    //-------------------------------------------------------------------------
    PluginSessionHostWPtr pluginSession;
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
    PluginSessionHostPtr getPluginSession() const {
        return pluginSession.lock();
    }
    //-------------------------------------------------------------------------
    void setPluginSession(PluginSessionHostPtr session);
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
    const ::processing::PluginInfo & getPluginInfo();
    //-------------------------------------------------------------------------
    const APluginImpl::Parameters & getParameters() const {
        return parameters;
    }
    //-------------------------------------------------------------------------
    APluginImpl::Parameters & getParameters() {
        return parameters;
    }
    //-------------------------------------------------------------------------
    fgci::WindowSessionHostPtr getWindowSession();
    //-------------------------------------------------------------------------
    void openEditor();
    //-------------------------------------------------------------------------
    void closeEditor();
}; // BridgePluginDelegate
}}} // namespace(s)

#endif /* SAMBAG_BRIDGEPLUGINDELEGATE_H */
