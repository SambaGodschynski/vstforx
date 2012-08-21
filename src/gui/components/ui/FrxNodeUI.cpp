/*
 * FrxNodeUI.cpp
 *
 *  Created on: Mon Aug 20 10:46:25 2012
 *      Author: Johannes Unger
 */

#include "FrxNodeUI.hpp"
#include <sambag/disco/components/AComponent.hpp>
#include <sambag/disco/components/events/MouseEvent.hpp>

namespace frx { namespace gui {
namespace components { namespace ui { 
//=============================================================================
//  Class FrxNodeUI
//=============================================================================
//-----------------------------------------------------------------------------
void FrxNodeUI::installUI(sdc::AComponentPtr c) {
	c->EventSender<sdc::events::MouseEvent>::addTrackedEventListener(
		boost::bind(&FrxNodeMouseListener::onMouse, &mouseListener, _1, _2),
		getPtr()
	);
}
}}}} // namespace(s)
