/*
 * FrxComponent.cpp
 *
 *  Created on: 17. Aug 2012
 *      Author: sambag
 */

#include "FrxComponent.hpp"
#include <sambag/disco/components/ui/ALookAndFeel.hpp>

namespace frx { namespace gui { namespace components {
//=============================================================================
//  Class FrxComponent
//=============================================================================
//-----------------------------------------------------------------------------
const std::string FrxComponent::PROPERTY_FLAG_TXT = "flag_text";
//-----------------------------------------------------------------------------
void FrxComponent::setFlagText(const std::string &txt) {
	std::string old = flagTxt;
	flagTxt = txt;
	firePropertyChanged(PROPERTY_FLAG_TXT, old, flagTxt);
}
//-----------------------------------------------------------------------------
FrxComponent::FrxComponent() {
	setName("FrxComponent");
	setFlagText(getName());
}
//-----------------------------------------------------------------------------
sdc::ui::AComponentUIPtr 
FrxComponent::createComponentUI(sdc::ui::ALookAndFeelPtr laf) const 
{
	return laf->getUI<FrxComponent>();
}
//-----------------------------------------------------------------------------
sd::Point2D FrxComponent::getPivot() const {
	sambag::com::Number w = getWidth(), h = getHeight();
	return sd::Point2D(w/2., h/2.);
}
//-----------------------------------------------------------------------------
void FrxComponent::setBounds(const sd::Rectangle &b) {
	Super::setBounds(b);
}
}}} // namespace(s)
