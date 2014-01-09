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
    SAMBAG_THROW(
        sambag::com::exceptions::IllegalStateException,
        "missing implemenation"
    );

    BridgePluginDelegate::Ptr res( new BridgePluginDelegate() );
    res->hostInfo = IHostInfo::Ptr( new _HostInfo(blockSize, sampleRate, res.get()) );
    res->plugin = PluginFactory::instance().load(res->hostInfo, &(res->parameters), location);
    if (!res->plugin->isAccessable()) {
        SAMBAG_THROW(
            sambag::com::exceptions::IllegalStateException,
            "couldn't access: " + location
        );
    }
    return res;
}
//-----------------------------------------------------------------------------
BridgePluginDelegate::~BridgePluginDelegate() {
    plugin->closePlugin();
    delete plugin;
}

}}} // namespace(s)
