/*
 * FrxParameterUI.hpp
 *
 *  Created on: Mon Aug 20 10:46:01 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXPARAMETERUI_H
#define SAMBAG_FRXPARAMETERUI_H

#include <boost/shared_ptr.hpp>
#include <gui/components/FrxConcreteParameter.hpp>
#include "FrxNodeUI.hpp"
#include <sambag/disco/components/events/MouseEvent.hpp>
#include <float.h>

namespace frx { namespace gui {
namespace components { namespace ui { 
namespace sd = sambag::disco;
namespace sdc = sd::components;
namespace sdce = sdc::events;
namespace sdcu = sdc::ui;

///////////////////////////////////////////////////////////////////////////////
namespace {
	template <class ParameterType>
	sambag::com::Number getParameterRadius() {
		SAMBAG_PROPERTY_TAG(PropertyTag, "StdKnob.radius");
		return sdcu::getUIPropertyCached<PropertyTag>((double)0.);
	}
} // namespace

//=============================================================================
/** 
  * @class FrxParameterUI.
  */
template <class _ParameterType>
class FrxParameterUI : public FrxNodeUI {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef FrxNodeUI Super;
	//-------------------------------------------------------------------------
	typedef _ParameterType ParameterType;
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxParameterUI> Ptr;
protected:
	//-------------------------------------------------------------------------
	FrxParameterUI(){}
private:
	//-------------------------------------------------------------------------
	typedef typename ParameterType::Model Model;
	//-------------------------------------------------------------------------
	typedef FrxParameterUI<ParameterType> ThisClass;
public:
	//-------------------------------------------------------------------------
	// MouseEvents
	void mousePressed(const sdc::events::MouseEvent &ev);
	//-------------------------------------------------------------------------
	void mouseReleased(const sdc::events::MouseEvent &ev);
	//-------------------------------------------------------------------------
	void mouseEntered(const sdc::events::MouseEvent &ev);
	//-------------------------------------------------------------------------
	void mouseExited(const sdc::events::MouseEvent &ev);
	//-------------------------------------------------------------------------
	void mouseClicked(const sdc::events::MouseEvent &ev);
	//-------------------------------------------------------------------------
	void mouseMoved(const sdc::events::MouseEvent &ev);
	//-------------------------------------------------------------------------
	void mouseDragged(const sdc::events::MouseEvent &ev);
	//-------------------------------------------------------------------------
	void mouseWheelMoved(const sdc::events::MouseEvent &ev);
	//-------------------------------------------------------------------------
	void onMouse(void *src, const sdc::events::MouseEvent &ev) {
		sdc::events::MouseEventSwitch<>::
			delegate(ev, *this);
	}
	//-------------------------------------------------------------------------
	virtual sambag::com::Number getCoreRadius(sdc::AComponentPtr c) const {
		return getParameterRadius<ParameterType>();
	}
	//-------------------------------------------------------------------------
	virtual sambag::com::Number getCoronaRadius(sdc::AComponentPtr c) const {
		return getCoreRadius(c) + 15.;
	}
	//-------------------------------------------------------------------------
	virtual int 
		determineContext(const sdc::events::MouseEvent &ev) const;
	//-------------------------------------------------------------------------
	static Ptr create() {
		Ptr res(new ThisClass());
		res->self = res;
		return res;
	}
	//-------------------------------------------------------------------------
	/**
	 * Configures the specified component appropriately for the look and feel.
	 * @param c
	 */
	virtual void installUI(sdc::AComponentPtr c);
	//-------------------------------------------------------------------------
	/**
	 * Paints the specified component appropriately for the look and feel.
	 * @param cn
	 * @param c
	 */
	virtual void draw(sd::IDrawContext::Ptr cn, sdc::AComponentPtr c);
	//-------------------------------------------------------------------------
	virtual void drawCorona(sd::IDrawContext::Ptr cn, sdc::AComponent::Ptr c);
}; // FrxParameterUI
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
template <class PT>
void FrxParameterUI<PT>::installUI(sdc::AComponentPtr c) {
	Super::installUI(c);
	FrxParameter::Ptr par = boost::shared_dynamic_cast<FrxParameter>(c);
	SAMBAG_ASSERT(par);
	sdc::AComponent::Ptr ctrl = par->getEncapsulatedCtrl();
	SAMBAG_ASSERT(ctrl);
	ctrl->EventSender<sdc::events::MouseEvent>::addTrackedEventListener(
		boost::bind(&ThisClass::onMouse, this, _1, _2),
		getPtr()
	);
}
//-----------------------------------------------------------------------------
template <class PT>
void FrxParameterUI<PT>::draw(sd::IDrawContext::Ptr cn, sdc::AComponentPtr c) {
	Super::draw(cn, c);
}
//-----------------------------------------------------------------------------
template <class PT>
void FrxParameterUI<PT>::drawCorona(sd::IDrawContext::Ptr cn, 
	sdc::AComponent::Ptr c)
{
	SAMBAG_PROPERTY_TAG(Corona01PropertyTag, "FrxStdKnobCorona01.color");
	sd::ColorRGBA coronaCol01 = 
		sdcu::getUIPropertyCached<Corona01PropertyTag>(sd::ColorRGBA());
	SAMBAG_PROPERTY_TAG(Corona02PropertyTag, "FrxStdKnobCorona02.color");
	sd::ColorRGBA coronaCol02 = 
		sdcu::getUIPropertyCached<Corona02PropertyTag>(sd::ColorRGBA());
	coronaCol01.setA(coronaAlpha);
	coronaCol02.setA(coronaAlpha);

	FrxComponent::Ptr node = boost::shared_dynamic_cast<FrxComponent>(c);
	
	sambag::com::Number sa = 90. * (M_PI / 180.);
	sambag::com::Number ea = 270. * (M_PI / 180.);
	sd::Point2D loc = node->getPivot();
	cn->arc(loc, getCoronaRadius(c), sa, ea);
	cn->setFillColor(coronaCol01);
	cn->fill();
	
	cn->arcNegative(loc, getCoronaRadius(c), sa, ea);
	cn->setFillColor(coronaCol02);
	cn->fill();
}
//-----------------------------------------------------------------------------
template <class PT>
int FrxParameterUI<PT>::determineContext(const sdc::events::MouseEvent &ev) const
{
	if (hitsCorona(ev.getSource(), ev.getLocation())) {
		if (ev.getLocation().x() > getCoronaRadius(ev.getSource())) {
			return DRAG;
		} else {
			return CONNECT;
		}
	}
	return NONE;
}
//-----------------------------------------------------------------------------
template <class PT>
void FrxParameterUI<PT>::mousePressed(const sdc::events::MouseEvent &ev) {
}
//-----------------------------------------------------------------------------
template <class PT>
void FrxParameterUI<PT>::mouseReleased(const sdc::events::MouseEvent &ev) {
	if ( hitsCorona(ev.getSource()->getParent(), ev.getLocation()) )
		return;
	// knob used and mouse is outside of corona now:
	sdc::events::MouseEvent nEv = ev;
	nEv.updateSoure(ev.getSource()->getParent());
	Super::mouseExited(nEv);
}
//-----------------------------------------------------------------------------
template <class PT>
void FrxParameterUI<PT>::mouseEntered(const sdc::events::MouseEvent &ev) {
	sdc::events::MouseEvent nEv = ev;
	nEv.updateSoure(ev.getSource()->getParent());
	Super::mouseEntered(nEv);
}
//-----------------------------------------------------------------------------
template <class PT>
void FrxParameterUI<PT>::mouseExited(const sdc::events::MouseEvent &ev) {
}
//-----------------------------------------------------------------------------
template <class PT>
void FrxParameterUI<PT>::mouseClicked(const sdc::events::MouseEvent &ev) {
}
//-----------------------------------------------------------------------------
template <class PT>
void FrxParameterUI<PT>::mouseMoved(const sdc::events::MouseEvent &ev) {
}
//-----------------------------------------------------------------------------
template <class PT>
void FrxParameterUI<PT>::mouseDragged(const sdc::events::MouseEvent &ev) {
}
//-----------------------------------------------------------------------------
template <class PT>
void FrxParameterUI<PT>::mouseWheelMoved(const sdc::events::MouseEvent &ev) {
}
}}}} // namespace(s)

#endif /* SAMBAG_FRXPARAMETERUI_H */
