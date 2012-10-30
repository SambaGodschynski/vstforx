/*
 * FrxPluginBrowser.cpp
 *
 *  Created on: Tue Oct 30 14:30:15 2012
 *      Author: Johannes Unger
 */

#include "FrxPluginBrowser.hpp"

namespace frx { namespace gui { namespace components {
//=============================================================================
//  Class FrxPluginBrowser
//=============================================================================
//-----------------------------------------------------------------------------
void FrxPluginBrowser::postConstructor() {
	Super::postConstructor();
}
//-----------------------------------------------------------------------------
FrxPluginBrowser::Ptr FrxPluginBrowser::create(sdc::Window::Ptr parent) {
	Ptr res( new FrxPluginBrowser(parent) );
	res->self = res;
	res->postConstructor();
	res->initWindow();
	return res;
}
}}} // namespace(s)
