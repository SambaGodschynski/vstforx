/*
 * FrxSvgNodeUI.cpp
 *
 *  Created on: Mon Aug 20 10:46:25 2012
 *      Author: Johannes Unger
 */

#include "FrxSvgNodeUI.hpp"
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
//=============================================================================
//  Class FrxSvgNodeUI
//=============================================================================
//------------------------------------------------------------------------------
void FrxSvgNodeUI::installListeners(sdc::AComponent::Ptr c) {
	// listeners
	getCore()->sdc::EventSender<sdc::events::MouseEvent>::addTrackedEventListener(
		boost::bind(&FrxSvgNodeUI::onMouse, this, _1, _2),
		getPtr()
	);
	// listeners
	getCorona()->sdc::EventSender<sdc::events::MouseEvent>::addTrackedEventListener(
		boost::bind(&FrxSvgNodeUI::onMouse, this, _1, _2),
		getPtr()
	);
}
//-----------------------------------------------------------------------------
void FrxSvgNodeUI::setCoronaAlpha(double alpha) {
    sdc::SvgComponent::Dummy::Ptr component = _corona.lock();
    if (!component) {
        return;
    }
    sdsg::Style style = component->getStyle();
    style.fillOpacity(alpha);
    style.strokeOpacity(alpha);
    component->setStyle(style);
    if (alpha<0.1) {
        if (component->isVisible()) {
            component->setVisible(false);
        }
    } else {
        if (!component->isVisible()) {
            component->setVisible(true);
        }
    }
    FrxComponent::Ptr c = component->getFirstContainer<FrxComponent>();
    c->redraw();
}
//-----------------------------------------------------------------------------
sdc::SvgComponent::Dummy::Ptr FrxSvgNodeUI::getCorona() const {
    return getSvgSubComponent("#corona");
}
//-----------------------------------------------------------------------------
sdc::SvgComponent::Dummy::Ptr FrxSvgNodeUI::getCore() const {
    return getSvgSubComponent("#core");
}
//-----------------------------------------------------------------------------
void FrxSvgNodeUI::installDefaults(sdc::AComponent::Ptr c) {
	Super::installDefaults(c);
    // set corona alpha
    sdc::SvgComponent::Dummy::Ptr corona = getCorona();
    sdsg::Style style = corona->getStyle();
    alphaEndValue = style.fillOpacity();
    corona->setStyle(style.fillOpacity(0.0));
    corona->setStyle(style.strokeOpacity(0.0));
    corona->setVisible(false);
    _corona = corona;
    
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
	fadeAnimation->_parent = getPtr();
}
//-----------------------------------------------------------------------------
void FrxSvgNodeUI::installUI(sdc::AComponentPtr c) {
	Super::installUI(c);
}
namespace {
	SAMBAG_PROPERTY_TAG(FrxNodeCoronaPropertyTag, "FrxNodeCorona.color");
} // namespace(s)
//-----------------------------------------------------------------------------
void FrxSvgNodeUI::drag(const sdc::events::MouseEvent &ev) {
	namespace geom = boost::geometry;
	namespace trans = geom::strategy::transform;
	typedef trans::translate_transformer<sd::Point2D, sd::Point2D> Transl;
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
void FrxSvgNodeUI::beginConnecting(const sdc::events::MouseEvent &ev) {
	sdc::AComponent::Ptr c = ev.getSource();
	FrxCircuidView::Ptr circ = c->getFirstContainer<FrxCircuidView>();
	SAMBAG_ASSERT(circ);
	if (!circ->containsComponent(toConnect))
		circ->add(toConnect, FrxCircuidView::Z_InteractiveStuff);
	// setline coord.
	FrxComponent::Ptr frxC = c->getFirstContainer<FrxComponent>();
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
FrxSvgNodeUI::ConnectingComponents
FrxSvgNodeUI::getConnectingComponents(const sdc::events::MouseEvent &ev) 
{
	ConnectingComponents res;
	sdc::AComponent::Ptr c = ev.getSource()->getFirstContainer<FrxComponent>();
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
void FrxSvgNodeUI::connecting(const sdc::events::MouseEvent &ev) {
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
void FrxSvgNodeUI::endConnecting(const sdc::events::MouseEvent &ev) {
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
//-----------------------------------------------------------------------------
void FrxSvgNodeUI::mousePressed(const sdc::events::MouseEvent &ev) {
	sdc::AComponent::Ptr c = ev.getSource()->getFirstContainer<FrxComponent>();
    SAMBAG_ASSERT(c);
	FrxCircuidView::Ptr circ = c->getFirstContainer<FrxCircuidView>();
	SAMBAG_ASSERT(circ);
	clickLoc = 
		circ->getViewport()->getView()->getLocationOnComponent(ev.getLocationOnScreen());
	if (ev.getButtons() != sdc::events::MouseEvent::DISCO_BTN1)
		return;
	if (ev.getSource() == getConnectingHandle()) {
		beginConnecting(ev);
		return;
	}
}
//-----------------------------------------------------------------------------
void FrxSvgNodeUI::mouseReleased(const sdc::events::MouseEvent &ev)  {
	if (ev.getSource() == getConnectingHandle())
		endConnecting(ev);
}
//-----------------------------------------------------------------------------
FrxSvgNodeUI::~FrxSvgNodeUI() {
}
//-----------------------------------------------------------------------------
void FrxSvgNodeUI::mouseEntered(const sdc::events::MouseEvent &ev)  {
    startCoronaFadeIn();
}
//-----------------------------------------------------------------------------
void FrxSvgNodeUI::mouseExited(const sdc::events::MouseEvent &ev) {
   startCoronaFadeOut();
}
//-----------------------------------------------------------------------------
void FrxSvgNodeUI::startCoronaFadeIn() {
	fadeAnimation->setStartValue(getCoronaAlpha());
	fadeAnimation->setEndValue(alphaEndValue);
	fadeAnimation->start();
}
//-----------------------------------------------------------------------------
void FrxSvgNodeUI::startCoronaFadeOut() {
	fadeAnimation->setStartValue(getCoronaAlpha());
	fadeAnimation->setEndValue(0.);
	fadeAnimation->start();
}
//-----------------------------------------------------------------------------
void FrxSvgNodeUI::mouseClicked(const sdc::events::MouseEvent &ev) {
	sdc::AComponent::Ptr c = ev.getSource()->getFirstContainer<FrxComponent>();
    SAMBAG_ASSERT(c);
	FrxCircuidView::Ptr circ = c->getFirstContainer<FrxCircuidView>();
	SAMBAG_ASSERT(circ);
	getFrxControl(circ).handleContextMenuPopup(ev);
}
//-----------------------------------------------------------------------------
void FrxSvgNodeUI::mouseMoved(const sdc::events::MouseEvent &ev) {
}
//-----------------------------------------------------------------------------
void FrxSvgNodeUI::mouseDragged(const sdc::events::MouseEvent &ev) {
	if (ev.getSource() == getDragHandle()) {
        sdc::AComponent::Ptr c = ev.getSource()->getFirstContainer<FrxComponent>();
        SAMBAG_ASSERT(c);
        sdc::events::MouseEvent newEv = ev;
        newEv.updateSource(c);
		drag(newEv);
		return;
	}
	if (ev.getSource() == getConnectingHandle()) {
        sdc::AComponent::Ptr c = ev.getSource()->getFirstContainer<FrxComponent>();
        SAMBAG_ASSERT(c);
        sdc::events::MouseEvent newEv = ev;
        newEv.updateSource(c);
		connecting(newEv);
		return;
	}
}	
//-----------------------------------------------------------------------------
void FrxSvgNodeUI::mouseWheelMoved(const sdc::events::MouseEvent &ev) {
}
//-----------------------------------------------------------------------------
void FrxSvgNodeUI::onMouse(void *src,
    const sdc::events::MouseEvent &ev)
{
    //SAMBAG_LOG_TRACE<<ev.toString();
	enum {F=sdce::MouseEvent::ALL_EVENTS & ~sdce::MouseEvent::DISCO_MOUSE_WHEEL};
	sdc::events::MouseEventSwitch<F>::delegate(ev, *this);
}
//-----------------------------------------------------------------------------
bool FrxSvgNodeUI::contains(sdc::AComponentPtr c, const sd::Point2D &p) {
    sdc::AComponent::Ptr corona = getCorona();
    if (corona->isVisible()) {
        return sd::Rectangle(0,0,corona->getWidth(),
               corona->getHeight()).contains(p);
    }
    sdc::AComponent::Ptr core = getCore();
    return core->getBounds().contains(p);
}
}}}} // namespace(s)
