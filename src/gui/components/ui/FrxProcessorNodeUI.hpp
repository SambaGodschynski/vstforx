/*
 * FrxProcessorNodeUI.hpp
 *
 *  Created on: Mon Aug 20 10:46:06 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXPROCESSORNODEUI_H
#define SAMBAG_FRXPROCESSORNODEUI_H

#include <boost/shared_ptr.hpp>
#include <gui/components/FrxProcessorNode.hpp>
#include "FrxNodeUI.hpp"

namespace frx { namespace gui {
namespace components { namespace ui {
namespace sd = sambag::disco;
namespace sdc = sd::components;
namespace sdcu = sdc::ui;

///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
namespace {
	template <class PrType>
	sambag::com::Number getProcessorRadius() {
		SAMBAG_PROPERTY_TAG(PropertyTag, "Processor.radius");
		return sdcu::getUIPropertyCached<PropertyTag>((double)0.);
	}
} // namespace

//=============================================================================
/** 
  * @class FrxProcessorNodeUI.
  */
template <class _ConcreteProcessor>
class FrxProcessorNodeUI : public FrxNodeUI {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef FrxNodeUI Super;
	//-------------------------------------------------------------------------
	typedef _ConcreteProcessor ConcreteProcessor;
	//-------------------------------------------------------------------------
	typedef FrxProcessorNodeUI<ConcreteProcessor> ThisClass;
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<ThisClass> Ptr;
private:
protected:
	//-------------------------------------------------------------------------
	FrxProcessorNodeUI();
public:
	//-------------------------------------------------------------------------
	virtual void beginConnecting(const sdc::events::MouseEvent &ev) {}
	//-------------------------------------------------------------------------
	virtual void connecting(const sdc::events::MouseEvent &ev) {}
	//-------------------------------------------------------------------------
	virtual void endConnecting(const sdc::events::MouseEvent &ev) {}
	//-------------------------------------------------------------------------
	virtual sambag::com::Number getCoreRadius(sdc::AComponentPtr c) const {
		return getProcessorRadius<ConcreteProcessor>();
	}
	//-------------------------------------------------------------------------
	static Ptr create() {
		Ptr res(new ThisClass());
		res->postConstructor(res);
		return res;
	}
	//-------------------------------------------------------------------------
	virtual bool contains(sdc::AComponentPtr c, const sd::Point2D &p);
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
}; // FrxProcessorNodeUI
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
template <class CT>
FrxProcessorNodeUI<CT>::FrxProcessorNodeUI() {
}
//-----------------------------------------------------------------------------
template <class CT>
bool FrxProcessorNodeUI<CT>::contains(sdc::AComponentPtr c, const sd::Point2D &p) 
{
	return Super::contains(c, p);
}
//-----------------------------------------------------------------------------
template <class CT>
void FrxProcessorNodeUI<CT>::installUI(sdc::AComponentPtr c) {
	Super::installUI(c);
	c->EventSender<sdc::events::MouseEvent>::addTrackedEventListener(
		boost::bind(&Super::onMouse, this, _1, _2),
		getPtr()
	);
}
//-----------------------------------------------------------------------------
template <class CT>
void FrxProcessorNodeUI<CT>::draw(sd::IDrawContext::Ptr cn, sdc::AComponentPtr c) {
	Super::draw(cn, c);
	FrxProcessorNode::Ptr node = boost::shared_dynamic_cast<FrxProcessorNode>(c);
	cn->translate(node->getPivot());
	cn->arc(sd::Point2D(0, 0), getCoreRadius(node));
	cn->setFillColor(node->getForeground());
	cn->fill();
}
}}}} // namespace(s)

#endif /* SAMBAG_FRXPROCESSORNODEUI_H */
