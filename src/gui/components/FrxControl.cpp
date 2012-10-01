/*
 * FrxControl.cpp
 *
 *  Created on: Thu Sep 27 14:49:27 2012
 *      Author: Johannes Unger
 */

#include "FrxControl.hpp"
#include "FrxCircuidView.hpp"
#include "FrxConcreteProcessor.hpp"
#include "FrxConcreteParameter.hpp"
#include <sambag/disco/components/PopupMenu.hpp>
#include "FrxHover.hpp"
#include <boost/foreach.hpp>
#include <sambag/com/Common.hpp>

namespace frx { namespace gui { namespace components {
//=============================================================================
//  Private FrxControl Functors
//=============================================================================
//-----------------------------------------------------------------------------
namespace {
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
void createMainMenuEntries(FrxCircuidViewPtr c, FrxControl::Entries &out) {
	typedef FrxControl::Entry Entry;
	FrxCircuidViewWPtr _c = c;
	out.push_back( Entry("add volume processor", boost::bind(&addProcessorToView<FrxVolumeNode>, _c, 1, 1)));
	out.push_back( Entry("add pan processor", boost::bind(&addProcessorToView<FrxPanNode>, _c, 1, 1)));
	out.push_back( Entry("add instep processor", boost::bind(&addProcessorToView<FrxInStepNode>, _c, 2, 1)));
	out.push_back( Entry("add outstep processor", boost::bind(&addProcessorToView<FrxOutStepNode>, _c, 1, 2)));
	out.push_back( Entry("add inswitch processor", boost::bind(&addProcessorToView<FrxInSwitchNode>, _c, 2, 1)));
	out.push_back( Entry("add outswitch processor", boost::bind(&addProcessorToView<FrxOutSwitchNode>, _c, 1, 2)));
	out.push_back( Entry("add adsr transformer", boost::bind(&addProcessorToView<FrxADSRNode>, _c, 1, 0)));
	out.push_back( Entry("add peak tracker", boost::bind(&addProcessorToView<FrxPeakTrackerNode>, _c, 1, 0)));
	out.push_back( Entry("add free knob", boost::bind(&addFreeKnobToView, _c)));

}
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
sdc::PopupMenuPtr FrxControl::getCircuidViewPopup(FrxCircuidViewPtr c) {
	using namespace sambag::disco::components;
	
	Entries mainMenuEntries;
	createMainMenuEntries(c, mainMenuEntries);

	PopupMenuPtr res = PopupMenu::create();
	BOOST_FOREACH(const Entry &e, mainMenuEntries) {
		MenuItem::Ptr item = MenuItem::create();
		item->setText(e.first);
		item->EventSender<sdc::events::ActionEvent>::addTrackedEventListener(
			boost::bind(&FrxControl::onMenuAction, this, _1, _2, e.second),
			c
		);
		res->add(item);
	}
	return res;
}
//=============================================================================
//-----------------------------------------------------------------------------
FrxControl & getFrxControl() {
	typedef Loki::SingletonHolder<FrxControl> FactoryHolder;
	return FactoryHolder::Instance();
}
}}} // namespace(s)
