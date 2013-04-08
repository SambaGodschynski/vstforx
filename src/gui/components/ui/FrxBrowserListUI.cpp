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
		selectedStyle.fillPattern() 
	);
	list->setSelectionForeground(
		selectedStyle.strokePattern()
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
void FrxBrowserListUI::mousePressed(const sdc::events::MouseEvent &ev) {
	const sd::Point2D &loc = ev.getLocation();
	sdc::AComponentPtr c = ev.getSource();
	currentIndex = locationToIndex(c, loc);
}
//-----------------------------------------------------------------------------
void FrxBrowserListUI::mouseReleased(const sdc::events::MouseEvent &ev) {
	currentIndex = -1;
}
//-----------------------------------------------------------------------------
void FrxBrowserListUI::mouseDragged(const sdc::events::MouseEvent &ev) {
	if (currentIndex<0) {
		return;
	}
	sdc::AComponentPtr c = ev.getSource();
	const sd::Point2D &loc = ev.getLocation();
	if (c->getWidth() <= 0.)
		return;
	double value = (double)loc.x();
	value /= c->getWidth();
	value = std::min( 1.0, std::max( 0., value ) );
	ListType::Ptr list = 
		boost::shared_dynamic_cast<ListType>(c);
	if (!list)
		return;
	const ListType::ValueType &node = list->get(currentIndex);
	if (!node.data.valueChanged)
		return;
	node.data.valueChanged((float)value);
}
//-----------------------------------------------------------------------------
void FrxBrowserListUI::onMouse(void *src, const sdc::events::MouseEvent &ev) {
	enum {
		Filter = sdc::events::MouseEvent::DISCO_MOUSE_DRAGGED | 
			sdc::events::MouseEvent::DISCO_MOUSE_PRESSED |
			sdc::events::MouseEvent::DISCO_MOUSE_RELEASED
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
