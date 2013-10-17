/*
 * VstForxResourceManager.cpp
 *
 *  Created on: Fri Jan 18 10:28:40 2013
 *      Author: Johannes Unger
 */

#include "VstForxResourceManager.hpp"
#include <boost/filesystem.hpp>


namespace frx { namespace com { 
    extern std::string getResourceLocation(const std::string &path);
    extern std::string getBundleLocation();
}}

namespace frx {
typedef Loki::SingletonHolder<VstForxResourceManager> VstForxResourceManagerHolder;
//=============================================================================
//  Class VstForxResourceManager
//=============================================================================
//-----------------------------------------------------------------------------
VstForxResourceManager & VstForxResourceManager::instance() {
	return VstForxResourceManagerHolder::Instance();
}
//-----------------------------------------------------------------------------
VstForxResourceManager::VstForxResourceManager() {
	Super::setHomeDirectory(".");
}
//-----------------------------------------------------------------------------
VstForxResourceManager::ImagePtr 
VstForxResourceManager::loadImage(const std::string &_path) 
{
	std::string filename = boost::filesystem::path(_path).filename().string();
    filename = com::getResourceLocation(filename);
	return Super::loadImage(filename);
}
//-----------------------------------------------------------------------------
VstForxResourceManager::Url VstForxResourceManager::getPath(const Url &url) const {
    return url;
}
} // namespace(s)
