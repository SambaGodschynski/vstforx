/*
 * BridgeSessionManager.cpp
 *
 *  Created on: Mon Dec  2 10:48:48 2013
 *      Author: Johannes Unger
 */

#include "BridgeSessionManager.hpp"
#include <sambag/com/Config.h>

namespace frx { namespace processing { namespace interprocess {
#ifdef SAMBAG_64
    const std::string FRX_BRIDGE_ARCH_STR("64");
#else
    const std::string FRX_BRIDGE_ARCH_STR("32");
#endif
typedef Loki::SingletonHolder<BridgeSessionManager> BridgeSessionManagerHolder;

//=============================================================================
//  Class BridgeSessionManager
//=============================================================================
//-----------------------------------------------------------------------------
BridgeSessionManager::BridgeSessionManager() : path(".") {
    initManager("VSTForx.Bridge.Sessionmanager");
}
//-----------------------------------------------------------------------------
BridgeSessionManager & BridgeSessionManager::instance() {
	return BridgeSessionManagerHolder::Instance();
}
//-----------------------------------------------------------------------------
std::string BridgeSessionManager::getHostSessionId() const {
    return path+"./"+"VSTForx.Bridge.Mainsession" + FRX_BRIDGE_ARCH_STR;
}
}}} // namespace(s)
