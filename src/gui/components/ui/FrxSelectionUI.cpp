/*
 * FrxSelectionUI.cpp
 *
 *  Created on: Tue Aug 28 10:47:37 2012
 *      Author: Johannes Unger
 */

#include "FrxSelectionUI.hpp"
#include <sambag/disco/components/AComponent.hpp>
#include <sambag/disco/components/ui/UIManager.hpp>

namespace frx { namespace gui {
namespace components { namespace ui { 
//=============================================================================
//  Class FrxSelectionUI
//=============================================================================
//-----------------------------------------------------------------------------
void FrxSelectionUI::installListeners(sdc::AComponentPtr c) {
	mouseListener = FrxSelectionMouseListener::create();
	c->sdc::EventSender<sdc::events::MouseEvent>::addTrackedEventListener (
		boost::bind(&FrxSelectionMouseListener::onMouse, mouseListener.get(), _1, _2),
		getPtr()
	);
}
//-----------------------------------------------------------------------------
void FrxSelectionUI::installDefaults(sdc::AComponentPtr c) {
	sdc::ui::UIManager &m = sdc::ui::getUIManager();
	m.getProperty("FrxSelection.selectedStyle", selectedStyle);
}
//-----------------------------------------------------------------------------
void FrxSelectionUI::installUI(sdc::AComponentPtr c) {
	installDefaults(c);
	installListeners(c);
}
//-----------------------------------------------------------------------------
void FrxSelectionUI::draw(sd::IDrawContext::Ptr cn, sdc::AComponentPtr c) {
	sd::Rectangle r = sd::Rectangle(0,0, c->getWidth(), c->getHeight()); 
	selectedStyle.intoContext(cn);
	cn->rect(r);
	cn->fill();
	cn->rect(r);
	cn->stroke();
}

}}}} // namespace(s)
