/*
 * FrxConnectionBrowser.cpp
 *
 *  Created on: Fri Dec  7 14:33:22 2012
 *      Author: Johannes Unger
 */

#include "FrxConnectionBrowser.hpp"

namespace frx { namespace gui { namespace components {
//=============================================================================
//  Class FrxConnectionBrowser
//=============================================================================
//-----------------------------------------------------------------------------
void FrxConnectionBrowser::postConstructor() {
	Super::postConstructor();
}
//-----------------------------------------------------------------------------
FrxConnectionBrowser::Ptr FrxConnectionBrowser::create(sdc::Window::Ptr parent)
{
	Ptr res( new FrxConnectionBrowser(parent) );
	res->self = res;
	res->postConstructor();
	res->initWindow();
	return res;
}
}}} // namespace(s)
