/*
 * BridgePluginDelegate.cpp
 *
 *  Created on: Fri Dec 13 13:46:37 2013
 *      Author: Johannes Unger
 */

#include "BridgePluginDelegate.hpp"
#include <processing/Plugin.h>
#include <sambag/com/exceptions/IllegalStateException.hpp>

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

   /* BridgePluginDelegate::Ptr res( new BridgePluginDelegate() );
    res->hostInfo = IHostInfo::Ptr( new _HostInfo(blockSize, sampleRate, res.get()) );
    res->plugin = ::processing::PluginFactory::createPlugNode(
        res->hostInfo,
        location
    );
    if (!res->plugin->isAccessable()) {
        SAMBAG_THROW(
            sambag::com::exceptions::IllegalStateException,
            "couldn't access: " + location
        );
    }
    return res;*/
}
//-----------------------------------------------------------------------------
size_t BridgePluginDelegate::getBlockSize() const {
    return getHostInfo()->getBlockSize();
}
//-----------------------------------------------------------------------------
float BridgePluginDelegate::getSampleRate() const {
    return getHostInfo()->getSampleRate();
}
//-----------------------------------------------------------------------------
size_t BridgePluginDelegate::getNumInputChannels() const {
    return plugin->getNumInputChannels();
}
//-----------------------------------------------------------------------------
size_t BridgePluginDelegate::getNumOutputChannels() const {
    return plugin->getNumOutputChannels();
}
//-----------------------------------------------------------------------------
std::string BridgePluginDelegate::getLocation() const {
    return plugin->getLocation();
}

}}} // namespace(s)
