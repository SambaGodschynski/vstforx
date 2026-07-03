/*
 * FrxComponent.cpp
 *
 *  Created on: 17. Aug 2012
 *      Author: sambag
 */

#include "FrxComponent.hpp"
#include <com/Serialization.h>
#include <sambag/disco/components/ui/ALookAndFeel.hpp>
#include <boost/algorithm/string.hpp>
#include <gui/components/ui/FrxComponentUI.hpp>
#include <gui/components/FrxCircuidView.hpp>
#include <sambag/disco/components/BorderLayout.hpp>

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
    setLayout(sdc::ALayoutManager::Ptr());
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
//-----------------------------------------------------------------------------
void FrxComponent::serializeSelfPtr(::com::iArchive &ar, const unsigned int version) {
	ar >> boost::serialization::base_object<ViewObject>(*this); 
	ar >> tmpSelf;
	ar >> uFlagTxt;
	ar >> lFlagTxt;
	std::string name = getName();
	ar >> name;
	if (version>0) {
		ar >> typeId;
	}
	self = tmpSelf;
	setName(name);
	postConstructor();
}
void FrxComponent::serializeSelfPtr(::com::oArchive &ar, const unsigned int version) {
	tmpSelf = boost::dynamic_pointer_cast<FrxComponent>(self.lock());
	ar << boost::serialization::base_object<ViewObject>(*this); 
	ar << tmpSelf;
	ar << uFlagTxt;
	ar << lFlagTxt;
	std::string name = getName();
	ar << name;
	ar << typeId;
}
//-----------------------------------------------------------------------------
void FrxComponent::save(::com::oArchive &ar, const unsigned int version) const {
	const sd::Rectangle &bounds = getBounds();
	ar << bounds;
    bool b=isVisible();
    ar << b;
}
//----------------------------------------------------------------------------- 
void FrxComponent::load(::com::iArchive &ar, const unsigned int version) {
	sd::Rectangle bounds;
	ar >> bounds;
	setBounds(bounds);
    if (version>1) {
        bool b;
        ar >> b;
        setVisible(b);
    }
}
//------------------------------------------------------------------------------
void FrxComponent::serialize(::com::oArchive &ar, const unsigned int version) {
	serializeSelfPtr(ar, version);
	boost::serialization::split_member(ar, *this, version);
}
//------------------------------------------------------------------------------
void FrxComponent::serialize(::com::iArchive &ar, const unsigned int version) {
	serializeSelfPtr(ar, version);
	boost::serialization::split_member(ar, *this, version);
}
//------------------------------------------------------------------------------
void FrxComponent::setName (const std::string &name) {
    Super::setName(name);
    setUpperFlagText(name);
}
}}} // namespace(s)
