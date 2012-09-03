/*
 * FrxNodeUI.cpp
 *
 *  Created on: Mon Aug 20 10:46:25 2012
 *      Author: Johannes Unger
 */

#include "FrxNodeUI.hpp"
#include <sambag/disco/components/AComponent.hpp>
#include <sambag/disco/components/events/MouseEvent.hpp>
#include <sambag/disco/components/ui/UIManager.hpp>
#include <gui/components/FrxCircuidView.hpp>

namespace frx { namespace gui {
namespace components { namespace ui { 
///////////////////////////////////////////////////////////////////////////////
namespace {
	const sambag::com::Number FINAL_ALPHA = 0.5f;
	const int FADE_IN_STEPS = 10;
	const int FADE_OUT_STEPS = 10;
	const sambag::com::Number FADE_IN_INCR = 
		FINAL_ALPHA / (sambag::com::Number)FADE_IN_STEPS;
	const sambag::com::Number FADE_OUT_DECR = 
		FINAL_ALPHA / (sambag::com::Number)FADE_IN_STEPS;
	void onFadeTimer( void *src, const sdc::Timer::Event &ev,
		sd::ColorRGBA *col,
		sdc::AComponent::WPtr _c, 
		const bool *fadeIn
	) {
	sdc::AComponent::Ptr c = _c.lock();
		if (!c)
			return;
		sambag::com::Number a = col->getA();
		if (*fadeIn) {
			a+=FADE_IN_INCR;
			if (a>=FINAL_ALPHA)
				ev.getSource()->stop();
		}
		else {
			a-=FADE_OUT_DECR;
			if (a<=0.)
				ev.getSource()->stop();
		}
		col->setA(a);
		c->redraw();
	}
}
//=============================================================================
//  Class FrxNodeUI
//=============================================================================
//-----------------------------------------------------------------------------
void FrxNodeUI::installUI(sdc::AComponentPtr c) {
	// size
	sd::Coordinate r = getCoronaRadius(c) * 2. + 5.;
	c->setSize(sd::Dimension(r, r));
	// listeners
	c->EventSender<sdc::events::MouseEvent>::addTrackedEventListener(
		boost::bind(&FrxNodeUI::onMouse, this, _1, _2),
		getPtr()
	);
	sdcu::getUIManager().getProperty("FrxNodeCorona.color", coronaCol);
	coronaCol.setA(0.0);
	// connection stuff
	toConnect = Line::create();
	sdsg::Line::Ptr line = toConnect->getObject();
	line->getP0().x().setType(sd::svg::units::Unit::PX);
	line->getP0().y().setType(sd::svg::units::Unit::PX);
	line->getP1().x().setType(sd::svg::units::Unit::PX);
	line->getP1().y().setType(sd::svg::units::Unit::PX);
	toConnect->setForeground(sd::ColorRGBA(1));
	// fade timer
	fadeTimer = sdc::Timer::create(1);
	fadeTimer->setNumRepetitions(-1);
	fadeIn = true;
	sdc::AComponent::WPtr _c = c;
	fadeTimer->EventSender<sdc::Timer::Event>::addTrackedEventListener(
		boost::bind(
			&onFadeTimer, _1, _2, &coronaCol, _c, &fadeIn
		),
		getPtr()
	);
}
//-----------------------------------------------------------------------------
void FrxNodeUI::draw(sd::IDrawContext::Ptr cn, sdc::AComponentPtr c) {
	if (!usesCorona() || coronaCol.getA() == 0.)
		return;
	FrxComponent::Ptr node = boost::shared_dynamic_cast<FrxComponent>(c);
	sd::Point2D loc = node->getPivot();
	cn->arc(loc, getCoronaRadius(c));
	cn->setFillColor(coronaCol);
	cn->fill();
}
//-----------------------------------------------------------------------------
void FrxNodeUI::drag(const sdc::events::MouseEvent &ev) {
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
void FrxNodeUI::beginConnecting(const sdc::events::MouseEvent &ev) {
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
void FrxNodeUI::connecting(const sdc::events::MouseEvent &ev) {
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
void FrxNodeUI::endConnecting(const sdc::events::MouseEvent &ev) {
	sdc::AComponent::Ptr c = ev.getSource();
	FrxCircuidView::Ptr circ = c->getFirstContainer<FrxCircuidView>();
	SAMBAG_ASSERT(circ);
	toConnect->setVisible(false);
	circ->AComponent::redraw();
}
//-----------------------------------------------------------------------------
void FrxNodeUI::mousePressed(const sdc::events::MouseEvent &ev) {
	sdc::AComponent::Ptr c = ev.getSource();
	FrxCircuidView::Ptr circ = c->getFirstContainer<FrxCircuidView>();
	SAMBAG_ASSERT(circ);
	clickLoc = circ->getLocationOnComponent(ev.getLocationOnScreen());
	if (ev.getButtons() == sdc::events::MouseEvent::DISCO_BTN1)
		beginConnecting(ev);
}
//-----------------------------------------------------------------------------
void FrxNodeUI::mouseReleased(const sdc::events::MouseEvent &ev)  {
	clickLoc = ev.getLocation();
	if (ev.getButtons() == sdc::events::MouseEvent::DISCO_BTN1)
		endConnecting(ev);

}
//-----------------------------------------------------------------------------
void FrxNodeUI::mouseEntered(const sdc::events::MouseEvent &ev)  {
	fadeIn = true;
	fadeTimer->start();
}
//-----------------------------------------------------------------------------
void FrxNodeUI::mouseExited(const sdc::events::MouseEvent &ev) {
	fadeIn = false;
	fadeTimer->start();
}
//-----------------------------------------------------------------------------
void FrxNodeUI::mouseClicked(const sdc::events::MouseEvent &ev) {
}
//-----------------------------------------------------------------------------
void FrxNodeUI::mouseMoved(const sdc::events::MouseEvent &ev) {
}
//-----------------------------------------------------------------------------
void FrxNodeUI::mouseDragged(const sdc::events::MouseEvent &ev) {
	if (ev.getButtons() == sdc::events::MouseEvent::DISCO_BTN1)
		use(ev);//connecting(ev);
	else 
		drag(ev);

}
//-----------------------------------------------------------------------------
void FrxNodeUI::mouseWheelMoved(const sdc::events::MouseEvent &ev) {
}
//-----------------------------------------------------------------------------
void FrxNodeUI::onMouse(void *src, const sdc::events::MouseEvent &ev) {
	sdc::events::MouseEventSwitch<>::
		delegate(ev, *this);
}
}}}} // namespace(s)
