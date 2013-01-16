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
const std::string FrxParameter::PROPERTY_RANGE_MODEL = "property_range_model";
//-----------------------------------------------------------------------------
FrxParameter::FrxParameter()  {
	setName("parameter");
}
//-----------------------------------------------------------------------------
sdcu::AComponentUIPtr 
FrxParameter::createComponentUI(sdcu::ALookAndFeelPtr laf) const
{
	return laf->getUI<FrxParameter>();
}

//-----------------------------------------------------------------------------
void FrxParameter::setRangeModel(sdc::DefaultBoundedRangeModel::Ptr model) {
	sdc::DefaultBoundedRangeModel::Ptr old = this->rangeModel;
	this->rangeModel = model;
	firePropertyChanged(PROPERTY_RANGE_MODEL, old, model);
}
//-----------------------------------------------------------------------------
void FrxParameter::postConstructor() {
}
//-----------------------------------------------------------------------------
void FrxParameter::setEncapsulatedCtrl(sdc::AComponent::Ptr ctrl) {
	sdc::AComponent::Ptr old = this->ctrl;
	if (old) {
		remove(old);
	}
	add(ctrl);
	
	this->ctrl = ctrl;
	updateChildLocation(ctrl);
	firePropertyChanged(PROPERTY_ENC_CTRL, old, ctrl);
}
}}} // namespace(s)
