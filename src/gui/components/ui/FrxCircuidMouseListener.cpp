/*
 * FrxCircuidViewMouseListener.cpp
 *
 *  Created on: Mon Aug 27 16:30:23 2012
 *      Author: Johannes Unger
 */

#include "FrxCircuidMouseListener.hpp"

#include <gui/components/FrxComponent.hpp>
#include <gui/components/FrxCircuidView.hpp>
#include <gui/components/FrxSelection.hpp>
#include <gui/IFrxControl.hpp>
#include <sambag/disco/components/ui/UIManager.hpp>
#include <sambag/disco/svg/units/Units.hpp>
#include <sambag/disco/Dash.hpp>
#include <boost/assign.hpp>
#include <vector>

namespace frx { namespace gui {
namespace components { namespace ui { 
//=============================================================================
//  Class FrxCircuidMouseListener
//=============================================================================
//-----------------------------------------------------------------------------
FrxCircuidMouseListener::FrxCircuidMouseListener() {
	selection = Rect::create();
	sd::svg::units::Rectangle & rect = selection->getObject()->getRectangle();
	rect.x0().x().setType(sd::svg::units::Unit::PX);
	rect.x0().y().setType(sd::svg::units::Unit::PX);
	rect.size().width().setType(sd::svg::units::Unit::PX);
	rect.size().height().setType(sd::svg::units::Unit::PX);
	sdc::ui::UIManager &m = sdc::ui::getUIManager();
	sd::svg::graphicElements::Style selectingStyle;
	m.getProperty("FrxSelection.selectingStyle", selectingStyle);
	selection->setStyle(selectingStyle);
}
//-----------------------------------------------------------------------------
void FrxCircuidMouseListener::drag(const sdc::events::MouseEvent &ev) {
	namespace geom = boost::geometry;
	namespace trans = geom::strategy::transform;
	typedef trans::translate_transformer<sd::Point2D, sd::Point2D> Transl;
	sdc::AComponent::Ptr c = ev.getSource();
	FrxCircuidView::Ptr circ = c->getFirstContainer<FrxCircuidView>();
	SAMBAG_ASSERT(circ);
	sd::Point2D distance = ev.getLocationOnScreen();
	boost::geometry::subtract_point(distance, clickLoc);
	Transl transl(distance.x(), distance.y());

	sd::Point2D loc; 
	geom::transform(circ->getViewPosition(), loc, transl);
	circ->setViewPosition(loc);

	clickLoc = ev.getLocationOnScreen();
	
}
//-----------------------------------------------------------------------------
void FrxCircuidMouseListener::beginSpanning(const sdc::events::MouseEvent &ev) {
	sdc::AComponent::Ptr c = ev.getSource();
	FrxCircuidView::Ptr circ = c->getFirstContainer<FrxCircuidView>();
	SAMBAG_ASSERT(circ);
	if (!circ->containsComponent(selection))
		circ->add(selection, FrxCircuidView::Z_OnTop);
	// set rect coord.
	sd::svg::units::Rectangle & rect = selection->getObject()->getRectangle();
	rect.x0().x().setValue( clickLoc.x() );
	rect.x0().y().setValue( clickLoc.y() );
	rect.size().width().setValue(0);
	rect.size().height().setValue(0);
	selection->updateBounds();
	selection->setVisible(true);
	circ->getSelection()->clearContent();
}
//-----------------------------------------------------------------------------
void FrxCircuidMouseListener::spanning(const sdc::events::MouseEvent &ev) {
	sdc::AComponent::Ptr c = ev.getSource();
	FrxCircuidView::Ptr circ = c->getFirstContainer<FrxCircuidView>();
	SAMBAG_ASSERT(circ);
	// setline coord.
	const sd::Point2D &loc = ev.getLocation();
	sd::svg::units::Rectangle & rect = selection->getObject()->getRectangle();
	rect.size().width().setValue(loc.x() - clickLoc.x());
	rect.size().height().setValue(loc.y() - clickLoc.y());
	selection->updateBounds();
	selection->redraw();
}
//-----------------------------------------------------------------------------
void FrxCircuidMouseListener::endSpanning(const sdc::events::MouseEvent &ev) {
	sdc::AComponent::Ptr c = ev.getSource();
	FrxCircuidView::Ptr circ = c->getFirstContainer<FrxCircuidView>();
	SAMBAG_ASSERT(circ);
	selection->setVisible(false);
	circ->AComponent::redraw();

	FrxSelection::ContentContainer content;
	circ->findComponentsInArea(content, selection->getBounds(), 2., 4.);
	circ->getSelection()->setContent(content);
}
//-----------------------------------------------------------------------------
void FrxCircuidMouseListener::mousePressed(const sdc::events::MouseEvent &ev) {
	clickLoc = ev.getLocationOnScreen();
	if (ev.getButtons() == sdc::events::MouseEvent::DISCO_BTN1)
		beginSpanning(ev);
}
//-----------------------------------------------------------------------------
void FrxCircuidMouseListener::mouseReleased(const sdc::events::MouseEvent &ev)  {
	if (ev.getButtons() == sdc::events::MouseEvent::DISCO_BTN1)
		endSpanning(ev);

}
//-----------------------------------------------------------------------------
void FrxCircuidMouseListener::mouseEntered(const sdc::events::MouseEvent &ev)  {
}
//-----------------------------------------------------------------------------
void FrxCircuidMouseListener::mouseExited(const sdc::events::MouseEvent &ev) {
}
//-----------------------------------------------------------------------------
void FrxCircuidMouseListener::mouseClicked(const sdc::events::MouseEvent &ev) {
	sdc::AComponent::Ptr c = ev.getSource();
	FrxCircuidView::Ptr circ = c->getFirstContainer<FrxCircuidView>();
	SAMBAG_ASSERT(circ);
	getFrxControl(circ).handleContextMenuPopup(ev);
}
//-----------------------------------------------------------------------------
void FrxCircuidMouseListener::mouseMoved(const sdc::events::MouseEvent &ev) {
}
//-----------------------------------------------------------------------------
void FrxCircuidMouseListener::mouseDragged(const sdc::events::MouseEvent &ev) {
	if (ev.getButtons() == sdc::events::MouseEvent::DISCO_BTN1)
		spanning(ev);
	else 
		drag(ev);

}
//-----------------------------------------------------------------------------
void FrxCircuidMouseListener::mouseWheelMoved(const sdc::events::MouseEvent &ev) {
}
//-----------------------------------------------------------------------------
void FrxCircuidMouseListener::onMouse(void *src, const sdc::events::MouseEvent &ev) {
	sdc::events::MouseEventSwitch<>::
		delegate(ev, *this);
}
}}}} // namespace(s)