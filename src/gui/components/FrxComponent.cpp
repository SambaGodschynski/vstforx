/*
 * FrxComponent.cpp
 *
 *  Created on: 17. Aug 2012
 *      Author: sambag
 */

#include "FrxComponent.hpp"
#include <sambag/disco/components/ui/ALookAndFeel.hpp>
#include <boost/algorithm/string.hpp>

namespace frx { namespace gui { namespace components {
//=============================================================================
//  Class FrxComponent
//=============================================================================
//-----------------------------------------------------------------------------
const std::string FrxComponent::PROPERTY_FLAG_TXT = "flag_text";
//-----------------------------------------------------------------------------
void FrxComponent::setFlagText(const std::string &txt) {
	std::string old = getFlagText();
	std::vector<std::string> strs;
	strs.reserve(2);
	boost::split(strs, txt, boost::is_any_of("/"));
	if (strs.size() >= 1) {
		uFlagTxt = strs[0];
	}
	if (strs.size() >= 2) {
		lFlagTxt = strs[1];
	}
	firePropertyChanged(PROPERTY_FLAG_TXT, old, getFlagText());
}
//-----------------------------------------------------------------------------
void FrxComponent::setUpperFlagText(const std::string &txt) {
	std::string old = getFlagText();
	uFlagTxt = txt;
	firePropertyChanged(PROPERTY_FLAG_TXT, old, getFlagText());
}
//-----------------------------------------------------------------------------
void FrxComponent::setLowerFlagText(const std::string &txt) {
	std::string old = getFlagText();
	lFlagTxt = txt;
	firePropertyChanged(PROPERTY_FLAG_TXT, old, getFlagText());
}
//-----------------------------------------------------------------------------
void FrxComponent::postConstructor() {
	Super::postConstructor();
	if (getFlagText()=="") {
		setFlagText(getName());
	}
}
//-----------------------------------------------------------------------------
FrxComponent::FrxComponent() {
	setName("");
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
