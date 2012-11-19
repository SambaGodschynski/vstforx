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
	// get ctrl, map
	frx::processing::IModelController::Ptr ctrl;
	IViewModelMap::Ptr map;
	boost::tie(ctrl, map) = getControllerAndMap(view);
	// get model obj
	FrxComponentPtr c = getComponent();
	frx::processing::IProcessor::Ptr pr = 
		boost::shared_dynamic_cast<frx::processing::IProcessor>(map->getModelObject(c));
	if (!pr)
		return;
	// create browser tree
	typedef FrxColumnBrowser::BrowserImpl Tree;
	Tree::Ptr tree = brws->getBrowserImpl();
	Tree::Node parameter = 
		tree->addNode(tree->getRootNode(), BrowserNode(c->getName() + " parameters"));
	// get frxctrl
	IFrxControl & frxctrl = getFrxControl(view);
	// create browser nodes
	for (size_t i=0; i<pr->getNumParameter(); ++i) {
		frx::processing::IParameter::Ptr p = pr->getParameter(i);
		BrowserNode::AcceptedFunction f = 
			boost::bind(&IFrxControl::addProcesorKnobToView, 
				&frxctrl,
				fgc::FrxCircuidViewWPtr(view), 
				fgc::FrxComponentWPtr(c),
				frx::processing::IParameter::WPtr(p)
			);
		BrowserNode par;
		createParameterNode(par, p->getName(), p);
		par.f = f;
		tree->addNode(
			parameter, 
			par
		);
	}
	tree->updateLists();
}
}}} // namespace(s)
