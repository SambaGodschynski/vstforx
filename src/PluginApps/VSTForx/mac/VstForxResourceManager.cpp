/*
 * VstForxResourceManager.cpp
 *
 *  Created on: Fri Jan 18 10:28:40 2013
 *      Author: Johannes Unger
 */

#include "VstForxResourceManager.hpp"

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
}
//-----------------------------------------------------------------------------
VstForxResourceManager::ImagePtr 
VstForxResourceManager::loadImage(const std::string &path) 
{
	return ImagePtr();
}
} // namespace(s)
