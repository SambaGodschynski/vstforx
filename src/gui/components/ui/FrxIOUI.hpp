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
#include <gui/HandyNamespaces.hpp>

namespace frx { namespace gui {
namespace components { namespace ui { 
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
namespace {
	SAMBAG_PROPERTY_TAG(EntryPropertyTag, "Entry.radius");
	SAMBAG_PROPERTY_TAG(InputPropertyTag, "ProcessorInput.radius");
	SAMBAG_PROPERTY_TAG(OutputPropertyTag, "ProcessorOutput.radius");
	template <class IOType>
	sambag::com::Number getIORadius() {
		return sdcu::getUIPropertyCached<EntryPropertyTag>((double)0.);
	}
	template <>
	sambag::com::Number getIORadius<ioTypes::Input>() {
		return
			sdcu::getUIPropertyCached<InputPropertyTag>((double)0.);
	}
	template <>
	sambag::com::Number getIORadius<ioTypes::Output>() {
		return
			sdcu::getUIPropertyCached<OutputPropertyTag>((double)0.);
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
		if (!hasImage())
			return getIORadius<ConcreteIO>();
		return getImage()->getSize().width()/2.;
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
	sd::ISurface::Ptr _ioDef(sdc::AComponentPtr c) {
		return sd::ISurface::Ptr();
	}
	template <>
	sd::ISurface::Ptr _ioDef<ioTypes::Input>(sdc::AComponentPtr c) {
		sd::ColorRGBA col;
		sdcu::getUIManager().getProperty("ProcessorInput.bgColor", col);
		c->setBackground(col);
		sdcu::getUIManager().getProperty("ProcessorInput.fgColor", col);
		c->setForeground(col);
		return sd::getResourceManager().getImage("ProcessorInput.image");
	}
	template <>
	sd::ISurface::Ptr _ioDef<ioTypes::Output>(sdc::AComponentPtr c) {
		sd::ColorRGBA col;
		sdcu::getUIManager().getProperty("ProcessorOutput.bgColor", col);
		c->setBackground(col);
		sdcu::getUIManager().getProperty("ProcessorOutput.fgColor", col);
		c->setForeground(col);
		return sd::getResourceManager().getImage("ProcessorOutput.image");
	}
	template <>
	sd::ISurface::Ptr _ioDef<ioTypes::Entry>(sdc::AComponentPtr c) {
		sd::ColorRGBA col;
		sdcu::getUIManager().getProperty("Entry.bgColor", col);
		c->setBackground(col);
		sdcu::getUIManager().getProperty("Entry.fgColor", col);
		c->setForeground(col);
		return sd::getResourceManager().getImage("Entry.image");
	}
	template <>
	sd::ISurface::Ptr _ioDef<ioTypes::Exit>(sdc::AComponentPtr c) {
		sd::ColorRGBA col;
		sdcu::getUIManager().getProperty("Exit.bgColor", col);
		c->setBackground(col);
		sdcu::getUIManager().getProperty("Exit.fgColor", col);
		c->setForeground(col);
		return sd::getResourceManager().getImage("Exit.image");
	}
} // namespace
//-----------------------------------------------------------------------------
template <class CIO>
void FrxIOUI<CIO>::draw(sd::IDrawContext::Ptr cn, sdc::AComponentPtr c) {
	Super::draw(cn, c);
	if (hasImage()) {
		drawImage(cn, c);
		return;
	}
	FrxIO::Ptr io = boost::shared_dynamic_cast<FrxIO>(c);
	sd::Point2D loc = io->getPivot();
	sambag::com::Number r = getCoreRadius(io);
	cn->arc(loc, r);
	cn->setStrokeColor(io->getForeground());
	cn->stroke();
	cn->arc(loc, r);
	cn->setFillColor(io->getBackground());
	cn->fill();
}
//-----------------------------------------------------------------------------
template <class CIO>
void FrxIOUI<CIO>::installUI(sdc::AComponentPtr c) {
	setImage(_ioDef<CIO>(c));
	Super::installUI(c);
}
}}}} // namespace(s)

#endif /* SAMBAG_FRXIOUI_H */
