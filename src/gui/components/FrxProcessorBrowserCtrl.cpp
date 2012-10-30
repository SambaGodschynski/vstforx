/*
 * FrxProcessorBrowserCtrl.cpp
 *
 *  Created on: Tue Oct 30 21:04:22 2012
 *      Author: Johannes Unger
 */

#include "FrxProcessorBrowserCtrl.hpp"
#include <gui/FrxControl.hpp>
#include <gui/IViewModelMap.hpp>
namespace frx { namespace gui { namespace components {/*
//-----------------------------------------------------------------------------
void onBrowserOk(void *src,
	const sdc::events::ActionEvent &ev,
	FrxColumnBrowser::WPtr _browser)
{
	// lock weak ptr
	FrxColumnBrowser::Ptr browser = _browser.lock();
	SAMBAG_ASSERT(browser);
	// get selection path
	typedef FrxColumnBrowser::BrowserImpl Tree;
	Tree::Ptr tree = browser->getBrowserImpl();
	const Tree::Path &path = tree->getSelectionPath();
	if (path.empty())
		return;
	const BrowserNode &bNode = tree->getNodeData(path.back());
	bNode.accept();
}*/
//=============================================================================
//  Class FrxProcessorBrowserCtrl
//=============================================================================
//-----------------------------------------------------------------------------
void FrxProcessorBrowserCtrl::initTree(FrxCircuidViewPtr view, 
	FrxColumnBrowserPtr brws)
{
	// get ctrl, map
	frx::processing::IModelController::Ptr ctrl;
	IViewModelMap::Ptr map;
	boost::tie(ctrl, map) = getControllerAndMap(view);
	/*// create browser
	FrxProcessorBrowser::Ptr browser = boost::shared_dynamic_cast<FrxProcessorBrowser>( 
		openProcessorBrowser(view, c) 
	);
	browser->setTitle(c->getName() + " details");
	// add btnOk listener
	browser->getBtnAdd()->EventSender<sdc::events::ActionEvent>::addEventListener(
		boost::bind(&onBrowserOk, _1, _2, FrxColumnBrowser::WPtr(browser))
	);
	// get model obj
	frx::processing::IProcessor::Ptr pr = 
		boost::shared_dynamic_cast<frx::processing::IProcessor>(map->getModelObject(c));
	if (!pr)
		return;
	// create browser tree
	typedef FrxColumnBrowser::BrowserImpl Tree;
	Tree::Ptr tree = browser->getBrowserImpl();
	Tree::Node parameter = 
		tree->addNode(tree->getRootNode(), BrowserNode(c->getName() + " parameters"));
	// create browser nodes
	for (size_t i=0; i<pr->getNumParameter(); ++i) {
		frx::processing::IParameter::Ptr p = pr->getParameter(i);
		BrowserNode::AcceptedFunction f  /*= 
			boost::bind(&FrxControl::addProcesorKnobToView, 
				this,
				fgc::FrxCircuidViewWPtr(view), 
				fgc::FrxComponentWPtr(c),
				frx::processing::IParameter::WPtr(p)
			)*//*;
		tree->addNode(
			parameter, 
			BrowserNode(p->getName(), f)
		);
	}
	tree->updateLists();*/
}
}}} // namespace(s)
