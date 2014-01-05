/*
 * PluginFactory.hpp
 *
 *  Created on: Sat Jan  4 12:15:40 2014
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_PLUGINFACTORY_H
#define SAMBAG_PLUGINFACTORY_H

#include <loki/Singleton.h>
#include <processing/Plugin.h>
#include <processing/IHostInfo.h>
#include <processing/ModelFactory.hpp>
#include <com/one4All.h>

namespace frx { namespace processing {
//=============================================================================
/** 
  * @class PluginFactory.
  * @brief determines plugintype and loads plugin.
  */
class PluginFactory {
//=============================================================================
friend struct Loki::CreateUsingNew<PluginFactory>;
public:
    //-------------------------------------------------------------------------
    typedef ::processing::Plugin Product;
protected:
private:
    //-------------------------------------------------------------------------
    Product::Ptr
    _loadBridged(IHostInfo::Ptr hI, ::com::IdParser id);
    //-------------------------------------------------------------------------
    Product::Ptr
    _loadVST2x(IHostInfo::Ptr hI, ::com::IdParser id);
    //-------------------------------------------------------------------------
    Product::Ptr
    _loadVST3x(IHostInfo::Ptr hI, ::com::IdParser id);
    //-------------------------------------------------------------------------
    Product::Ptr
    _loadAU(IHostInfo::Ptr hI, ::com::IdParser id);
    //-------------------------------------------------------------------------
    Product::Ptr
    _load(IHostInfo::Ptr hI, const std::string &location);
public:
	//-------------------------------------------------------------------------
	static PluginFactory & instance();
    //-------------------------------------------------------------------------
    /**
     * @brief determines plugintype and loads plugin.
     * @return plugin
     * @param
     * @param the plugin location
     */
    static Product::Ptr load(IHostInfo::Ptr hI, const std::string &location);
}; // PluginFactory

namespace {
    const bool PluginFactoryRegistered =
        ModelFactory::instance().registerWithDetail (
                "unknown-plugin.Plugin", &PluginFactory::load
        );
}

}} // namespace(s)


#endif /* SAMBAG_PLUGINFACTORY_H */
