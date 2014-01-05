/*
 * PluginFactory.cpp
 *
 *  Created on: Sat Jan  4 12:15:40 2014
 *      Author: Johannes Unger
 */

#include "PluginFactory.hpp"
#include <processing/ModelFactory.hpp>


namespace frx { namespace processing {
typedef Loki::SingletonHolder<PluginFactory> PluginFactoryHolder;
//=============================================================================
//  Class PluginFactory
//=============================================================================
//-----------------------------------------------------------------------------
PluginFactory & PluginFactory::instance() {
	return PluginFactoryHolder::Instance();
}
//-----------------------------------------------------------------------------
PluginFactory::Product::Ptr
PluginFactory::_loadBridged(IHostInfo::Ptr hI, ::com::IdParser id)
{
    return Product::Ptr();
}
//-----------------------------------------------------------------------------
PluginFactory::Product::Ptr
PluginFactory::_loadVST2x(IHostInfo::Ptr hI, ::com::IdParser id)
{
    ModelFactory &fac = ModelFactory::instance();
    id.type("vst2x");
    try {
        return fac.create<Product>(id.toString(), hI);
    } catch (const ::processing::PluginArchitectureMissmatch &ex) {
        return Product::Ptr();
    }
}
//-----------------------------------------------------------------------------
PluginFactory::Product::Ptr
PluginFactory::_loadVST3x(IHostInfo::Ptr hI, ::com::IdParser id)
{
    return Product::Ptr();
}
//-----------------------------------------------------------------------------
PluginFactory::Product::Ptr
PluginFactory::_loadAU(IHostInfo::Ptr hI, ::com::IdParser id)
{
    return Product::Ptr();
}
//-----------------------------------------------------------------------------
PluginFactory::Product::Ptr
PluginFactory::_load(IHostInfo::Ptr hI, const std::string &location)
{
    Product::Ptr res;
    ::com::IdParser id;
    id.namespace_("processing").name("Plugin").details(location);
    
    // vst2x
    if ( res = _loadVST2x(hI, id) ) {
        return res;
    }
    
    // no success
    return Product::Ptr();
}
//-----------------------------------------------------------------------------
PluginFactory::Product::Ptr
PluginFactory::load(IHostInfo::Ptr hI, const std::string &location)
{
    return instance()._load(hI, location);
}
}} // namespace(s)
