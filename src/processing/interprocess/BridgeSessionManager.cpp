/*
 * BridgeSessionManager.cpp
 *
 *  Created on: Mon Dec  2 10:48:48 2013
 *      Author: Johannes Unger
 */

#include "BridgeSessionManager.hpp"


namespace frx { namespace processing { namespace interprocess {
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
}}} // namespace(s)
