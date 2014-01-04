/*
 * PluginFactory.cpp
 *
 *  Created on: Sat Jan  4 12:15:40 2014
 *      Author: Johannes Unger
 */

#include "PluginFactory.hpp"
#include <processing/ModelFactory.hpp>
#include <com/one4All.h>

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
PluginFactory::_load(IHostInfo::Ptr hI, const std::string &location)
{
    ModelFactory &fac = ModelFactory::instance();
    Product::Ptr res;
    ::com::IdParser id;
    id.namespace_("processing").name("Plugin").details(location);
    
    // vst2x
    id.type("vst2x");
    res = fac.create<Product>(id.toString(), hI);
    if (res) {
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
