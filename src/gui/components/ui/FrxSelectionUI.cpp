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
	c->EventSender<sdc::events::MouseEvent>::addTrackedEventListener (
		boost::bind(&FrxSelectionMouseListener::onMouse, mouseListener.get(), _1, _2),
		getPtr()
	);
}
//-----------------------------------------------------------------------------
void FrxSelectionUI::installDefaults(sdc::AComponentPtr c) {
	sd::ColorRGBA col(1., .25, .5, .25);
	sdc::ui::getUIManager().getProperty("FrxSelection.bgColor", col);
	c->setBackground(col);
	sdc::ui::getUIManager().getProperty("FrxSelection.fgColor", col);
	c->setForeground(col);
}
//-----------------------------------------------------------------------------
void FrxSelectionUI::installUI(sdc::AComponentPtr c) {
	installDefaults(c);
	installListeners(c);
}
//-----------------------------------------------------------------------------
void FrxSelectionUI::draw(sd::IDrawContext::Ptr cn, sdc::AComponentPtr c) {
	cn->rect(sd::Rectangle(0,0, c->getWidth(), c->getHeight()));
	cn->setFillColor(c->getBackground());
	cn->fill();
}

}}}} // namespace(s)
