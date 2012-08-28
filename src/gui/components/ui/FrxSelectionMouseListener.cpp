/*
 * FrxSelectionMouseListener.cpp
 *
 *  Created on: Tue Aug 28 13:22:07 2012
 *      Author: Johannes Unger
 */

#include "FrxSelectionMouseListener.hpp"
#include <gui/components/FrxCircuidView.hpp>
#include <gui/components/FrxComponent.hpp>
#include <gui/components/FrxSelection.hpp>

namespace frx { namespace gui {
namespace components { namespace ui { 
//=============================================================================
//  Class FrxSelectionMouseListener
//=============================================================================
//-----------------------------------------------------------------------------
void FrxSelectionMouseListener::drag(const sdc::events::MouseEvent &ev) {
	namespace geom = boost::geometry;
	namespace trans = geom::strategy::transform;
	typedef trans::translate_transformer<sd::Point2D, sd::Point2D> Transl;
	sdc::AComponent::Ptr c = ev.getSource();
	FrxSelection::Ptr sel = boost::shared_dynamic_cast<FrxSelection>(c);
	FrxCircuidView::Ptr circ = sel->getFirstContainer<FrxCircuidView>();
	SAMBAG_ASSERT(circ);
	sd::Point2D distance = circ->getLocationOnComponent(ev.getLocationOnScreen());
	boost::geometry::subtract_point(distance, clickLoc);
	Transl transl(distance.x(), distance.y());
	BOOST_FOREACH(sdc::AComponent::WPtr _sc, sel->getContent()) {
		sdc::AComponent::Ptr sc = _sc.lock();
		sd::Point2D loc; 
		geom::transform(sc->getLocation(), loc, transl);
		sc->setLocation(loc);
	}
	// update sel:
	sd::Point2D loc; 
	geom::transform(sel->getLocation(), loc, transl);
	sel->setLocation(loc);

	clickLoc = circ->getLocationOnComponent(ev.getLocationOnScreen());
}
//-----------------------------------------------------------------------------
void FrxSelectionMouseListener::mousePressed(const sdc::events::MouseEvent &ev) {
	sdc::AComponent::Ptr c = ev.getSource();
	FrxCircuidView::Ptr circ = c->getFirstContainer<FrxCircuidView>();
	SAMBAG_ASSERT(circ);
	clickLoc = circ->getLocationOnComponent(ev.getLocationOnScreen());
}
//-----------------------------------------------------------------------------
void FrxSelectionMouseListener::mouseDragged(const sdc::events::MouseEvent &ev) {
	if (ev.getButtons() == sdc::events::MouseEvent::DISCO_BTN2)
		drag(ev);
}
//-----------------------------------------------------------------------------
void FrxSelectionMouseListener::onMouse(void *src, const sdc::events::MouseEvent &ev)
{
	using namespace sdc::events;
	enum { Filter = 
		MouseEvent::DISCO_MOUSE_PRESSED |
		MouseEvent::DISCO_MOUSE_DRAGGED
	};
	sdc::events::MouseEventSwitch<Filter>::
		delegate(ev, *this);
}
}}}} // namespace(s)
