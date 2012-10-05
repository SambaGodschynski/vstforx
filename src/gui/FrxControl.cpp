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

namespace frx { namespace gui {
using namespace components;
////////////////////////////////////////////////////////////////////////////////
//  Private executives
//-----------------------------------------------------------------------------
namespace {
template <class ProcessorType>
void addStdComponentMenuEntries(FrxCircuidViewPtr c, 
			typename ProcessorType::Ptr obj,
			FrxControl::Entries &out);
//-----------------------------------------------------------------------------
void doNothing(){}
//-----------------------------------------------------------------------------
template <class ConcreteProcessor>
typename ConcreteProcessor::Ptr 
createProcessor(FrxCircuidViewWPtr c) 
{
	typename ConcreteProcessor::Ptr res = ConcreteProcessor::create();
	return res;
}
//-----------------------------------------------------------------------------
template <class ConcreteProcessor>
void addProcessorToView(FrxCircuidViewWPtr c, int numInputs, int numOutputs) {
	typename ConcreteProcessor::Ptr res = createProcessor<ConcreteProcessor>(c);
	if (!res) {
		return;
	}
	FrxCircuidViewPtr circ(c);
	if (!circ) {
		SAMBAG_WARN("tried to add processor with FrxCircuidViewPtr == NULL");
		return;
	}
	circ->add(res, FrxCircuidView::Z_ProcessorNodes);
	res->setLocation(0, 0);
	res->configIO(numInputs, numOutputs);
	// add selection
	FrxHover::Ptr sel = FrxHover::create();
	circ->add(sel);
	sel->addElement(res);
	sel->addElements(res->getInputs());
	sel->addElements(res->getOutputs());
	// create contextmenu
	FrxControl::Entries entries;
	addStdComponentMenuEntries<ConcreteProcessor>(circ, res, entries);
	res->setComponentPopupMenu(
		getFrxControl(circ).createPopupMenu(res, entries)
	);
}
//-----------------------------------------------------------------------------
void addFreeKnobToView(FrxCircuidViewWPtr c) {
	FrxStdKnob::Ptr res = FrxStdKnob::create();
	if (!res) {
		return;
	}
	FrxCircuidViewPtr circ(c);
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
void removeComponent(FrxCircuidViewWPtr _view, FrxComponentWPtr _c) {
	FrxCircuidViewPtr view(_view);
	FrxComponentPtr c(_c);
	if (!c || !view)
		return;
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
template <class ConnectionType>
bool perfomConnect(FrxCircuidView::Ptr view, 
				   FrxComponent::Ptr src, 
				   FrxComponent::Ptr dst) 
{
	typename ConnectionType::Ptr cn = ConnectionType::create();
	cn->setSrcComponent(src);
	cn->setDstComponent(dst);
	view->add(cn, FrxCircuidView::Z_Wires);
	// create contextmenu
	FrxControl::Entries entries;
	addStdComponentMenuEntries<ConnectionType>(view, cn , entries);
	cn->setComponentPopupMenu(
		getFrxControl(view).createPopupMenu(cn, entries)
	);
	return true;
}
////////////////////////////////////////////////////////////////////////////////
// Menu Entries
//-----------------------------------------------------------------------------
template <class FrxComponentType>
void addStdComponentMenuEntries(FrxCircuidViewPtr view, 
			typename FrxComponentType::Ptr obj,
			FrxControl::Entries &out)
{
	typedef FrxControl::Entry Entry;
	FrxCircuidViewWPtr _view = view; // always use weakptr for menus !
	FrxComponentWPtr _obj = obj;
	out.push_back( Entry("remove " + obj->getName(), 
		boost::bind(&removeComponent, _view, _obj)));
}
//-----------------------------------------------------------------------------
void createMainMenuEntries(FrxCircuidViewPtr c, FrxControl::Entries &out) {
	typedef FrxControl::Entry Entry;
	FrxCircuidViewWPtr _c = c; // always use weakptr for menus !
	out.push_back( Entry("add volume processor",
		boost::bind(&addProcessorToView<FrxVolumeNode>, _c, 1, 1)));
	out.push_back( Entry("add pan processor", 
		boost::bind(&addProcessorToView<FrxPanNode>, _c, 1, 1)));
	out.push_back( Entry("add instep processor", 
		boost::bind(&addProcessorToView<FrxInStepNode>, _c, 2, 1)));
	out.push_back( Entry("add outstep processor", 
		boost::bind(&addProcessorToView<FrxOutStepNode>, _c, 1, 2)));
	out.push_back( Entry("add inswitch processor", 
		boost::bind(&addProcessorToView<FrxInSwitchNode>, _c, 2, 1)));
	out.push_back( Entry("add outswitch processor",
		boost::bind(&addProcessorToView<FrxOutSwitchNode>, _c, 1, 2)));
	out.push_back( Entry("add adsr transformer",
		boost::bind(&addProcessorToView<FrxADSRNode>, _c, 1, 0)));
	out.push_back( Entry("add peak tracker", 
		boost::bind(&addProcessorToView<FrxPeakTrackerNode>, _c, 1, 0)));
	out.push_back( Entry("add free knob",
		boost::bind(&addFreeKnobToView, _c)));
}
//=============================================================================
// class Connector
//=============================================================================
struct Connector {
	FrxCircuidView::Ptr view;
	Connector(FrxCircuidView::Ptr view) : view(view) {}
	bool OnError(FrxNode &a, FrxNode &b) {return false;}
	bool Fire(FrxNode &a, FrxNode &b) {return false;}
	bool Fire(FrxInputNode &a, FrxOutputNode &b) { 
		return perfomConnect<IOCn>(view, a.getPtr(), b.getPtr());
	}
	bool Fire(FrxInputNode &a, FrxEntryNode &b) {
		return perfomConnect<IOCn>(view, a.getPtr(), b.getPtr());
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
void FrxControl::onMenuAction(void *src, 
				  const sdc::events::ActionEvent &ev, 
				  const CtrlFunc &cmd)
{
	cmd();
}
//-----------------------------------------------------------------------------
sdc::PopupMenuPtr FrxControl::createPopupMenu(FrxControl::AnyWPtr anyPtr, 
	const FrxControl::Entries &entries) 
{
	using namespace sambag::disco::components;
	PopupMenuPtr res = PopupMenu::create();
	BOOST_FOREACH(const Entry &e, entries) {
		MenuItem::Ptr item = MenuItem::create();
		item->setText(e.first);
		item->EventSender<sdc::events::ActionEvent>::addTrackedEventListener(
			boost::bind(&FrxControl::onMenuAction, this, _1, _2, e.second),
			anyPtr
		);
		res->add(item);
	}
	return res;
}
//-----------------------------------------------------------------------------
sdc::PopupMenuPtr FrxControl::getCircuidViewPopup(FrxCircuidViewPtr c) {
	Entries mainMenuEntries;
	createMainMenuEntries(c, mainMenuEntries);
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
	return disp.Go(*(from.get()), *(to.get()), Connector(view));
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
//=============================================================================
//-----------------------------------------------------------------------------
FrxControl & getFrxControl(FrxCircuidViewPtr view) {
	typedef Loki::SingletonHolder<FrxControl> FactoryHolder;
	return FactoryHolder::Instance();
}
}} // namespace(s)
