/*
 * FrxFlag.cpp
 *
 *  Created on: Sun Dec 23 14:03:14 2012
 *      Author: Johannes Unger
 */

#include "FrxFlag.hpp"
#include <sambag/disco/components/ui/ALookAndFeel.hpp>
#include "FrxCircuidView.hpp"


namespace frx { namespace gui { namespace components {
//=============================================================================
//  Class FrxFlag
//=============================================================================
//-----------------------------------------------------------------------------
const std::string FrxFlag::PROPERTY_TARGET = "flag_target";
//-----------------------------------------------------------------------------
sdcu::AComponentUIPtr 
FrxFlag::createComponentUI(sdcu::ALookAndFeelPtr laf) const
{
	return laf->getUI<FrxFlag>();
}
//-----------------------------------------------------------------------------
void FrxFlag::onComponentRemoving(void *src, const OnRemoving &ev) {
	Ptr saftey = getPtr(); // hold object
	ev.view->remove(saftey);
}
//-----------------------------------------------------------------------------
void FrxFlag::redraw() {
	AContainer::Ptr c = getParent();
	if (!c) {
		return;
	}
	sd::Rectangle b = getBounds();
	c->redraw(b);
}
//-----------------------------------------------------------------------------
void FrxFlag::setTarget(FrxComponent::Ptr target) {
	FrxComponent::Ptr old = this->target;
	if (rmvConnection.connected()) {
		rmvConnection.disconnect();
	}
	this->target = target;
	firePropertyChanged(PROPERTY_TARGET, old, target);
	rmvConnection = target->sce::EventSender<OnRemoving>::addTrackedEventListener(
		boost::bind(&FrxFlag::onComponentRemoving, this, _1, _2),
		getPtr()
	);
}
//-----------------------------------------------------------------------------
FrxFlag::~FrxFlag() {
	
}
}}} // namespace(s)
