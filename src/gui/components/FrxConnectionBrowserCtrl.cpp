/*
 * FrxConnectionBrowserCtrl.cpp
 *
 *  Created on: Fri Dec  7 14:45:52 2012
 *      Author: Johannes Unger
 */

#include "FrxConnectionBrowserCtrl.hpp"

namespace frx { namespace gui { namespace components {
//=============================================================================
//  Class FrxConnectionBrowserCtrl
//=============================================================================
//-----------------------------------------------------------------------------
void FrxConnectionBrowserCtrl::setComponent(FrxComponentPtr c)
{
	component = c;
}
//-----------------------------------------------------------------------------
FrxComponentPtr FrxConnectionBrowserCtrl::getComponent() const
{
	return component;
}
//-----------------------------------------------------------------------------
void FrxConnectionBrowserCtrl::initTree(FrxCircuidViewPtr view, 
	FrxColumnBrowserPtr brws)
{
	browser = brws;
	wView = view; 
	// get model obj
	FrxComponentPtr c = getComponent();
	// create browser tree
	typedef FrxColumnBrowser::BrowserImpl Tree;
	Tree::Ptr tree = brws->getBrowserImpl();
	Tree::Node parameter = 
		tree->addNode(tree->getRootNode(), BrowserNode(c->getName() + " parameters"));
	
	addModelObjectParameter(c, parameter);
	tree->updateLists();
}
}}} // namespace(s)
