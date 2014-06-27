/*
 * PluginFactory.hpp
 *
 *  Created on: Sat Jan  4 12:15:40 2014
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_PLUGINFACTORY_H
#define SAMBAG_PLUGINFACTORY_H

#include <loki/Singleton.h>
#include "PluginImpl.hpp"
#include <processing/PlugInfo.h>
#include <processing/IHostInfo.h>

namespace frx { namespace processing {
//=============================================================================
/** 
  * @class PluginFactory.
  * @brief determines plugintype and loads plugin impl (@see APluginImpl).
  */
class PluginFactory {
//=============================================================================
friend struct Loki::CreateUsingNew<PluginFactory>;
public:
    //-------------------------------------------------------------------------
    typedef APluginImpl Product;
    //-------------------------------------------------------------------------
    typedef APluginImpl::Parameters Parameters;
    //-------------------------------------------------------------------------
    typedef Product::Ptr ProductPtr;
    //-------------------------------------------------------------------------
	typedef ::processing::PluginInfo PluginInfo;
    typedef PluginInfo::PluginType Type;

protected:
    //-------------------------------------------------------------------------
    std::string complete(const std::string &path);
    //-------------------------------------------------------------------------
    /**
     * @return bridged plugin impl
     */
    ProductPtr loadBridged(IHostInfo::Ptr, Parameters*, const std::string &);
    //-------------------------------------------------------------------------
    /**
     * @return vst2x plugin impl
     */
    ProductPtr loadVST2x(IHostInfo::Ptr, Parameters*, const std::string &);
    //-------------------------------------------------------------------------
    /**
     * @return vst3x plugin impl
     */
    ProductPtr loadVST3x(IHostInfo::Ptr, Parameters*, const std::string &);
    //-------------------------------------------------------------------------
    /**
     * @return au plugin impl
     */
    ProductPtr loadAU(IHostInfo::Ptr, Parameters*, const std::string &);
    //-------------------------------------------------------------------------
    /**
     * @return au plugin impl
     */
    ProductPtr loadLua(IHostInfo::Ptr, Parameters*, const std::string &);
    //-------------------------------------------------------------------------
    Type detectType(const std::string &);
public:
	//-------------------------------------------------------------------------
	static PluginFactory & instance();
    //-------------------------------------------------------------------------
    /**
     * @return appropriate plugin impl
     */
    ProductPtr load(IHostInfo::Ptr,
        Parameters*,
        const std::string &,
        Type type = PluginInfo::UNKNOWN);


}; // PluginFactory
}} // namespace(s)


#endif /* SAMBAG_PLUGINFACTORY_H */
