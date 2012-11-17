/*
 * FrxParameterLabelUI.cpp
 *
 *  Created on: Thu Nov 15 08:53:50 2012
 *      Author: Johannes Unger
 */

#include "FrxParameterLabelUI.hpp"
#include <gui/components/FrxParameterLabel.hpp>
namespace frx { namespace gui {
namespace components { namespace ui { 
//=============================================================================
//  Class FrxParameterLabelUI
//=============================================================================
//-----------------------------------------------------------------------------
void FrxParameterLabelUI::installUI(sdc::AComponentPtr c) {
}
//-----------------------------------------------------------------------------
void FrxParameterLabelUI::draw(sd::IDrawContext::Ptr cn, sdc::AComponentPtr c) {
	FrxParameterLabel::Ptr label = 
		boost::shared_dynamic_cast<FrxParameterLabel>(c);
	SAMBAG_ASSERT(label);
	sambag::com::Number value = label->getValue();
	Super::draw(cn, c);
	// draw bar:
	sd::Rectangle r = c->getBounds();
	cn->setFillColor(sd::ColorRGBA(0,0,0,0.3));
	cn->rect(sd::Rectangle(0,0, r.width() * value, r.height()));
	cn->fill();
}
}}}} // namespace(s)
