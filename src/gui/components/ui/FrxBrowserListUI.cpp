/*
 * FrxBrowserListUI.cpp
 *
 *  Created on: Thu Nov 15 20:33:55 2012
 *      Author: Johannes Unger
 */

#include "FrxBrowserListUI.hpp"
#include <sambag/disco/components/ui/UIManager.hpp>

namespace frx { namespace gui { 
namespace components { namespace ui {
//=============================================================================
//  Class FrxBrowserListUI
//=============================================================================
//-----------------------------------------------------------------------------
void FrxBrowserListUI::installDefaults(sdc::AComponentPtr c) {
	sdc::ui::UIManager &m = sdc::ui::getUIManager();
	m.getProperty("FrxBrowserList.selectedEntryStyle", selectedStyle);
	ListType::Ptr list = 
		boost::shared_dynamic_cast<ListType>(c);
	SAMBAG_ASSERT(list);
	list->setSelectionBackground(
		selectedStyle.fillPattern()->getColor() 
	);
	list->setSelectionForeground(
		selectedStyle.strokePattern()->getColor()
	);
}
//-----------------------------------------------------------------------------
void FrxBrowserListUI::installListeners(sdc::AComponentPtr c) {
	c->sdc::EventSender<sdc::events::MouseEvent>::addTrackedEventListener(
		boost::bind(&FrxBrowserListUI::onMouse, this, _1, _2),
		self
	);
}
//-----------------------------------------------------------------------------
void FrxBrowserListUI::mouseDragged(const sdc::events::MouseEvent &ev) {
	sdc::AComponentPtr c = ev.getSource();
	if (c->getWidth() <= 0.)
		return;
	const sd::Point2D &loc = ev.getLocation();
	int index = locationToIndex(c, loc);
	sd::Coordinate value = loc.x();
	value /= c->getWidth();
	ListType::Ptr list = 
		boost::shared_dynamic_cast<ListType>(c);
	if (!list)
		return;
	const ListType::ValueType &node = list->get(index);
	if (!node.data.valueChanged)
		return;
	node.data.valueChanged(value);
}
//-----------------------------------------------------------------------------
void FrxBrowserListUI::onMouse(void *src, const sdc::events::MouseEvent &ev) {
	enum {
		Filter = sdc::events::MouseEvent::DISCO_MOUSE_DRAGGED
	};
	sdc::events::MouseEventSwitch<Filter>::delegate(ev, *this);
}
//-----------------------------------------------------------------------------
void FrxBrowserListUI::installUI(sdc::AComponentPtr c) {
	Super::installUI(c);
	installDefaults(c);
	installListeners(c);
}

}}}} // namespace(s)
