/*
 * FrxHoverUI.cpp
 *
 *  Created on: Mon Oct  1 10:41:33 2012
 *      Author: Johannes Unger
 */

#include <boost/bind.hpp>
#include "FrxHoverUI.hpp"
#include <sambag/disco/components/AComponent.hpp>
#include <sambag/disco/components/ui/UIManager.hpp>
#include <sambag/disco/components/Window.hpp>

namespace frx { namespace gui {
namespace components { namespace ui { 
//=============================================================================
//  Class FrxHoverUI
//=============================================================================
//-----------------------------------------------------------------------------
void FrxHoverUI::installListeners(sdc::AComponentPtr c) {
	sdc::Window::Ptr top = c->getTopLevelAncestor();
	FrxHover::Ptr hover = boost::dynamic_pointer_cast<FrxHover>(c);
	SAMBAG_ASSERT(top && hover);
	FrxHoverMouseListener::Ptr mouseListener = FrxHoverMouseListener::create();
	mouseListener->setHover(hover);
	this->mouseListener = mouseListener;
	top->addTrackedWindowMouseEventListener(
		boost::bind(&FrxHoverMouseListener::onMouse, 
		mouseListener.get(), 
		_1, _2),
		getPtr()
	);
}
//-----------------------------------------------------------------------------
void FrxHoverUI::installDefaults(sdc::AComponentPtr c) {
	Super::installDefaults(c);
}
//-----------------------------------------------------------------------------
FrxHoverUI::Ptr FrxHoverUI::create() {
	Ptr res(new FrxHoverUI());
	res->self = res;
	return res;
}

}}}} // namespace(s)
