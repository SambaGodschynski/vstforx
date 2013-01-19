/*
 * FrxProcessorBrowser.cpp
 *
 *  Created on: Tue Oct 23 10:09:40 2012
 *      Author: Johannes Unger
 */

#include "FrxProcessorBrowser.hpp"


namespace frx { namespace gui { namespace components {
//=============================================================================
//  Class FrxProcessorBrowser
//=============================================================================
//-----------------------------------------------------------------------------
void FrxProcessorBrowser::postConstructor() {
	Super::postConstructor();
}
//-----------------------------------------------------------------------------
FrxProcessorBrowser::Ptr FrxProcessorBrowser::create(sdc::Window::Ptr parent) {
	Ptr res( new FrxProcessorBrowser(parent) );
	res->self = res;
	res->postConstructor();
	res->initWindow();
	return res;
}
}}} // namespace(s)
