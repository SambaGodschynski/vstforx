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

extern PluginFactory::ProductPtr createVST2xPlugin(IHostInfo::Ptr hI,
    const std::string &location);

//=============================================================================
//  Class PluginFactory
//=============================================================================
//-----------------------------------------------------------------------------
PluginFactory & PluginFactory::instance() {
	return PluginFactoryHolder::Instance();
}
//-----------------------------------------------------------------------------
PluginFactory::ProductPtr
PluginFactory::loadBridged(IHostInfo::Ptr hI, const std::string &loc)
{
    return NULL;
}
//-----------------------------------------------------------------------------
PluginFactory::ProductPtr
PluginFactory::loadVST2x(IHostInfo::Ptr hI, const std::string &loc)
{
    return createVST2xPlugin(hI, location);
}
//-----------------------------------------------------------------------------
PluginFactory::ProductPtr
PluginFactory::loadVST3x(IHostInfo::Ptr hI, const std::string &loc)
{
    return NULL;
}
//-----------------------------------------------------------------------------
PluginFactory::ProductPtr
PluginFactory::loadAU(IHostInfo::Ptr hI, const std::string &loc)
{
    return NULL;
}
//-----------------------------------------------------------------------------
PluginFactory::ProductPtr
PluginFactory::load(IHostInfo::Ptr hI, const std::string &location)
{
    ProductPtr res;
    
    try {
        // vst2x
        if ( (res = _loadVST2x(hI, location)) ) {
            return res;
        }
    } catch (const ::processing::PluginArchitectureMissmatch &ex) {
        return _loadBridged(hI, location);
    }
    
    // no success
    return NULL;
}
}} // namespace(s)
