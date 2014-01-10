/*
 * PluginFactory.cpp
 *
 *  Created on: Sat Jan  4 12:15:40 2014
 *      Author: Johannes Unger
 */

#include "PluginFactory.hpp"
#include <processing/ModelFactory.hpp>
#include <processing/interprocess/BridgeSessionManager.hpp>
#include <sambag/com/exceptions/IllegalStateException.hpp>

namespace frx { namespace processing {
typedef Loki::SingletonHolder<PluginFactory> PluginFactoryHolder;

/**
 * creation of products could be approached using a loki::Factory pattern as the
 * ModelFactory is using.
 * BUT, since we need a reasonable number of creators we use
 * this "cheap" approach.
 */
extern PluginFactory::ProductPtr createVST2xPluginImpl(IHostInfo::Ptr,
    PluginFactory::Parameters*, const std::string&);
    
extern PluginFactory::ProductPtr createBridgedPluginImpl(IHostInfo::Ptr,
    PluginFactory::Parameters*, const std::string&);

//=============================================================================
//  Class PluginFactory
//=============================================================================
//-----------------------------------------------------------------------------
PluginFactory & PluginFactory::instance() {
	return PluginFactoryHolder::Instance();
}
//-----------------------------------------------------------------------------
PluginFactory::ProductPtr
PluginFactory::loadBridged(IHostInfo::Ptr hI, Parameters*par, const std::string &loc)
{
    SAMBAG_LOG_INFO<<"loading bridge...";
    if (interprocess::BridgeSessionManager::instance().isBridge()) {
        using sambag::com::exceptions::IllegalStateException;
        SAMBAG_THROW(IllegalStateException,"bridge: unsupportet architecture.");
    }
    return createBridgedPluginImpl(hI, par, loc);
}
//-----------------------------------------------------------------------------
PluginFactory::ProductPtr
PluginFactory::loadVST2x(IHostInfo::Ptr hI, Parameters*par, const std::string &loc)
{
    return createVST2xPluginImpl(hI, par, loc);
}
//-----------------------------------------------------------------------------
PluginFactory::ProductPtr
PluginFactory::loadVST3x(IHostInfo::Ptr hI, Parameters*par, const std::string &loc)
{
    return NULL;
}
//-----------------------------------------------------------------------------
PluginFactory::ProductPtr
PluginFactory::loadAU(IHostInfo::Ptr hI, Parameters*par, const std::string &loc)
{
    return NULL;
}
//-----------------------------------------------------------------------------
PluginFactory::Type PluginFactory::detectType(const std::string &) {
    using ::processing::PluginInfo;
    return PluginInfo::VST2X;
}
//-----------------------------------------------------------------------------
PluginFactory::ProductPtr
PluginFactory::load(IHostInfo::Ptr hI,
    Parameters*par, const std::string &loc, Type type)
{
    using ::processing::PluginInfo;
    
    if (type == PluginInfo::UNKNOWN) {
        type = detectType(loc);
    }
    
    try {
        switch (type) {
        case PluginInfo::VST2X : return loadVST2x(hI, par, loc);
        default : return NULL;
        
        }
    } catch (const PluginArchitectureMissmatch &ex) {
        return loadBridged(hI, par, loc);
    }
    
    // no success
    return NULL;
}
}} // namespace(s)
