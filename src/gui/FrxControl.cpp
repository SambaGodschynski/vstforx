/*
 * FrxControl.cpp
 *
 *  Created on: Thu Sep 27 14:49:27 2012
 *      Author: Johannes Unger
 */

#include "FrxControl.hpp"
#include "components/FrxCircuidView.hpp"
#include "components/FrxConcreteProcessor.hpp"
#include "components/FrxConcreteParameter.hpp"
#include "components/FrxHover.hpp"
#include "components/FrxConcreteIO.hpp"
#include "components/FrxConcreteParameter.hpp"
#include "components/FrxConcreteConnections.hpp"
#include "components/Forward.hpp"
#include <boost/foreach.hpp>
#include <sambag/com/Common.hpp>
#include <loki/MultiMethods.h>
#include <sambag/disco/components/PopupMenu.hpp>
#include <sambag/disco/components/MenuSelectionManager.hpp>
#include <sambag/com/Exception.hpp>
#include <sambag/com/exceptions/IllegalStateException.hpp>
#include <processing/IModelController.hpp>
#include "IViewModelMap.hpp"
#include <exception>
#include "__ModelExecutors.hpp"
#include <boost/archive/text_oarchive.hpp> 
#include <boost/archive/text_iarchive.hpp>
#include "components/FrxSerializationRegister.hpp"
#include <list>
#include <string>
#include <gui/components/FrxProcessorBrowser.hpp>
#include <processing/IParameter.hpp>
#include <processing/IProcessor.hpp>
#include <sambag/disco/components/ui/ALookAndFeel.hpp>
#include <sambag/disco/components/DefaultBoundedRangeModel.hpp>
namespace frx { namespace gui {
using namespace components;
namespace {
////////////////////////////////////////////////////////////////////////////////
//typedef std::string BrowserNode;
typedef FrxColumnBrowser<BrowserNode> ColumnBrowser;
typedef FrxProcessorBrowser<BrowserNode> ProcessorBrowser;
////////////////////////////////////////////////////////////////////////////////
boost::tuple<
	frx::processing::IModelController::Ptr,
	IViewModelMap::Ptr
>
getControllerAndMap(FrxCircuidViewPtr circ)
{
	// create model obj.
	frx::processing::IModelController::Ptr ctrl = 
		frx::processing::getModelController(circ);
	if (!ctrl) {
		SAMBAG_THROW(sambag::com::exceptions::IllegalStateException, 
			"tried to add processor with IModelController == NULL");
	}
	IViewModelMap::Ptr map = getViewModelMap(circ);
	if (!map) {
		SAMBAG_THROW(sambag::com::exceptions::IllegalStateException, 
			"tried to add processor with IViewModelMap == NULL");
	}
	return boost::make_tuple(ctrl, map);
}
////////////////////////////////////////////////////////////////////////////////
//  Private executors
//-----------------------------------------------------------------------------
bool onModelObjectRemoved(fp::ModelObject::WPtr _mObj, FrxCircuidViewWPtr _view);
//-----------------------------------------------------------------------------
template <class ConcreteProcessor>
typename ConcreteProcessor::Ptr 
createProcessor(FrxCircuidViewPtr c) 
{
	typename ConcreteProcessor::Ptr res = ConcreteProcessor::create();
	return res;
}
//-----------------------------------------------------------------------------
void registerProcessor(IViewModelMap::Ptr map,
					   FrxProcessorNode::Ptr v,
					   FrxCircuidViewPtr view,
					   frx::processing::IProcessor::Ptr m) 
{
	map->registerObjects(v, m);
	const FrxProcessorNode::IOContainer &ins = v->getInputs();
	int num = std::min(ins.size(), m->getNumInputs());
	for (int i=0; i<num; ++i) {
		frx::processing::INode::Ptr node = m->getInput(i);
		map->registerObjects(ins[i], node);
		node->addRemoveRequestExecuter(
			boost::bind(&onModelObjectRemoved, _1, FrxCircuidViewWPtr(view))
		);
	}
	const FrxProcessorNode::IOContainer &outs = v->getOutputs();
	num = std::min(outs.size(), m->getNumOutputs());
	for (int i=0; i<num; ++i) {
		frx::processing::INode::Ptr node = m->getOutput(i);
		map->registerObjects(outs[i], node);
		node->addRemoveRequestExecuter(
			boost::bind(&onModelObjectRemoved, _1, FrxCircuidViewWPtr(view))
		);
	}

}
//-----------------------------------------------------------------------------
template <class ConcreteProcessor>
void addProcessorToView(FrxCircuidViewPtr circ, 
	FrxComponentPtr alwaysNull, int numInputs, int numOutputs) 
{
	typename ConcreteProcessor::Ptr viewObj =
		createProcessor<ConcreteProcessor>(circ);
	if (!viewObj) {
		return;
	}
	if (!circ) {
		SAMBAG_THROW(sambag::com::exceptions::IllegalStateException, 
			"tried to add processor with FrxCircuidViewPtr == NULL");
	}
	// create model obj.
	frx::processing::IModelController::Ptr ctrl;
	IViewModelMap::Ptr map;
	boost::tie(ctrl, map) = getControllerAndMap(circ);

	frx::processing::IProcessor::Ptr mObj = 
		createProcessorOnModel<ConcreteProcessor>(ctrl, numInputs, numOutputs);
	if (!mObj) {
		SAMBAG_THROW(sambag::com::exceptions::IllegalStateException, 
			"could'nt create processor object.");
	}
	// create view obj.
	circ->add(viewObj, FrxCircuidView::Z_ProcessorNodes);
	viewObj->setLocation(0, 0);
	viewObj->configIO(numInputs, numOutputs);
	// register
	registerProcessor(map, viewObj, circ, mObj);
	// hover
	FrxHover::Ptr sel = FrxHover::create();
	circ->add(sel);
	sel->addElement(viewObj);
	sel->addElements(viewObj->getInputs());
	sel->addElements(viewObj->getOutputs());
}
//-----------------------------------------------------------------------------
void knobChanged(void *src, 
	const sdc::DefaultBoundedRangeModelChanged &ev,
	frx::processing::IParameter::WPtr _par) 
{
	frx::processing::IParameter::Ptr par = _par.lock();
	if (!par)
		return;
	par->setValue(ev.getSrc().getValue());
}
//-----------------------------------------------------------------------------
void parameterChanged(void *src, float value, FrxParameter::WPtr _knob) {
	FrxParameter::Ptr knob = _knob.lock();
	if (!knob)
		return;
	knob->getRangeModel()->setValue(value);
}
//-----------------------------------------------------------------------------
bool onModelObjectRemoved(fp::ModelObject::WPtr _mObj, FrxCircuidViewWPtr _view)
{
	fp::ModelObject::Ptr mObj = _mObj.lock();
	FrxCircuidViewPtr view(_view);
	if (!view)
		return true; // nothing to do anymore
	// get model obj.
	frx::processing::IModelController::Ptr ctrl;
	IViewModelMap::Ptr map;
	boost::tie(ctrl, map) = getControllerAndMap(view);
	ViewObject::Ptr obj = map->getViewObject(mObj);
	if (!obj) {
		SAMBAG_THROW(sambag::com::exceptions::IllegalStateException, 
			"access to view object failed while removing model obj."
		);
	}
	FrxComponent::Ptr c = boost::shared_dynamic_cast<FrxComponent>(obj);
	map->remove(c, mObj);
	view->remove(c);
	view->AContainer::redraw();
	return true;
}
//-----------------------------------------------------------------------------
void addProcesorKnobToView(FrxCircuidViewWPtr _view, FrxComponentWPtr _c,
	frx::processing::IParameter::WPtr _par) 
{
	FrxCircuidViewPtr view = _view.lock();
	FrxComponentPtr c = _c.lock();
	frx::processing::IParameter::Ptr par = _par.lock();

	frx::processing::IModelController::Ptr ctrl;
	IViewModelMap::Ptr map;
	boost::tie(ctrl, map) = getControllerAndMap(view);
	// create knob
	FrxStdKnob::Ptr knob = FrxStdKnob::create();
	if (!knob) {
		return;
	}
	knob->getRangeModel()->setValue(par->getValue());
	// knob listener
	knob->getRangeModel()->EventSender<sdc::DefaultBoundedRangeModelChanged>::
		addTrackedEventListener ( boost::bind(&knobChanged, _1, _2, _par), _par );
	// parameter listener
	par->getEventSender().addTrackedValueChangedListener(
		boost::bind(&parameterChanged, _1, _2, FrxParameter::WPtr(knob)), knob
	);
	// register knob
	map->registerObjects(knob, par);
	par->addRemoveRequestExecuter(
		boost::bind(&onModelObjectRemoved, _1, FrxCircuidViewWPtr(view))
	);
	// create connection
	ProcessorParameterCn::Ptr cn = ProcessorParameterCn::create();
	cn->setSrcComponent(c);
	cn->setDstComponent(knob);
	view->add(cn, FrxCircuidView::Z_Wires);
	// place knob
	knob->setLocation(0, 0);
	view->add(knob, FrxCircuidView::Z_Knobs);
	// add hover
	FrxHover::Ptr sel = FrxHover::create();
	view->add(sel);
	sel->addElement(knob);
}
//-----------------------------------------------------------------------------
void addFreeKnobToView(FrxCircuidViewPtr circ, FrxComponentPtr alwaysNull) {
	FrxStdKnob::Ptr res = FrxStdKnob::create();
	if (!res) {
		return;
	}
	if (!circ) {
		SAMBAG_WARN("tried to add knob with FrxCircuidViewPtr == NULL");
		return;
	}
	res->setLocation(0, 0);
	circ->add(res, FrxCircuidView::Z_Knobs);
	// add selection
	FrxHover::Ptr sel = FrxHover::create();
	circ->add(sel);
	sel->addElement(res);
}
//-----------------------------------------------------------------------------
template <class ConnectionType>
bool perfomConnect(FrxCircuidView::Ptr view, 
				   FrxComponent::Ptr src, 
				   FrxComponent::Ptr dst)
{
	// model stuff
	frx::processing::IModelController::Ptr ctrl;
	IViewModelMap::Ptr map;
	boost::tie(ctrl, map) = getControllerAndMap(view);
	frx::processing::ModelObject::Ptr msrc = map->getModelObject(src);
	frx::processing::ModelObject::Ptr mdst = map->getModelObject(dst);
	if (!msrc || !mdst) {
		SAMBAG_THROW(sambag::com::exceptions::IllegalStateException, 
			"tried to connect NULL object(s)."
		);
	}
	frx::processing::IConnection::Ptr mcnt = 
		connectModelObjects<ConnectionType>(ctrl, msrc, mdst);
	if (!mcnt)
		return false;
	// view stuff
	typename ConnectionType::Ptr cn = ConnectionType::create();
	cn->setSrcComponent(src);
	cn->setDstComponent(dst);
	view->add(cn, FrxCircuidView::Z_Wires);
	map->registerObjects(cn, mcnt);
	return true;
}
//-----------------------------------------------------------------------------
ColumnBrowser::Ptr openBrowser(fgc::FrxCircuidViewPtr view, 
		fgc::FrxComponentPtr c)
{
	static ColumnBrowser::Ptr browser;
	browser = FrxProcessorBrowser<BrowserNode>::create();
	browser->validate();
	browser->pack();
	browser->setTitle("Browser");
	browser->open();
	return browser;
}
////////////////////////////////////////////////////////////////////////////////
// Menu Entries
//-----------------------------------------------------------------------------
typedef std::pair<std::string, IFrxControl::CtrlCmd> Entry;
//-----------------------------------------------------------------------------
typedef std::list<Entry> Entries;
//-----------------------------------------------------------------------------
void createMainMenuEntries(Entries &out) {
	out.push_back( Entry("add volume processor",
		boost::bind(&addProcessorToView<FrxVolumeNode>, _1, _2, 1, 1)));
	out.push_back( Entry("add pan processor", 
		boost::bind(&addProcessorToView<FrxPanNode>, _1, _2, 1, 1)));
	out.push_back( Entry("add instep processor", 
		boost::bind(&addProcessorToView<FrxInStepNode>, _1, _2, 2, 1)));
	out.push_back( Entry("add outstep processor", 
		boost::bind(&addProcessorToView<FrxOutStepNode>, _1, _2, 1, 2)));
	out.push_back( Entry("add inswitch processor", 
		boost::bind(&addProcessorToView<FrxInSwitchNode>, _1, _2, 2, 1)));
	out.push_back( Entry("add outswitch processor",
		boost::bind(&addProcessorToView<FrxOutSwitchNode>, _1, _2, 1, 2)));
	out.push_back( Entry("add adsr transformer",
		boost::bind(&addProcessorToView<FrxADSRNode>, _1, _2, 1, 0)));
	out.push_back( Entry("add peak tracker", 
		boost::bind(&addProcessorToView<FrxPeakTrackerNode>, _1, _2, 1, 0)));
	out.push_back( Entry("add free knob",
		boost::bind(&addFreeKnobToView, _1, _2)));
	out.push_back( Entry("open plugin browser...",
		boost::bind(&openBrowser, _1, _2)));
}
//-----------------------------------------------------------------------------
sdc::PopupMenuPtr createPopupMenu(FrxCircuidViewPtr view, 
	const Entries &entries) 
{
	using namespace sambag::disco::components;
	PopupMenuPtr res = PopupMenu::create();
	BOOST_FOREACH(const Entry &e, entries) {
		MenuItem::Ptr item = MenuItem::create();
		item->setText(e.first);
		item->EventSender<sdc::events::ActionEvent>::addTrackedEventListener(
			getFrxControl(view).createCtrlCommandFunction(view, fgc::FrxComponentPtr(), e.second),
			view
		);
		res->add(item);
	}
	return res;
}
//=============================================================================
// class Connector
//=============================================================================
struct Connector {
	FrxCircuidView::Ptr view;
	Connector(FrxCircuidView::Ptr view) : view(view) {}
	bool OnError(FrxNode &a, FrxNode &b) {return false;}
	bool Fire(FrxNode &a, FrxNode &b) {return false;}
	// consider direction: out->in
	bool Fire(FrxInputNode &a, FrxOutputNode &b) { 
		return perfomConnect<IOCn>(view, b.getPtr(), a.getPtr());
	}
	bool Fire(FrxInputNode &a, FrxEntryNode &b) {
		return perfomConnect<IOCn>(view, b.getPtr(), a.getPtr());
	}
	bool Fire(FrxOutputNode &a, FrxExitNode &b) {
		return perfomConnect<IOCn>(view, a.getPtr(), b.getPtr());
	}
	bool Fire(FrxEntryNode &a, FrxExitNode &b) {
		return perfomConnect<IOCn>(view, a.getPtr(), b.getPtr());
	}
	bool Fire(FrxStdKnob &a, FrxStdKnob &b) {
		return perfomConnect<ParameterCn>(view, a.getPtr(), b.getPtr());
	}
};
} // namespace
//=============================================================================
//  Class FrxControl
//=============================================================================
//-----------------------------------------------------------------------------
void FrxControl::removeComponent(FrxCircuidViewPtr _view, FrxComponentPtr _c)
{
	FrxCircuidViewPtr view(_view);
	FrxComponentPtr c(_c);
	if (!c || !view)
		return;
	// get model obj.
	frx::processing::IModelController::Ptr ctrl;
	IViewModelMap::Ptr map;
	boost::tie(ctrl, map) = getControllerAndMap(view);
	frx::processing::ModelObject::Ptr mObj = map->getModelObject(c);
	if (!mObj) {
		SAMBAG_THROW(sambag::com::exceptions::IllegalStateException, 
			"access to model object failed while removing."
		);
	}

	if (!mObj->requestRemove(mObj)) {
		SAMBAG_THROW(sambag::com::exceptions::IllegalStateException, 
			"removing model object failed."
		);
	}
	map->remove(c, mObj);
	view->remove(c);
	view->AContainer::redraw();
}
//-----------------------------------------------------------------------------
sdc::PopupMenuPtr FrxControl::getCircuidViewPopup(FrxCircuidViewPtr c) {
	Entries mainMenuEntries;
	createMainMenuEntries(mainMenuEntries);
	return createPopupMenu(c, mainMenuEntries);
}
//-----------------------------------------------------------------------------
bool FrxControl::connect(FrxCircuidViewPtr view, FrxNodePtr from, FrxNodePtr to) 
{
	typedef LOKI_TYPELIST_5(
		FrxInputNode,
		FrxOutputNode, 
		FrxEntryNode, 
		FrxExitNode,
		FrxStdKnob
	) Types;
	typedef Loki::StaticDispatcher  <
		Connector,
        FrxNode, 
        Types,
		true,
        FrxNode,
        Types,
        bool
    > Dispatcher;
	Dispatcher disp;
	try {
		return disp.Go(*(from.get()), *(to.get()), Connector(view));
	} catch (...) {
		return false;
	}
}
//-----------------------------------------------------------------------------
void FrxControl::handleContextMenuPopup(const sdc::events::MouseEvent &ev) {
	using namespace sambag::disco::components;
	if (ev.getButtons() != sdc::events::MouseEvent::DISCO_BTN2) {
		if (currPopup)
			currPopup->hidePopup();
		return;
	}
	sdc::PopupMenuPtr popup = ev.getSource()->getComponentPopupMenu();
	if (!popup)
		return;
	if (!popup->isPopupVisible()) {
		MenuSelectionManager &m = MenuSelectionManager::defaultManager();
		m.clearSelectedPath();
		IMenuElement::MenuElements p;
		p.push_back(popup);
		m.setSelectedPath(p);
		popup->setInvoker(ev.getSource());
		popup->showPopup(
			ev.getLocationOnScreen()
		);
	}
	currPopup = popup;
}
//-----------------------------------------------------------------------------
boost::tuple<fgc::FrxNodePtr, fgc::FrxNodePtr>
FrxControl::createEntryExtitNodes(fgc::FrxCircuidViewPtr circ)
{
	frx::processing::IModelController::Ptr ctrl;
	IViewModelMap::Ptr map;
	boost::tie(ctrl, map) = getControllerAndMap(circ);
	boost::tuple<fgc::FrxNodePtr, fgc::FrxNodePtr> res;

	// entry node
	boost::get<0>(res) = FrxEntryNode::create();
	circ->add(boost::get<0>(res), FrxCircuidView::Z_IO);
	map->registerObjects(boost::get<0>(res), ctrl->getEntry());
	
	//exit node
	boost::get<1>(res) = FrxExitNode::create();
	circ->add(boost::get<1>(res), FrxCircuidView::Z_IO);
	map->registerObjects(boost::get<1>(res), ctrl->getExit());

	return res;

}
//-----------------------------------------------------------------------------
void FrxControl::executeCtrlCommand(void *src,
		const sdc::events::ActionEvent &ev,
		fgc::FrxCircuidViewWPtr v, 
		fgc::FrxComponentWPtr c, 
		CtrlCmd cmd)
{
	FrxCircuidViewPtr view = v.lock();
	FrxComponentPtr comp = c.lock(); // can be null
	SAMBAG_ASSERT(view);
	try {
		cmd(view,comp);
	} catch(const std::exception &ex) {
		view->errorMessage("operation failed: " + std::string(ex.what()));
	} catch (...) {
		view->errorMessage("operation failed: unkown error.");
	}
}
//-----------------------------------------------------------------------------
sambag::com::events::EventSender<sdc::events::ActionEvent>::EventFunction
FrxControl::createCtrlCommandFunction(fgc::FrxCircuidViewPtr view,
	fgc::FrxComponentPtr comp,
	const IFrxControl::CtrlCmd &cmdF)
{
	FrxCircuidViewWPtr wView = view;
	FrxComponentWPtr wC = comp;
	return boost::bind(&FrxControl::executeCtrlCommand, 
		this, 
		_1, 
		_2, 
		wView, 
		wC,
		cmdF
	);
}
//-----------------------------------------------------------------------------
void onBrowserOk(void *src,
	const sdc::events::ActionEvent &ev,
	ColumnBrowser::WPtr _browser)
{
	// lock weak ptr
	ColumnBrowser::Ptr browser = _browser.lock();
	SAMBAG_ASSERT(browser);
	// get selection path
	typedef ColumnBrowser::BrowserImpl Tree;
	Tree::Ptr tree = browser->getBrowserImpl();
	const Tree::Path &path = tree->getSelectionPath();
	if (path.empty())
		return;
	const BrowserNode &bNode = tree->getNodeData(path.back());
	bNode.accept();
}
//-----------------------------------------------------------------------------
void FrxControl::showProcessorDetails(fgc::FrxCircuidViewPtr view, 
		fgc::FrxComponentPtr c)
{
	// get ctrl, map
	frx::processing::IModelController::Ptr ctrl;
	IViewModelMap::Ptr map;
	boost::tie(ctrl, map) = getControllerAndMap(view);
	// create browser
	ProcessorBrowser::Ptr browser = boost::shared_dynamic_cast<ProcessorBrowser>( 
		openBrowser(view, c) 
	);
	browser->setTitle(c->getName() + " details");
	// add btnOk listener
	browser->getBtnOk()->EventSender<sdc::events::ActionEvent>::addEventListener(
		boost::bind(&onBrowserOk, _1, _2, ColumnBrowser::WPtr(browser))
	);
	// get model obj
	frx::processing::IProcessor::Ptr pr = 
		boost::shared_dynamic_cast<frx::processing::IProcessor>(map->getModelObject(c));
	if (!pr)
		return;
	// create browser tree
	typedef ColumnBrowser::BrowserImpl Tree;
	Tree::Ptr tree = browser->getBrowserImpl();
	Tree::Node parameter = 
		tree->addNode(tree->getRootNode(), BrowserNode(c->getName() + " parameters"));
	// create browser nodes
	for (size_t i=0; i<pr->getNumParameter(); ++i) {
		frx::processing::IParameter::Ptr p = pr->getParameter(i);
		BrowserNode::AcceptedFunction f = 
			boost::bind(&addProcesorKnobToView, 
				fgc::FrxCircuidViewWPtr(view), 
				fgc::FrxComponentWPtr(c),
				frx::processing::IParameter::WPtr(p)
			);
		tree->addNode(
			parameter, 
			BrowserNode(p->getName(), f)
		);
	}
	tree->updateLists();
}
//=============================================================================
//-----------------------------------------------------------------------------
IFrxControl & getFrxControl(FrxCircuidViewPtr view) {
	typedef Loki::SingletonHolder<FrxControl> FactoryHolder;
	return FactoryHolder::Instance();
}
}} // namespace(s)
