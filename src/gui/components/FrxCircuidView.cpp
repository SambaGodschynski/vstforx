/*
 * FrxCircuidView.cpp
 *
 *  Created on: Mon Aug 20 12:12:59 2012
 *      Author: Johannes Unger
 */

#include "FrxCircuidView.hpp"
#include <sambag/disco/components/ui/ALookAndFeel.hpp>
#include <sambag/disco/components/MenuSelectionManager.hpp>
#include <sambag/disco/components/PopupMenu.hpp>
#include "FrxControl.hpp"

namespace frx { namespace gui { namespace components {
//=============================================================================
//  Class FrxCircuidView
//=============================================================================
//-----------------------------------------------------------------------------
const std::string FrxCircuidView::PROPERTY_ZORDER = "z_order";
//-----------------------------------------------------------------------------
const float FrxCircuidView::Z_Wires = 5.f;
//-----------------------------------------------------------------------------
const float FrxCircuidView::Z_ProcessorNodes = 4.f;
//-----------------------------------------------------------------------------
const float FrxCircuidView::Z_IO = 4.f;
//-----------------------------------------------------------------------------
const float FrxCircuidView::Z_Knobs = 3.f;
//-----------------------------------------------------------------------------
const float FrxCircuidView::Z_OnTop = 1.f;
//-----------------------------------------------------------------------------
const float FrxCircuidView::Z_Default = FrxCircuidView::Z_OnTop;
//-----------------------------------------------------------------------------
const float FrxCircuidView::Z_InteractiveStuff = Z_OnTop;
//-----------------------------------------------------------------------------
void FrxCircuidView::add(sdc::AComponentPtr comp, ZOrder zord) {
	// order on insert:
	for (size_t i = 0; i<content->getComponentCount(); ++i) {
		AComponent::Ptr c = content->getComponent(i);
		ZOrder z = FLT_MAX;
		c->getClientProperty(PROPERTY_ZORDER, z);
		if (zord < z ) {
			comp->putClientProperty(PROPERTY_ZORDER, zord);
			content->add(comp, i);
			return;
		}
	}
	comp->putClientProperty(PROPERTY_ZORDER, zord);
	content->add(comp);
	// add listener
	comp->EventSender<sdc::events::MouseEvent>::addTrackedEventListener (
		boost::bind(&FrxCircuidView::onMouse, this, _1, _2),
		getPtr()
	);
}
//-----------------------------------------------------------------------------
FrxCircuidView::FrxCircuidView() {
	setName("FrxCircuidView");
	//selection = FrxSelection::create();
	//add(selection, Z_InteractiveStuff); // !parent <= !!
}
//-----------------------------------------------------------------------------
void FrxCircuidView::remove(sdc::AComponentPtr comp) {
	content->remove(comp);
}
//-----------------------------------------------------------------------------
sdcu::AComponentUIPtr FrxCircuidView::createComponentUI(sdcu::ALookAndFeelPtr laf) const
{
	return laf->getUI<FrxCircuidView>();
}
//-----------------------------------------------------------------------------
void FrxCircuidView::postConstructor() {
	content = sdc::Panel::create();
	content->setSize(sd::Dimension(10000, 10000));
	content->EventSender<sdc::events::MouseEvent>::addTrackedEventListener (
		boost::bind(&FrxCircuidView::onMouse, this, _1, _2),
		getPtr()
	);
	Super::add(content);
	content->setLayout(sdc::ALayoutManagerPtr());
	selection = FrxSelection::create();
	add(selection, Z_InteractiveStuff);
	setComponentPopupMenu(getFrxControl().getCircuidViewPopup(getPtr()));
}
//-----------------------------------------------------------------------------
void FrxCircuidView::handleMousePopup(const sdc::events::MouseEvent &ev) {
	using namespace sambag::disco::components;
	if (ev.getButtons() != sdc::events::MouseEvent::DISCO_BTN2) {
		if (currPopup)
			currPopup->hidePopup();
		return;
	}
	sdc::PopupMenuPtr popup = ev.getSource()->getComponentPopupMenu();
	if (!popup)
		return;
	if (!popup->isPopupVisible()) {
		MenuSelectionManager &m = MenuSelectionManager::defaultManager();
		m.clearSelectedPath();
		IMenuElement::MenuElements p;
		p.push_back(popup);
		m.setSelectedPath(p);
		popup->setInvoker(ev.getSource());
		popup->showPopup(
			ev.getLocationOnScreen()
		);
	}
	currPopup = popup;
}
//-----------------------------------------------------------------------------
void FrxCircuidView::onMouse(void *src, const sdc::events::MouseEvent &ev) {
	if (ev.getType() != sdc::events::MouseEvent::DISCO_MOUSE_CLICKED) 
		return;
	handleMousePopup(ev);
}
}}} // namespace(s)
