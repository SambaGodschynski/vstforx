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
#include <sambag/disco/components/ui/basic/BasicKnobUI.hpp>

namespace frx { namespace gui {
namespace components { namespace ui { 
namespace sd = sambag::disco;
namespace sdc = sd::components;
namespace sdce = sdc::events;
namespace sdcu = sdc::ui;
//=============================================================================
/** 
  * @class FrxParameterUI.
  */
template <class _ParameterType>
class FrxParameterUI : public FrxNodeUI {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef _ParameterType ParameterType;
	//-------------------------------------------------------------------------
	typedef FrxNodeUI Super;
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxParameterUI> Ptr;
protected:
	//-------------------------------------------------------------------------
	FrxParameterUI(){}
private:
	//-------------------------------------------------------------------------
	typedef typename ParameterType::Model Model;
	//-------------------------------------------------------------------------
	typedef typename sdcu::basic::BasicKnobUI<Model> EncKnobUI;
	//-------------------------------------------------------------------------
	typename EncKnobUI::Ptr basicKnobUI;
	//-------------------------------------------------------------------------
	FrxParameterMouseListener mouseListener;
	//-------------------------------------------------------------------------
	typedef FrxParameterUI<ParameterType> ThisClass;
public:
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
namespace {
template <class ParameterType>
void setParameterComponentSize(sdc::AComponentPtr c) {
	SAMBAG_ASSERT(c->getParent());
	c->getParent()->setSize(sd::Dimension(50, 50));
	c->setSize(sd::Dimension(50, 50));
}
} // namespace
//-----------------------------------------------------------------------------
template <class PT>
void FrxParameterUI<PT>::installUI(sdc::AComponentPtr c) {
	setParameterComponentSize<PT>(c);
	basicKnobUI = typename EncKnobUI::create();
	basicKnobUI->installUI(c);
}
//-----------------------------------------------------------------------------
template <class PT>
void FrxParameterUI<PT>::draw(sd::IDrawContext::Ptr cn, sdc::AComponentPtr c) {
	basicKnobUI->draw(cn, c);
}
	
}}}} // namespace(s)

#endif /* SAMBAG_FRXPARAMETERUI_H */
