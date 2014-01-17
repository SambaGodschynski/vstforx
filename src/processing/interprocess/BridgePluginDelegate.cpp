/*
 * BridgePluginDelegate.cpp
 *
 *  Created on: Fri Dec 13 13:46:37 2013
 *      Author: Johannes Unger
 */

#include "BridgePluginDelegate.hpp"
#include <processing/Plugin.h>
#include <sambag/com/exceptions/IllegalStateException.hpp>
#include <processing/pluginTypes/PluginFactory.hpp>
#include <gui/components/interprocess/WindowSession.hpp>
#include <gui/components/FrxPluginEditor.hpp>

namespace frx { namespace processing { namespace interprocess {
namespace {
    struct _HostInfo : public IHostInfo {
        _HostInfo(float sr, int bs, BridgePluginDelegate *master) :
            sampleRate(sr), blockSize(bs), master(master)
        {
        }
        virtual float getSampleRate () const
        {
            return sampleRate;
        }
        virtual int getBlockSize () const
        {
            return blockSize;
        }
        virtual TimeInfo * 	getHostTimeInfo (int filter)
        {
            // TODO:
            return NULL;
        }
        virtual void * getEffectPtr ()
        {
            // TODO:
            return NULL;
        }
        virtual void * getMasterCallback ()
        {
            // TODO:
            return NULL;
        }
        virtual bool ioChanged () {
            // TODO:
            return true;
        }
        virtual HostIOChangedConnection
        addHostChangedListener (const HostIOChangedFunction &f)
        {
            // TODO:
            return HostIOChangedConnection();
        }
        virtual HostIOChangedConnection
        addTrackedHostChangedListener (const HostIOChangedFunction &f, AnyWPtr wptr)
        {
            // TODO:
            return HostIOChangedConnection();
        }
        float sampleRate;
        int blockSize;
        BridgePluginDelegate *master;
    };
} // namespace

//=============================================================================
//  Class BridgePluginDelegate
//=============================================================================
//-----------------------------------------------------------------------------
BridgePluginDelegate::Ptr
BridgePluginDelegate::create(size_t blockSize,
    float sampleRate, const std::string &location)
{
    BridgePluginDelegate::Ptr res( new BridgePluginDelegate() );
    res->hostInfo = IHostInfo::Ptr( new _HostInfo(sampleRate, blockSize, res.get()) );
    res->plugin = PluginFactory::instance().load(res->hostInfo, &(res->parameters), location);
    if (!res->plugin->isAccessable()) {
        SAMBAG_THROW(
            sambag::com::exceptions::IllegalStateException,
            "couldn't access: " + location
        );
    }
    
    res->plugin->sce::EventSender<sce::PropertyChanged>::addEventListener(
        boost::bind(&BridgePluginDelegate::onPluginPropertyChanged, res.get(), _1, _2)
    );

    return res;
}
//-----------------------------------------------------------------------------
BridgePluginDelegate::BridgePluginDelegate() {
}
//-----------------------------------------------------------------------------
BridgePluginDelegate::~BridgePluginDelegate() {
    delete plugin;
}
//-----------------------------------------------------------------------------
const ::processing::PluginInfo & BridgePluginDelegate::getPluginInfo() {
    plugin->updatePluginInfo(pluginInfo);
    return pluginInfo;
}
//-----------------------------------------------------------------------------
fgci::WindowSessionHostPtr BridgePluginDelegate::getWindowSession() {
    if (windowSession) {
        return windowSession;
    }
    windowSession = fgci::WindowSessionHost::create();
    return windowSession;
}
//-----------------------------------------------------------------------------
void BridgePluginDelegate::openEditor() {
    SAMBAG_LOG_TRACE<<"OPEN";
    sdc::Window::Ptr win = getWindowSession()->getWindow();
    void *wndPtr = win->getWindowImpl()->getSystemHandle();
    plugin->openEditor(wndPtr);

    idleTimer = sdc::Timer::create(10);
    idleTimer->setNumRepetitions(-1);
    idleTimer->sce::EventSender<sdc::TimerEvent>::addTrackedEventListener(
        boost::bind(&BridgePluginDelegate::onIdleTimer, this, _1, _2),
            win);
    idleTimer->start();

}
//-----------------------------------------------------------------------------
void BridgePluginDelegate::closeEditor() {
    void *wndPtr = getWindowSession()->getWindow()->getWindowImpl()->getSystemHandle();
    plugin->closeEditor(wndPtr);
    
    if (idleTimer) {
        idleTimer->stop();
    }

}
//-----------------------------------------------------------------------------
void BridgePluginDelegate::onIdleTimer(void *src, const sdc::TimerEvent &ev) {
	plugin->onEditorIdle();
}
//-----------------------------------------------------------------------------
void BridgePluginDelegate::onPluginPropertyChanged(void*,
        const sambag::com::events::PropertyChanged &ev)
{
    SAMBAG_LOG_TRACE<<"BridgePluginDelegate";
    sce::EventSender<sce::PropertyChanged>::notifyListeners(this, ev);
}
}}} // namespace(s)
