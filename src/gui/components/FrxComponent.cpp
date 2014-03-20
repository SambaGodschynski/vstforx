/*
 * FrxComponent.cpp
 *
 *  Created on: 17. Aug 2012
 *      Author: sambag
 */

#include "FrxComponent.hpp"
#include <sambag/disco/components/ui/ALookAndFeel.hpp>
#include <boost/algorithm/string.hpp>
#include <gui/components/ui/FrxComponentUI.hpp>
#include <gui/components/FrxCircuidView.hpp>

namespace frx { namespace gui { namespace components {
//=============================================================================
//  Class FrxComponent
//=============================================================================
//-----------------------------------------------------------------------------
const std::string FrxComponent::PROPERTY_UPFLAG_TXT = "upper_flag_text";
const std::string FrxComponent::PROPERTY_LOFLAG_TXT = "lower_flag_text";
//-----------------------------------------------------------------------------
void FrxComponent::setUpperFlagText(const std::string &txt) {
	std::string old = getUpperFlagText();
	uFlagTxt = txt;
	firePropertyChanged(PROPERTY_UPFLAG_TXT, old, txt);
}
//-----------------------------------------------------------------------------
void FrxComponent::setLowerFlagText(const std::string &txt) {
	std::string old = getLowerFlagText();
	lFlagTxt = txt;
	firePropertyChanged(PROPERTY_LOFLAG_TXT, old, txt);
}
//-----------------------------------------------------------------------------
void FrxComponent::postConstructor() {
	Super::postConstructor();
	if (getUpperFlagText()=="") {
		setUpperFlagText(getName());
	}
}
//-----------------------------------------------------------------------------
void FrxComponent::__setTypeId_(const std::string &id) {
    typeId = id;
}
//-----------------------------------------------------------------------------
FrxComponent::FrxComponent() {
	setName("");
}
//----------------------------------------------------------------------------
FrxComponent::~FrxComponent() {
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
