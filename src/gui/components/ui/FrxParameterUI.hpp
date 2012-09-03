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
#include "FrxParameterMouseListener.hpp"
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
	FrxParameterMouseListener mouseListener;
	//-------------------------------------------------------------------------
	typedef FrxParameterUI<ParameterType> ThisClass;
public:
	//-------------------------------------------------------------------------
	virtual sambag::com::Number getCoreRadius(sdc::AComponentPtr c) const {
		return getParameterRadius<ParameterType>();
	}
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
}; // FrxParameterUI
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
template <class PT>
void FrxParameterUI<PT>::installUI(sdc::AComponentPtr c) {
	Super::installUI(c);
}
//-----------------------------------------------------------------------------
template <class PT>
void FrxParameterUI<PT>::draw(sd::IDrawContext::Ptr cn, sdc::AComponentPtr c) {
	Super::draw(cn, c);
}
	
}}}} // namespace(s)

#endif /* SAMBAG_FRXPARAMETERUI_H */
