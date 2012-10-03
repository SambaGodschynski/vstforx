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
#include <gui/components/FrxControl.hpp>
#include <gui/components/FrxNode.hpp>
namespace frx { namespace gui {
namespace components { namespace ui { 
///////////////////////////////////////////////////////////////////////////////
namespace {
	const sambag::com::Number FINAL_ALPHA = 0.5f;
	const int FADE_STEPS = 10;
	void onFadeTimer( void *src, const sdc::Timer::Event &ev,
		sambag::com::Number *alpha,
		sdc::AComponent::WPtr _c, 
		const bool *fadeIn
	) {
	sdc::AComponent::Ptr c = _c.lock();
		if (!c)
			return;
		if (*fadeIn) {
			if (*alpha>=FINAL_ALPHA) {
				ev.getSource()->stop();
				return;
			}
			*alpha+=FINAL_ALPHA/FADE_STEPS;
		}
		else {
			if (*alpha<=0.) {
				ev.getSource()->stop();
				return;
			}
			*alpha-=FINAL_ALPHA/FADE_STEPS;
		}
		c->getParent()->redraw();
	}
}
//=============================================================================
//  Class FrxNodeUI
//=============================================================================
//-----------------------------------------------------------------------------
bool FrxNodeUI::contains(sdc::AComponentPtr c, const sd::Point2D &p) {
	if (inside)
		return hitsCorona(c, p);
	else
		return hitsCore(c, p);
}
//-----------------------------------------------------------------------------
bool FrxNodeUI::hitsCorona(sdc::AComponentPtr c, const sd::Point2D &p) const {
	FrxComponent::Ptr node = boost::shared_dynamic_cast<FrxComponent>(c);
	sd::Coordinate x = p.x() - node->getPivot().x();
	sd::Coordinate y = p.y() - node->getPivot().y();
	sd::Coordinate radius = getCoronaRadius(c);
	return x*x + y*y <= radius*radius;
}
//-----------------------------------------------------------------------------
bool FrxNodeUI::hitsCore(sdc::AComponentPtr c, const sd::Point2D &p) const {
	FrxComponent::Ptr node = boost::shared_dynamic_cast<FrxComponent>(c);
	sd::Coordinate x = p.x() - node->getPivot().x();
	sd::Coordinate y = p.y() - node->getPivot().y();
	sd::Coordinate radius = getCoreRadius(c);
	return x*x + y*y <= radius*radius;
}
//------------------------------------------------------------------------------
void FrxNodeUI::installListeners(sdc::AComponent::Ptr c) {
	// listeners
	c->EventSender<sdc::events::MouseEvent>::addTrackedEventListener(
		boost::bind(&FrxNodeUI::onMouse, this, _1, _2),
		getPtr()
	);
}
//------------------------------------------------------------------------------
void FrxNodeUI::installDefaults(sdc::AComponent::Ptr c) {
	// size
	sd::Coordinate r = getCoronaRadius(c) * 2. + 5.;
	c->setSize(sd::Dimension(r, r));

	coronaAlpha = 0.;
	// connection stuff
	toConnect = Line::create();
	sdsg::Line::Ptr line = toConnect->getObject();
	line->getP0().x().setType(sd::svg::units::Unit::PX);
	line->getP0().y().setType(sd::svg::units::Unit::PX);
	line->getP1().x().setType(sd::svg::units::Unit::PX);
	line->getP1().y().setType(sd::svg::units::Unit::PX);
	toConnect->setForeground(sd::ColorRGBA(1));
	// fade timer
	fadeTimer = sdc::Timer::create(10);
	fadeTimer->setNumRepetitions(-1);
	fadeIn = true;
	sdc::AComponent::WPtr _c = c;
	fadeTimer->EventSender<sdc::Timer::Event>::addTrackedEventListener(
		boost::bind(
			&onFadeTimer, _1, _2, &coronaAlpha, _c, &fadeIn
		),
		getPtr()
	);
}
//-----------------------------------------------------------------------------
void FrxNodeUI::installUI(sdc::AComponentPtr c) {
	installDefaults(c);
	installListeners(c);
}
//-----------------------------------------------------------------------------
void FrxNodeUI::drawCorona(sd::IDrawContext::Ptr cn, sdc::AComponentPtr c) {
	SAMBAG_PROPERTY_TAG(PropertyTag, "FrxNodeCorona.color");
	sd::ColorRGBA coronaCol = 
		sdcu::getUIPropertyCached<PropertyTag>(sd::ColorRGBA());
	coronaCol.setA(coronaAlpha);
	FrxComponent::Ptr node = boost::shared_dynamic_cast<FrxComponent>(c);
	sd::Point2D loc = node->getPivot();
	cn->arc(loc, getCoronaRadius(c));
	cn->setFillColor(coronaCol);
	cn->fill();
}
//-----------------------------------------------------------------------------
void FrxNodeUI::draw(sd::IDrawContext::Ptr cn, sdc::AComponentPtr c) {
	if (!usesCorona() || coronaAlpha == 0.)
		return;
	drawCorona(cn, c);
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
	FrxNodePtr from = boost::shared_dynamic_cast<FrxNode>(c);
	sd::Point2D loc = 
		circ->getLocationOnComponent(ev.getLocationOnScreen());
	FrxNodePtr to = boost::shared_dynamic_cast<FrxNode>(
		circ->findComponentOnPoint(loc, FrxCircuidView::Z_Knobs, 
		FrxCircuidView::Z_ProcessorNodes)
	);
	if (!from || !to) {
		return;
	}
	getFrxControl(circ).connect(circ, from, to);
	circ->AComponent::redraw();
}
//------------------------------------------------------------------------------
int FrxNodeUI::determineContext(const sdc::events::MouseEvent &ev) const {
	if (hitsCore(ev.getSource(), ev.getLocation())) {
		return CONNECT;
	}
	if (hitsCorona(ev.getSource(), ev.getLocation())) {
		return DRAG;
	}
	return NONE;
}
//-----------------------------------------------------------------------------
void FrxNodeUI::mousePressed(const sdc::events::MouseEvent &ev) {
	sdc::AComponent::Ptr c = ev.getSource();
	FrxCircuidView::Ptr circ = c->getFirstContainer<FrxCircuidView>();
	SAMBAG_ASSERT(circ);
	clickLoc = circ->getLocationOnComponent(ev.getLocationOnScreen());
	if (ev.getButtons() != sdc::events::MouseEvent::DISCO_BTN1)
		return;
	context = determineContext(ev);
	if (context == CONNECT) {
		beginConnecting(ev);
		return;
	}
}
//-----------------------------------------------------------------------------
void FrxNodeUI::mouseReleased(const sdc::events::MouseEvent &ev)  {
	if (context == CONNECT)
		endConnecting(ev);
	context = NONE;
}
//-----------------------------------------------------------------------------
void FrxNodeUI::mouseEntered(const sdc::events::MouseEvent &ev)  {
	if (inside)
		return;
	fadeIn = true;
	fadeTimer->start();
	inside = true;
}
//-----------------------------------------------------------------------------
void FrxNodeUI::mouseExited(const sdc::events::MouseEvent &ev) {
	if (!inside)
		return;
	fadeIn = false;
	fadeTimer->start();
	inside = false;
}
//-----------------------------------------------------------------------------
void FrxNodeUI::mouseClicked(const sdc::events::MouseEvent &ev) {
	sdc::AComponent::Ptr c = ev.getSource();
	FrxCircuidView::Ptr circ = c->getFirstContainer<FrxCircuidView>();
	SAMBAG_ASSERT(circ);
	getFrxControl(circ).handleContextMenuPopup(ev);
}
//-----------------------------------------------------------------------------
void FrxNodeUI::mouseMoved(const sdc::events::MouseEvent &ev) {
}
//-----------------------------------------------------------------------------
void FrxNodeUI::mouseDragged(const sdc::events::MouseEvent &ev) {
	if (context==DRAG) {
		drag(ev);
		return;
	}
	if (context==CONNECT) {
		connecting(ev);
		return;
	}
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
