/*
 * FrxParameter.cpp
 *
 *  Created on: Mon Aug 20 10:45:54 2012
 *      Author: Johannes Unger
 */

#include "FrxParameter.hpp"
#include <sambag/disco/components/ui/ALookAndFeel.hpp>

namespace frx { namespace gui { namespace components {
//=============================================================================
//  Class FrxParameter
//=============================================================================
//-----------------------------------------------------------------------------
const std::string FrxParameter::PROPERTY_ENC_CTRL = "property_encapsulated_ctrl";
//-----------------------------------------------------------------------------
FrxParameter::FrxParameter()  {
	setName("FrxParameter");
}
//-----------------------------------------------------------------------------
sdcu::AComponentUIPtr 
FrxParameter::createComponentUI(sdcu::ALookAndFeelPtr laf) const
{
	return laf->getUI<FrxParameter>();
}
//-----------------------------------------------------------------------------
void FrxParameter::updateCtrlLocation() {
	if (!ctrl)
		return;
	// translate ctrl to center
	sd::Point2D loc ( 
		getWidth()/2. - ctrl->getWidth()/2.,
		getHeight()/2. - ctrl->getHeight()/2.
	);
	ctrl->setLocation(loc);
}
//-----------------------------------------------------------------------------
void FrxParameter::onCtrlChanged(void *src, const sce::PropertyChanged &ev) {
	if (ev.getPropertyName() == sdc::AComponent::PROPERTY_BOUNDS)
		updateCtrlLocation();
}
//-----------------------------------------------------------------------------
void FrxParameter::setBounds(const sd::Rectangle &r) {
	Super::setBounds(r);
	updateCtrlLocation();
}
//-----------------------------------------------------------------------------
void FrxParameter::postConstructor() {
}
//-----------------------------------------------------------------------------
void FrxParameter::setEncapsulatedCtrl(sdc::AComponent::Ptr ctrl) {
	sdc::AComponent::Ptr old = this->ctrl;
	if (old) {
		remove(old);
		ctrlConnection.disconnect();
	}
	add(ctrl);

	ctrlConnection = ctrl->EventSender<sce::PropertyChanged>::addEventListener (
		boost::bind(&FrxParameter::onCtrlChanged, this, _1, _2)
	);
	
	this->ctrl = ctrl;
	updateCtrlLocation();
	firePropertyChanged(PROPERTY_ENC_CTRL, old, ctrl);
}
}}} // namespace(s)
