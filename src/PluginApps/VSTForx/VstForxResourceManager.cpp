/*
 * VstForxResourceManager.cpp
 *
 *  Created on: Fri Jan 18 10:28:40 2013
 *      Author: Johannes Unger
 */

#include "VstForxResourceManager.hpp"
#include "initResourceMap.h"

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
	adaptee = &sd::Win32IntResourceManager::instance();
}
//-----------------------------------------------------------------------------
VstForxResourceManager::ImagePtr 
VstForxResourceManager::loadImage(const std::string &path) 
{
	return adaptee->getImage(path);
}
//-----------------------------------------------------------------------------
namespace {
	void _register(const char *p, long id) {
		VstForxResourceManager &m = VstForxResourceManager::instance();
		m.getAdaptee()->registerImage(p, id);
	}
} // namespace(s)
void VstForxResourceManager::initMap(HINSTANCE hI) {
	using namespace createResourcesPy;
	adaptee->setInstance(hI);
	initResourceMap(&_register);
}
} // namespace(s)
