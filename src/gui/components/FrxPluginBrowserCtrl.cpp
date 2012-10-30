/*
 * FrxPluginBrowserCtrl.cpp
 *
 *  Created on: Tue Oct 30 15:04:41 2012
 *      Author: Johannes Unger
 */

#include "FrxPluginBrowserCtrl.hpp"
#include <com/PluginCollection.h>

namespace frx { namespace gui { namespace components {
//=============================================================================
//  Class FrxPluginBrowserCtrl
//=============================================================================
//-----------------------------------------------------------------------------
void FrxPluginBrowserCtrl::setHostInfo(IHostInfo::Ptr hostInfo) {
	this->hostInfo = hostInfo;
}
//-----------------------------------------------------------------------------
void FrxPluginBrowserCtrl::initTree(FrxCircuidViewPtr view, 
	FrxColumnBrowserPtr brws)
{
}
}}} // namespace(s)
