/*
 * FrxCircuidViewUI.cpp
 *
 *  Created on: Mon Aug 20 12:13:07 2012
 *      Author: Johannes Unger
 */

#include <boost/bind.hpp>
#include "FrxCircuidViewUI.hpp"
#include <gui/components/FrxCircuidView.hpp>
#include <gui/IFrxControl.hpp>
#include "TooltipTexts.hpp"


namespace frx { namespace gui {
namespace components { namespace ui { 
//=============================================================================
//  Class FrxCircuidViewUI
//=============================================================================
//-----------------------------------------------------------------------------
FrxCircuidViewUI::FrxCircuidViewUI() {
}
//-----------------------------------------------------------------------------
FrxCircuidViewUI::Ptr FrxCircuidViewUI::create() {
	Ptr res(new FrxCircuidViewUI());
	res->self = res;
	return res;
}
//-----------------------------------------------------------------------------
void FrxCircuidViewUI::installUI(sdc::AComponentPtr c) {
	FrxCircuidView::Ptr circ = std::dynamic_pointer_cast<FrxCircuidView>(c);
	sdc::AContainer::Ptr cont = circ->getContentPane();
	SAMBAG_ASSERT(cont);
	cont->sdc::EventSender<sdc::events::MouseEvent>::addTrackedEventListener(
		boost::bind(&FrxCircuidMouseListener::onMouse, &mouseListener, _1, _2),
		getPtr()
	);
	circ->getContentPane()->setTooltipText(
		TooltipTexts::getText(*(c.get()))
	);
}/*
//-----------------------------------------------------------------------------
void FrxCircuidViewUI::draw(sd::IDrawContext::Ptr cn, sdc::AComponentPtr c) {
	cn->setFillColor(sd::ColorRGBA(1));
	sd::Coordinate w = c->getWidth();
	sd::Coordinate h = c->getHeight();
	cn->rect(sd::Rectangle(0,0,w,h));
	cn->fill();
}*/
}}}} // namespace(s)
