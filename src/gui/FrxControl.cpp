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

namespace frx { namespace gui {
using namespace components;
namespace {
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
					   frx::processing::IProcessor::Ptr m) 
{
	map->registerObjects(v, m);
	const FrxProcessorNode::IOContainer &ins = v->getInputs();
	for (int i=0; i<m->getNumInputs(); ++i) {
		frx::processing::INode::Ptr node = m->getInput(i);
		map->registerObjects(ins[i], node);
	}
	const FrxProcessorNode::IOContainer &outs = v->getOutputs();
	for (int i=0; i<m->getNumOutputs(); ++i) {
		frx::processing::INode::Ptr node = m->getOutput(i);
		map->registerObjects(outs[i], node);
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
	registerProcessor(map, viewObj, mObj);
	// hover
	FrxHover::Ptr sel = FrxHover::create();
	circ->add(sel);
	sel->addElement(viewObj);
	sel->addElements(viewObj->getInputs());
	sel->addElements(viewObj->getOutputs());
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
	// create model obj.
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
	FrxProcessorNode::Ptr pr = boost::shared_dynamic_cast<FrxProcessorNode>(c);
	if (pr) {
		BOOST_FOREACH(FrxNode::Ptr io, pr->getInputs()) {
			view->remove(io);
		}
		BOOST_FOREACH(FrxNode::Ptr io, pr->getOutputs()) {
			view->remove(io);
		}
	}
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
//=============================================================================
//-----------------------------------------------------------------------------
IFrxControl & getFrxControl(FrxCircuidViewPtr view) {
	typedef Loki::SingletonHolder<FrxControl> FactoryHolder;
	return FactoryHolder::Instance();
}
}} // namespace(s)
