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
#include <sambag/disco/IResourceManager.hpp>

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
	sd::ISurface::Ptr image;
	//-------------------------------------------------------------------------
	sd::Point2D imageOffset;
	//-------------------------------------------------------------------------
	typedef FrxIOUI<ConcreteIO> ThisClassType;
	//-------------------------------------------------------------------------
	FrxIOUI(){}
private:
public:
	//-------------------------------------------------------------------------
	virtual sambag::com::Number getCoreRadius(sdc::AComponentPtr c) const {
		if (!image)
			return getIORadius<ConcreteIO>();
		return image->getSize().width()/2.;
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
	if (image) {
		cn->translate(imageOffset);
		cn->drawSurface(image);
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
	image = _ioDef<CIO>(c);
	Super::installUI(c);
	if (image) {
		FrxIO::Ptr io = boost::shared_dynamic_cast<FrxIO>(c);
		sd::Rectangle r = image->getSize();
		imageOffset = io->getPivot();
		boost::geometry::subtract_point(imageOffset, 
			sd::Point2D(r.width()/2., r.height()/2.));
	}
}
}}}} // namespace(s)

#endif /* SAMBAG_FRXIOUI_H */
