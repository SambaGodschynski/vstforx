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
#include <sambag/disco/components/DefaultBoundedRangeModel.hpp>
#include "FrxNodeUI.hpp"
#include <gui/components/FrxCircuidView.hpp>
#include <sambag/disco/components/events/MouseEvent.hpp>
#include <sambag/disco/components/ui/UIManager.hpp>
#include <float.h>
#include <gui/HandyNamespaces.hpp>

namespace frx { namespace gui {
namespace components { namespace ui { 
///////////////////////////////////////////////////////////////////////////////
namespace {
	SAMBAG_PROPERTY_TAG(KnobRadiusPropertyTag, "StdKnob.radius");
	template <class ParameterType>
	sambag::com::Number getParameterRadius() {
		return sdcu::getUIPropertyCached<KnobRadiusPropertyTag>((double)0.);
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
	//-------------------------------------------------------------------------
	typedef sdc::DefaultBoundedRangeModelChanged KnobStateChanged;
protected:
	//-------------------------------------------------------------------------
	virtual void onKnobStateChanged(void *src, const KnobStateChanged &ev);
	//-------------------------------------------------------------------------
	FrxParameterUI(){}
	//-------------------------------------------------------------------------
	virtual void installListeners(sdc::AComponent::Ptr c);
private:
	//-------------------------------------------------------------------------
	typedef typename ParameterType::Model Model;
	//-------------------------------------------------------------------------
	typedef FrxParameterUI<ParameterType> ThisClass;
	//-------------------------------------------------------------------------
	FrxParameter::WPtr _parameter;
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
	void mouseWheelRotated(const sdc::events::MouseEvent &ev);
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
void FrxParameterUI<PT>::onKnobStateChanged(void *src, const KnobStateChanged &ev)
{
	FrxParameter::Ptr parameter = _parameter.lock();
	if (!parameter) {
		return;
	}
	sdc::AComponent::Ptr parent = parameter->getParent();
	if (!parent) {
		return;
	}
	parent->redraw(parameter->getBounds());
}
//-----------------------------------------------------------------------------
template <class PT>
void FrxParameterUI<PT>::installUI(sdc::AComponentPtr c) {
	Super::installUI(c);
}
//-----------------------------------------------------------------------------
template <class PT>
void FrxParameterUI<PT>::installListeners(sdc::AComponentPtr c) {
	Super::installListeners(c);
	FrxParameter::Ptr parameter = boost::dynamic_pointer_cast<FrxParameter>(c);
	SAMBAG_ASSERT(parameter);
	_parameter = parameter; 
	sdc::AComponent::Ptr ctrl = parameter->getEncapsulatedCtrl();
	SAMBAG_ASSERT(ctrl);
	ctrl->sdc::EventSender<sdc::events::MouseEvent>::addTrackedEventListener(
		boost::bind(&ThisClass::onMouse, this, _1, _2),
		getPtr()
	);

	sdc::DefaultBoundedRangeModel::Ptr ctrlModel = 
		boost::dynamic_pointer_cast<sdc::DefaultBoundedRangeModel>(ctrl);
	if (!ctrlModel) {
		return;
	}
	ctrlModel->sdc::EventSender<KnobStateChanged>::addTrackedEventListener(
		boost::bind(&ThisClass::onKnobStateChanged, this, _1, _2),
		getPtr()
	);
	ctrl->setMouseWheelEventsEnabled(true);
}
//-----------------------------------------------------------------------------
template <class PT>
void FrxParameterUI<PT>::draw(sd::IDrawContext::Ptr cn, sdc::AComponentPtr c) {
	Super::draw(cn, c);
}
//-----------------------------------------------------------------------------
namespace {
	SAMBAG_PROPERTY_TAG(Corona01PropertyTag, "FrxStdKnobCorona01.color");
	SAMBAG_PROPERTY_TAG(Corona02PropertyTag, "FrxStdKnobCorona02.color");
} // namespace(s)
template <class PT>
void FrxParameterUI<PT>::drawCorona(sd::IDrawContext::Ptr cn, 
	sdc::AComponent::Ptr c)
{
	FrxParameter::Ptr parameter = _parameter.lock();
	if (!parameter) {
		return;
	}
	sd::ColorRGBA coronaCol01 = 
		sdcu::getUIPropertyCached<Corona01PropertyTag>(sd::ColorRGBA());
	sd::ColorRGBA coronaCol02 = 
		sdcu::getUIPropertyCached<Corona02PropertyTag>(sd::ColorRGBA());
	coronaCol01.setA(getCoronaAlpha());
	coronaCol02.setA(getCoronaAlpha());

	sd::Point2D loc = parameter->getPivot();
	double rCore = getCoreRadius(c), rCorona = getCoronaRadius(c);
	// clip
	cn->save();
	clipCorona(cn, loc, rCore, rCorona);
	// draw
	sambag::com::Number sa = 90. * (M_PI / 180.);
	sambag::com::Number ea = 270. * (M_PI / 180.);
	cn->arc(loc, rCorona, sa, ea);
	cn->setFillColor(coronaCol01);
	cn->fill();
	cn->arcNegative(loc, rCorona, sa, ea);
	cn->setFillColor(coronaCol02);
	cn->fill();
	cn->restore();
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
	sdc::AComponent::Ptr src = ev.getSource();
	if ( hitsCorona(src->getParent(), ev.getLocation()) )
		return;
	// knob used and mouse is outside of corona now:
	sdc::events::MouseEvent nEv = ev;
	nEv.updateSource(src->getParent());
	Super::mouseExited(nEv);
	// endEdit
	src->putClientProperty("Parameter.edit", (bool)false);
}
//-----------------------------------------------------------------------------
template <class PT>
void FrxParameterUI<PT>::mouseEntered(const sdc::events::MouseEvent &ev) {
	/*sdc::events::MouseEvent nEv = ev;
	nEv.updateSoure(ev.getSource()->getParent());*/
	Super::mouseEntered(ev);
}
//-----------------------------------------------------------------------------
template <class PT>
void FrxParameterUI<PT>::mouseExited(const sdc::events::MouseEvent &ev) {
	/*sdc::events::MouseEvent nEv = ev;
	nEv.updateSoure(ev.getSource()->getParent());*/
	//Super::mouseExited(ev);
}
//-----------------------------------------------------------------------------
template <class PT>
void FrxParameterUI<PT>::mouseClicked(const sdc::events::MouseEvent &ev) {
	Super::mouseClicked(ev);
}
//-----------------------------------------------------------------------------
template <class PT>
void FrxParameterUI<PT>::mouseMoved(const sdc::events::MouseEvent &ev) {
}
//-----------------------------------------------------------------------------
template <class PT>
void FrxParameterUI<PT>::mouseDragged(const sdc::events::MouseEvent &ev) {
	sdc::AComponent::Ptr src = ev.getSource();
	// beginEdit
	src->putClientProperty("Parameter.edit", (bool)true);
}
//-----------------------------------------------------------------------------
template <class PT>
void FrxParameterUI<PT>::mouseWheelRotated(const sdc::events::MouseEvent &ev) {
}
}}}} // namespace(s)

#endif /* SAMBAG_FRXPARAMETERUI_H */
