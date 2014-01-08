/*
 * PluginFactory.cpp
 *
 *  Created on: Sat Jan  4 12:15:40 2014
 *      Author: Johannes Unger
 */

#include "PluginFactory.hpp"
#include <processing/ModelFactory.hpp>
#include "BridgedPlugin.hpp"

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
    return NULL;
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
PluginFactory::load(Type type, IHostInfo::Ptr hI,
    Parameters*par, const std::string &loc)
{
    using ::processing::PluginInfo;
    ProductPtr res;
    
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
