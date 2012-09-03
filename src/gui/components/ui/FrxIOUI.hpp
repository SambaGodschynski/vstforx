/*
 * FrxIOUI.hpp
 *
 *  Created on: Mon Aug 27 10:35:54 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXIOUI_H
#define SAMBAG_FRXIOUI_H

#include <boost/shared_ptr.hpp>
#include <gui/components/FrxConcreteIO.hpp>
#include "FrxNodeUI.hpp"

namespace frx { namespace gui {
namespace components { namespace ui { 
namespace sd = sambag::disco;
namespace sdc = sd::components;
namespace sdcu = sdc::ui;
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
namespace {
	template <class IOType>
	sambag::com::Number getIORadius() {
		SAMBAG_PROPERTY_TAG(PropertyTag, "Entry.radius");
		return sdcu::getUIPropertyCached<PropertyTag>((double)0.);
	}
	template <>
	sambag::com::Number getIORadius<ioTypes::Input>() {
		SAMBAG_PROPERTY_TAG(PropertyTag, "ProcessorInput.radius");
		return
			sdcu::getUIPropertyCached<PropertyTag>((double)0.);
	}
	template <>
	sambag::com::Number getIORadius<ioTypes::Output>() {
		SAMBAG_PROPERTY_TAG(PropertyTag, "ProcessorOutput.radius");
		return
			sdcu::getUIPropertyCached<PropertyTag>((double)0.);
	}
} // namespace
//=============================================================================
/** 
  * @class FrxIOUI.
  */
template <class _ConcreteIO>
class FrxIOUI : public FrxNodeUI {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef FrxNodeUI Super;
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxIOUI> Ptr;
	//-------------------------------------------------------------------------
	typedef _ConcreteIO ConcreteIO;
protected:
	//-------------------------------------------------------------------------
	typedef FrxIOUI<ConcreteIO> ThisClassType;
	//-------------------------------------------------------------------------
	FrxIOUI(){}
private:
public:
	//-------------------------------------------------------------------------
	virtual sambag::com::Number getCoreRadius(sdc::AComponentPtr c) const {
		return getIORadius<ConcreteIO>();
	}
	//-------------------------------------------------------------------------
	virtual void installUI(sdc::AComponentPtr c);
	//-------------------------------------------------------------------------
	static Ptr create() {
		Ptr res = Ptr(new ThisClassType());
		res->self = res;
		return res;
	}
	//-------------------------------------------------------------------------
	virtual void draw(sd::IDrawContext::Ptr cn, sdc::AComponentPtr c);
}; // FrxIOUI
///////////////////////////////////////////////////////////////////////////////
namespace {
	template <class IOType>
	void setIODefaults(sdc::AComponentPtr c) {}
	template <>
	void setIODefaults<ioTypes::Input>(sdc::AComponentPtr c) {
		sd::ColorRGBA col;
		sdcu::getUIManager().getProperty("ProcessorInput.color", col);
		c->setForeground(col);
	}
	template <>
	void setIODefaults<ioTypes::Output>(sdc::AComponentPtr c) {
		sd::ColorRGBA col;
		sdcu::getUIManager().getProperty("ProcessorOutput.color", col);
		c->setForeground(col);
	}
	template <>
	void setIODefaults<ioTypes::Entry>(sdc::AComponentPtr c) {
		sd::ColorRGBA col;
		sdcu::getUIManager().getProperty("Entry.color", col);
		c->setForeground(col);
	}
	template <>
	void setIODefaults<ioTypes::Exit>(sdc::AComponentPtr c) {
		sd::ColorRGBA col;
		sdcu::getUIManager().getProperty("Exit.color", col);
		c->setForeground(col);
	}
} // namespace
//-----------------------------------------------------------------------------
template <class CIO>
void FrxIOUI<CIO>::draw(sd::IDrawContext::Ptr cn, sdc::AComponentPtr c) {
	Super::draw(cn, c);
	FrxIO::Ptr io = boost::shared_dynamic_cast<FrxIO>(c);
	sd::Point2D loc = io->getPivot();
	cn->arc(loc, getCoreRadius(io));
	cn->setFillColor(io->getForeground());
	cn->fill();
}
//-----------------------------------------------------------------------------
template <class CIO>
void FrxIOUI<CIO>::installUI(sdc::AComponentPtr c) {
	Super::installUI(c);
	setIODefaults<CIO>(c);
}
}}}} // namespace(s)

#endif /* SAMBAG_FRXIOUI_H */
