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
void FrxFlag::onTargetProperty(const sce::PropertyChanged &ev) {
    if (ev.getPropertyName() == sdc::AComponent::PROPERTY_VISIBILITY) {
        bool b;
        ev.getNewValue(b);
        setVisible(b);
    }
}
//-----------------------------------------------------------------------------
void FrxFlag::installListeners() {
    if (rmvConnection.connected()) {
		rmvConnection.disconnect();
	}
	if (propertyConnection.connected()) {
		propertyConnection.disconnect();
	}
	rmvConnection = target->sce::EventSender<OnRemoving>::addTrackedEventListener(
		boost::bind(&FrxFlag::onComponentRemoving, this, _1, _2),
		getPtr()
	);
    propertyConnection =
        target->sce::EventSender<sce::PropertyChanged>::addTrackedEventListener(
            boost::bind(&FrxFlag::onTargetProperty, this, _2),
            getPtr()
    );
}
//-----------------------------------------------------------------------------
void FrxFlag::setTarget(FrxComponent::Ptr target) {
	FrxComponent::Ptr old = this->target;
	this->target = target;
	firePropertyChanged(PROPERTY_TARGET, old, target);
    installListeners();
}
//-----------------------------------------------------------------------------
FrxFlag::~FrxFlag() {
	
}
}}} // namespace(s)
