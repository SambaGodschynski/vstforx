/*
 * FrxCircuidViewUI.cpp
 *
 *  Created on: Mon Aug 20 12:13:07 2012
 *      Author: Johannes Unger
 */

#include "FrxCircuidViewUI.hpp"
#include <gui/components/FrxCircuidView.hpp>
#include <gui/IFrxControl.hpp>

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
	FrxCircuidView::Ptr circ = boost::shared_dynamic_cast<FrxCircuidView>(c);
	sdc::AContainer::Ptr cont = circ->getContentPane();
	SAMBAG_ASSERT(cont);
	cont->EventSender<sdc::events::MouseEvent>::addTrackedEventListener(
		boost::bind(&FrxCircuidMouseListener::onMouse, &mouseListener, _1, _2),
		getPtr()
	);
	circ->setComponentPopupMenu(getFrxControl(circ).getCircuidViewPopup(circ));
}
}}}} // namespace(s)
