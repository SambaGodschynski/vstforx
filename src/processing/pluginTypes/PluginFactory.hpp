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
public:
    //-------------------------------------------------------------------------
    /**
     * @brief determines plugintype and loads plugin.
     * @return plugin and PluginInfo
     * @param
     * @param the plugin info, only location is needed, the rest will be set
     */
    Product::Ptr loadAndDeterminePlugin(IHostInfo::Ptr hI, ::processing::PluginInfo &inOut);
	//-------------------------------------------------------------------------
	static PluginFactory & instance();
    //-------------------------------------------------------------------------
    static Product::Ptr loadPlugin(IHostInfo::Ptr hI, const std::string &location);
}; // PluginFactory

namespace {
    const bool PluginFactoryRegistered =
        ModelFactory::instance().registerWithDetail (
                "unknown-plugin.Plugin", &PluginFactory::loadPlugin
        );
}

}} // namespace(s)


#endif /* SAMBAG_PLUGINFACTORY_H */
