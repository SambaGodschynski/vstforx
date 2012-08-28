/*
 * FrxSelectionUI.cpp
 *
 *  Created on: Tue Aug 28 10:47:37 2012
 *      Author: Johannes Unger
 */

#include "FrxSelectionUI.hpp"
#include <sambag/disco/components/AComponent.hpp>

namespace frx { namespace gui {
namespace components { namespace ui { 
//=============================================================================
//  Class FrxSelectionUI
//=============================================================================
//-----------------------------------------------------------------------------
void FrxSelectionUI::installUI(sdc::AComponentPtr c) {
	c->EventSender<sdc::events::MouseEvent>::addTrackedEventListener (
		boost::bind(&FrxSelectionMouseListener::onMouse, &mouseListener, _1, _2),
		getPtr()
	);
}
//-----------------------------------------------------------------------------
void FrxSelectionUI::draw(sd::IDrawContext::Ptr cn, sdc::AComponentPtr c) {
	cn->rect(sd::Rectangle(0,0, c->getWidth(), c->getHeight()));
	cn->setFillColor(sd::ColorRGBA(1., .25, .5, .25));
	cn->fill();
}

}}}} // namespace(s)
