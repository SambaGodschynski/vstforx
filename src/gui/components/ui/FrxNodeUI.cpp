/*
 * FrxNodeUI.cpp
 *
 *  Created on: Mon Aug 20 10:46:25 2012
 *      Author: Johannes Unger
 */

#include "FrxNodeUI.hpp"
#include <gui/components/FrxConcreteProcessor.hpp>
#include <gui/components/FrxConcreteParameter.hpp>
#include <gui/components/FrxConcreteIO.hpp>
#include <sambag/disco/components/AContainer.hpp>
#include <sambag/disco/components/events/MouseEvent.hpp>
#include <sambag/disco/components/ui/UIManager.hpp>
#include <gui/components/FrxCircuidView.hpp>
#include <gui/IFrxControl.hpp>
#include <gui/components/FrxNode.hpp>
#include <loki/MultiMethods.h>

namespace frx { namespace gui {
namespace components { namespace ui { 
///////////////////////////////////////////////////////////////////////////////
namespace { 
	const double FINAL_ALPHA = .5f;
} // namespace(s)
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
	FrxComponent::Ptr node = boost::dynamic_pointer_cast<FrxComponent>(c);
	sd::Coordinate x = p.x() - node->getPivot().x();
	sd::Coordinate y = p.y() - node->getPivot().y();
	sd::Coordinate radius = getCoronaRadius(c);
	return x*x + y*y <= radius*radius;
}
//-----------------------------------------------------------------------------
bool FrxNodeUI::hitsCore(sdc::AComponentPtr c, const sd::Point2D &p) const {
	FrxComponent::Ptr node = boost::dynamic_pointer_cast<FrxComponent>(c);
	sd::Coordinate x = p.x() - node->getPivot().x();
	sd::Coordinate y = p.y() - node->getPivot().y();
	sd::Coordinate radius = getCoreRadius(c);
	return x*x + y*y <= radius*radius;
}
//------------------------------------------------------------------------------
void FrxNodeUI::installListeners(sdc::AComponent::Ptr c) {
	// listeners
	c->sdc::EventSender<sdc::events::MouseEvent>::addTrackedEventListener(
		boost::bind(&FrxNodeUI::onMouse, this, _1, _2),
		getPtr()
	);
	sdc::AContainer::Ptr cont = 
		boost::dynamic_pointer_cast<sdc::AContainer>(c);
	if (!cont) {
		return;
	}
	/*BOOST_FOREACH(sdc::AComponentPtr cc, cont->getComponents()) {
		cc->sdc::EventSender<sdc::events::MouseEvent>::addTrackedEventListener(
			boost::bind(&FrxNodeUI::onChildComponentMouse, this, _1, _2),
			getPtr()
		);
	}*/
}
//------------------------------------------------------------------------------
void FrxNodeUI::installDefaults(sdc::AComponent::Ptr c) {
	Super::installDefaults(c);
	// size
	sd::Coordinate r = getCoronaRadius(c) * 2. + 5.;
	c->setSize(sd::Dimension(r, r));

	// connection stuff
	toConnect = Line::create();
	toConnect->setName("connector");
	sdsg::Line::Ptr line = toConnect->getObject();
	line->getP0().x().setType(sd::svg::units::Unit::PX);
	line->getP0().y().setType(sd::svg::units::Unit::PX);
	line->getP1().x().setType(sd::svg::units::Unit::PX);
	line->getP1().y().setType(sd::svg::units::Unit::PX);
	toConnect->setForeground(sd::ColorRGBA(1));
	// fade timer
	sdcu::UIManager &uim = sdcu::getUIManager();
	long duration = 150, rfRate = 15;
	std::string tweenType = "lin";
	
	uim.getProperty("FrxNodeCorona.fadeAnimation.duration", duration);
	uim.getProperty("FrxNodeCorona.fadeAnimation.refreshRate", rfRate);
	uim.getProperty("FrxNodeCorona.fadeAnimation.tweenType", tweenType);
	fadeAnimation = FadeAnimation::create();
	fadeAnimation->setRefreshRate(rfRate);
	fadeAnimation->setDuration(duration);
	fadeAnimation->setTweenType(tweenType);
	fadeAnimation->setComponent(c);
}
//-----------------------------------------------------------------------------
void FrxNodeUI::installUI(sdc::AComponentPtr c) {
	Super::installUI(c);
}
namespace {
	SAMBAG_PROPERTY_TAG(FrxNodeCoronaPropertyTag, "FrxNodeCorona.color");
} // namespace(s)
//-----------------------------------------------------------------------------
void FrxNodeUI::clipCorona(sd::IDrawContext::Ptr cn, const sd::Point2D &loc,
		double coreRadius, double coronaRadius)
{
	cn->setFillRule(sd::IDrawContext::FILL_RULE_EVEN_ODD);
	cn->arc(loc, coronaRadius);
	cn->arc(loc, coreRadius);
	cn->clip();
}
//-----------------------------------------------------------------------------
void FrxNodeUI::drawCorona(sd::IDrawContext::Ptr cn, sdc::AComponentPtr c) {
	sd::ColorRGBA coronaCol = 
		sdcu::getUIPropertyCached<FrxNodeCoronaPropertyTag>(sd::ColorRGBA());
	coronaCol.setA(getCoronaAlpha());
	FrxComponent::Ptr node = boost::dynamic_pointer_cast<FrxComponent>(c);
	sd::Point2D loc = node->getPivot();
	double rCore = getCoreRadius(c), rCorona = getCoronaRadius(c);
	// clip
	cn->save();
	clipCorona(cn, loc, rCore, rCorona);
	//draw
	cn->arc(loc, rCorona);
	cn->setFillColor(coronaCol);
	cn->fill();
	cn->restore();
}
//-----------------------------------------------------------------------------
void FrxNodeUI::draw(sd::IDrawContext::Ptr cn, sdc::AComponentPtr c) {
	if (!usesCorona() || getCoronaAlpha() == 0.)
		return;
	drawCorona(cn, c);
}
//-----------------------------------------------------------------------------
void FrxNodeUI::drag(const sdc::events::MouseEvent &ev) {
	namespace geom = boost::geometry;
	namespace trans = geom::strategy::transform;
	typedef trans::translate_transformer<double, 2, 2> Transl;
	sdc::AComponent::Ptr c = ev.getSource();
	FrxCircuidView::Ptr circ = c->getFirstContainer<FrxCircuidView>();
	SAMBAG_ASSERT(circ);
	// cacl transl.
	sd::Point2D distance = 
		circ->getViewport()->getView()->getLocationOnComponent(ev.getLocationOnScreen());
	boost::geometry::subtract_point(distance, clickLoc);
	Transl transl(distance.x(), distance.y());
	// update
	sd::Point2D loc; 
	geom::transform(c->getLocation(), loc, transl);
	c->setLocation(loc);
	clickLoc = circ->getViewport()->getView()->getLocationOnComponent(ev.getLocationOnScreen());
}
//-----------------------------------------------------------------------------
void FrxNodeUI::beginConnecting(const sdc::events::MouseEvent &ev) {
	sdc::AComponent::Ptr c = ev.getSource();
	FrxCircuidView::Ptr circ = c->getFirstContainer<FrxCircuidView>();
	SAMBAG_ASSERT(circ);
	if (!circ->containsComponent(toConnect))
		circ->add(toConnect, FrxCircuidView::Z_InteractiveStuff);
	// setline coord.
	FrxComponent::Ptr frxC = boost::dynamic_pointer_cast<FrxComponent>(c);
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
namespace {
	struct CanConnect {
		bool OnError(FrxNode &a, FrxNode &b) {return false;}
		bool Fire(FrxNode &a, FrxNode &b) {return false;}
		bool Fire(FrxInputNode &a, FrxOutputNode &b) { 
			return true;
		}
		bool Fire(FrxInputNode &a, FrxEntryNode &b) {
			return true;
		}
		bool Fire(FrxOutputNode &a, FrxExitNode &b) {
			return true;
		}
		bool Fire(FrxEntryNode &a, FrxExitNode &b) {
			return true;
		}
		bool Fire(FrxStdKnob &a, FrxStdKnob &b) {
			return true;
		}
	};
	bool canConnect(FrxNodePtr from, FrxNodePtr to) {
		typedef LOKI_TYPELIST_5(
			FrxInputNode,
			FrxOutputNode, 
			FrxEntryNode, 
			FrxExitNode,
			FrxStdKnob
		) Types;
		typedef Loki::StaticDispatcher  <
			CanConnect,
			FrxNode, 
			Types,
			true,
			FrxNode,
			Types,
			bool
		> Dispatcher;
		return Dispatcher::Go(*(from.get()), *(to.get()), CanConnect());
	}
} // namespace(s)
//-----------------------------------------------------------------------------
FrxNodeUI::ConnectingComponents
FrxNodeUI::getConnectingComponents(const sdc::events::MouseEvent &ev) 
{
	ConnectingComponents res;
	sdc::AComponent::Ptr c = ev.getSource();
	FrxCircuidView::Ptr circ = c->getFirstContainer<FrxCircuidView>();
	if (!circ)
		return res;
	// uset usr message
	boost::get<0>(res) = boost::dynamic_pointer_cast<FrxNode>(c);
	const sd::Point2D &loc = 
		circ->getViewport()->getView()->getLocationOnComponent(ev.getLocationOnScreen());
	
	boost::get<1>(res) = boost::dynamic_pointer_cast<FrxNode>(
		circ->findComponentOnPoint(loc, 
		FrxCircuidView::ZArea_BeginNodes, 
		FrxCircuidView::ZArea_EndNodes)
	);
	boost::get<2>(res) = loc;
	return res;
}
//-----------------------------------------------------------------------------
void FrxNodeUI::connecting(const sdc::events::MouseEvent &ev) {
	sdc::AComponent::Ptr c = ev.getSource();
	FrxCircuidView::Ptr circ = c->getFirstContainer<FrxCircuidView>();
	SAMBAG_ASSERT(circ);
	
	FrxNodePtr from, to;
	sd::Point2D loc;
	boost::tie(from, to, loc) = getConnectingComponents(ev);
	std::stringstream ss;
	std::string type("default");
	if (to) {
		if (!canConnect(from, to)) {
			ss<<"unable ";
			type = "warning";
		}
	}
	ss<<"connecting to...";
	if (to) {
		ss<<to->getName();
	}
	circ->setUserMessage(ss.str(), type);
	// setline coord.
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
	// uset usr message
	circ->setUserMessage("");

	toConnect->setVisible(false);
	
	FrxNodePtr from, to;
	sd::Point2D loc;
	boost::tie(from, to, loc) = getConnectingComponents(ev);

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
	clickLoc = 
		circ->getViewport()->getView()->getLocationOnComponent(ev.getLocationOnScreen());
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
FrxNodeUI::~FrxNodeUI() {
}
//-----------------------------------------------------------------------------
void FrxNodeUI::ensureToBeTheSingleCoronaOnView() {
	static FadeAnimation::WPtr _lastCoronaAni;
	FadeAnimation::Ptr lastCoronaAni = _lastCoronaAni.lock();
	if (!lastCoronaAni) {
		_lastCoronaAni = fadeAnimation;
		return;
	}
	if (lastCoronaAni==fadeAnimation) {
		std::cout<<"?"<<std::endl;
		return;
	}
	lastCoronaAni->UpdatePolicy::update(0.);
	_lastCoronaAni = fadeAnimation;
}
//-----------------------------------------------------------------------------
void FrxNodeUI::mouseEntered(const sdc::events::MouseEvent &ev)  {
	if (inside) {
		return;
	}
	/* Problem: timer lock. The timer dosen't stop (under win32)
	   immediately. The best solution (for now) is to ignore it.*/
	//if (fadeAnimation->isRunning()) {
	//	fadeAnimation->stop();
	//}
	fadeAnimation->setStartValue(getCoronaAlpha());
	fadeAnimation->setEndValue(FINAL_ALPHA);
	fadeAnimation->start();
	inside = true;
}
//-----------------------------------------------------------------------------
void FrxNodeUI::mouseExited(const sdc::events::MouseEvent &ev) {
	if (!inside) {
		return;
	}
	/* Problem: timer lock. The timer dosen't stop (under win32)
	   immediately. The best solution (for now) is to ignore it.*/
	//if (fadeAnimation->isRunning()) {
	//	fadeAnimation->stop();
	//Users/johannesunger/workspace/vstforx_next/VSTForx-NEXT.xcodeproj/}
	fadeAnimation->setStartValue(getCoronaAlpha());
	fadeAnimation->setEndValue(0.);
	fadeAnimation->start();
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
	enum {F=sdce::MouseEvent::ALL_EVENTS & ~sdce::MouseEvent::DISCO_MOUSE_WHEEL};
	sdc::events::MouseEventSwitch<F>::delegate(ev, *this);
}
}}}} // namespace(s)
