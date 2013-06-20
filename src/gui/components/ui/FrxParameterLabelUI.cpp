/*
 * FrxParameterLabelUI.cpp
 *
 *  Created on: Thu Nov 15 08:53:50 2012
 *      Author: Johannes Unger
 */

#include "FrxParameterLabelUI.hpp"
#include <gui/components/FrxParameterLabel.hpp>
#include <sambag/disco/components/ui/UIManager.hpp>

namespace frx { namespace gui {
namespace components { namespace ui { 
//=============================================================================
//  Class FrxParameterLabelUI
//=============================================================================
//-----------------------------------------------------------------------------
void FrxParameterLabelUI::installDefaults(sdc::AComponentPtr c) {
	sdc::ui::UIManager &m = sdc::ui::getUIManager();
	m.getProperty("FrxParameterLabel.style", style);
}
//-----------------------------------------------------------------------------
void FrxParameterLabelUI::installListeners(sdc::AComponentPtr c) {
}
//-----------------------------------------------------------------------------
void FrxParameterLabelUI::installUI(sdc::AComponentPtr c) {
	installDefaults(c);
	installListeners(c);
}
//-----------------------------------------------------------------------------
void FrxParameterLabelUI::draw(sd::IDrawContext::Ptr cn, sdc::AComponentPtr c) {
	FrxParameterLabel::Ptr label = 
		boost::dynamic_pointer_cast<FrxParameterLabel>(c);
	SAMBAG_ASSERT(label);
	sambag::com::Number value = label->getValue();
	Super::draw(cn, c);
	// draw bar:
	if (value==0.0)
		return;
	sd::Rectangle r = c->getBounds();
	sd::Coordinate w = r.width() * value;
	style.intoContext(cn);
	cn->rect(sd::Rectangle(0,0, w, r.height()));
	cn->fill();
	cn->rect(sd::Rectangle(0,0, w, r.height()));
	cn->stroke();
}
}}}} // namespace(s)
