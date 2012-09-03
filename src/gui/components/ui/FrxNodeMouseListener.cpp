/*
 * FrxNodeMouseListener.cpp
 *
 *  Created on: Mon Aug 20 16:59:36 2012
 *      Author: Johannes Unger
 */

#include "FrxNodeMouseListener.hpp"
#include <gui/components/FrxComponent.hpp>
#include <gui/components/FrxCircuidView.hpp>
#include <sambag/disco/svg/units/Units.hpp>

namespace frx { namespace gui {
namespace components { namespace ui { 
//=============================================================================
//  Class FrxNodeMouseListener
//=============================================================================
//-----------------------------------------------------------------------------
FrxNodeMouseListener::FrxNodeMouseListener() {

}
//-----------------------------------------------------------------------------
void FrxNodeMouseListener::drag(const sdc::events::MouseEvent &ev) {
	namespace geom = boost::geometry;
	namespace trans = geom::strategy::transform;
	typedef trans::translate_transformer<sd::Point2D, sd::Point2D> Transl;
	sdc::AComponent::Ptr c = ev.getSource();
	FrxCircuidView::Ptr circ = c->getFirstContainer<FrxCircuidView>();
	SAMBAG_ASSERT(circ);
	// cacl transl.
	sd::Point2D distance = circ->getLocationOnComponent(ev.getLocationOnScreen());
	boost::geometry::subtract_point(distance, clickLoc);
	Transl transl(distance.x(), distance.y());
	// update
	sd::Point2D loc; 
	geom::transform(c->getLocation(), loc, transl);
	c->setLocation(loc);
	clickLoc = circ->getLocationOnComponent(ev.getLocationOnScreen());
}
//-----------------------------------------------------------------------------
void FrxNodeMouseListener::beginConnecting(const sdc::events::MouseEvent &ev) {
	sdc::AComponent::Ptr c = ev.getSource();
	FrxCircuidView::Ptr circ = c->getFirstContainer<FrxCircuidView>();
	SAMBAG_ASSERT(circ);
	if (!circ->containsComponent(toConnect))
		circ->add(toConnect, FrxCircuidView::Z_InteractiveStuff);
	// setline coord.
	FrxComponent::Ptr frxC = boost::shared_dynamic_cast<FrxComponent>(c);
	SAMBAG_ASSERT(frxC);
	sd::Point2D loc = frxC->getPivot();
	boost::geometry::add_point(loc, frxC->getLocation());
	sdsg::Line::Ptr line = toConnect->getObject();
	line->getP0().x().setValue(loc.x());
	line->getP0().y().setValue(loc.y());
	line->getP1().x().setValue(loc.x());
	line->getP1().y().setValue(loc.y());
	toConnect->setVisible(true);
}
//-----------------------------------------------------------------------------
void FrxNodeMouseListener::connecting(const sdc::events::MouseEvent &ev) {
	sdc::AComponent::Ptr c = ev.getSource();
	FrxCircuidView::Ptr circ = c->getFirstContainer<FrxCircuidView>();
	SAMBAG_ASSERT(circ);
	// setline coord.
	const sd::Point2D &loc = circ->getLocationOnComponent(ev.getLocationOnScreen());
	sdsg::Line::Ptr line = toConnect->getObject();
	line->getP1().x().setValue(loc.x());
	line->getP1().y().setValue(loc.y());
	toConnect->updateBounds();
	toConnect->redraw();
}
//-----------------------------------------------------------------------------
void FrxNodeMouseListener::endConnecting(const sdc::events::MouseEvent &ev) {
	sdc::AComponent::Ptr c = ev.getSource();
	FrxCircuidView::Ptr circ = c->getFirstContainer<FrxCircuidView>();
	SAMBAG_ASSERT(circ);
	toConnect->setVisible(false);
	circ->AComponent::redraw();
}
//-----------------------------------------------------------------------------
void FrxNodeMouseListener::mousePressed(const sdc::events::MouseEvent &ev) {
	sdc::AComponent::Ptr c = ev.getSource();
	FrxCircuidView::Ptr circ = c->getFirstContainer<FrxCircuidView>();
	SAMBAG_ASSERT(circ);
	clickLoc = circ->getLocationOnComponent(ev.getLocationOnScreen());
	if (ev.getButtons() == sdc::events::MouseEvent::DISCO_BTN1)
		beginConnecting(ev);
}
//-----------------------------------------------------------------------------
void FrxNodeMouseListener::mouseReleased(const sdc::events::MouseEvent &ev)  {
	clickLoc = ev.getLocation();
	if (ev.getButtons() == sdc::events::MouseEvent::DISCO_BTN1)
		endConnecting(ev);

}
//-----------------------------------------------------------------------------
void FrxNodeMouseListener::mouseEntered(const sdc::events::MouseEvent &ev)  {
}
//-----------------------------------------------------------------------------
void FrxNodeMouseListener::mouseExited(const sdc::events::MouseEvent &ev) {
}
//-----------------------------------------------------------------------------
void FrxNodeMouseListener::mouseClicked(const sdc::events::MouseEvent &ev) {
}
//-----------------------------------------------------------------------------
void FrxNodeMouseListener::mouseMoved(const sdc::events::MouseEvent &ev) {
}
//-----------------------------------------------------------------------------
void FrxNodeMouseListener::mouseDragged(const sdc::events::MouseEvent &ev) {
	if (ev.getButtons() == sdc::events::MouseEvent::DISCO_BTN1)
		use(ev);//connecting(ev);
	else 
		drag(ev);

}
//-----------------------------------------------------------------------------
void FrxNodeMouseListener::mouseWheelMoved(const sdc::events::MouseEvent &ev) {
}
//-----------------------------------------------------------------------------
void FrxNodeMouseListener::onMouse(void *src, const sdc::events::MouseEvent &ev) {
	sdc::events::MouseEventSwitch<>::
		delegate(ev, *this);
}
}}}} // namespace(s)
