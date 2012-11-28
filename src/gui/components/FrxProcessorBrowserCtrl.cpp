/*
 * FrxProcessorBrowserCtrl.cpp
 *
 *  Created on: Tue Oct 30 21:04:22 2012
 *      Author: Johannes Unger
 */

#include "FrxProcessorBrowserCtrl.hpp"
#include <gui/FrxControl.hpp>
#include <gui/IViewModelMap.hpp>
#include "FrxProcessorBrowser.hpp"
#include <processing/IProcessor.hpp>
#include "FrxComponent.hpp"
#include <gui/FrxControl.hpp>

namespace frx { namespace gui { namespace components {
//=============================================================================
//  Class FrxProcessorBrowserCtrl
//=============================================================================
//-----------------------------------------------------------------------------
void FrxProcessorBrowserCtrl::setComponent(FrxComponentPtr c)
{
	component = c;
}
//-----------------------------------------------------------------------------
FrxComponentPtr FrxProcessorBrowserCtrl::getComponent() const
{
	return component;
}
//-----------------------------------------------------------------------------
void FrxProcessorBrowserCtrl::initTree(FrxCircuidViewPtr view, 
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
	
	addProcessorParameterNodes(c, parameter);
	
	tree->updateLists();
}
}}} // namespace(s)
