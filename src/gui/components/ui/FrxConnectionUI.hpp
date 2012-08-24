/*
 * FrxConnectionUI.hpp
 *
 *  Created on: Mon Aug 20 10:46:11 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXCONNECTIONUI_H
#define SAMBAG_FRXCONNECTIONUI_H

#include <boost/shared_ptr.hpp>
#include <gui/components/FrxConcreteConnections.hpp>
#include <sambag/disco/IDiscoFactory.hpp>
#include "FrxComponentUI.hpp"
#include <sambag/disco/Geometry.hpp>


namespace frx { namespace gui {
namespace components { namespace ui { 
namespace sd = sambag::disco;
namespace sdc = sd::components;
namespace sdcu = sdc::ui;
//=============================================================================
/** 
  * @class FrxConnectionUIBase.
  */
class FrxConnectionUIBase : public FrxComponentUI {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef FrxComponentUI Super;
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxConnectionUIBase> Ptr;
protected:
	//-------------------------------------------------------------------------
	FrxConnectionUIBase() {}
private:
public:
	//-------------------------------------------------------------------------
	virtual ~FrxConnectionUIBase() {}
	//-------------------------------------------------------------------------
	/**
	 * adjust bounds considering line style.
	 */ 
	virtual void 
	adjustBoundingRect(sd::Rectangle &r, FrxConnection::Ptr c) const = 0;
}; // FrxConnectionUIBase
//=============================================================================
/** 
  * @class FrxConnectionUI.
  */
template <class _ConnectionType>
class FrxConnectionUI : public FrxConnectionUIBase {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef FrxConnectionUIBase Super;
	//-------------------------------------------------------------------------
	typedef FrxConcreteConnection<_ConnectionType> _ConcreteConnection;
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxConnectionUI> Ptr;
protected:
	//-------------------------------------------------------------------------
	FrxConnectionUI() {}
private:
public:
	//-------------------------------------------------------------------------
	/**
	 * adjust bounds considering line style.
	 */ 
	virtual void 
	adjustBoundingRect(sd::Rectangle &r, FrxConnection::Ptr c) const;
	//-------------------------------------------------------------------------
	static Ptr create() {
		return Ptr(new FrxConnectionUI());
	}
	//-------------------------------------------------------------------------
	virtual void draw(sd::IDrawContext::Ptr cn, sdc::AComponentPtr c);
}; // FrxConnectionUI
///////////////////////////////////////////////////////////////////////////////
template <class ConnectionType>
void setStyle(sd::IDrawContext::Ptr cn) {
	cn->setStrokeWidth(4);
	cn->setStrokeColor(sd::ColorRGBA(0));
}
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
template <class CT>
void FrxConnectionUI<CT>::adjustBoundingRect(sd::Rectangle &r, 
	FrxConnection::Ptr c) const 
{
	// approach isn't really satisfying, dosen't work with higher 
	// strokewitdh values.
	sd::IDrawContext::Ptr cn = sd::getDiscoFactory()->createContext();
	setStyle<CT>(cn);
	sd::Coordinate lw = cn->getStrokeWidth() * 2.5;
	if (r.width() < lw) {
		sd::Coordinate miss =  lw - r.width();
		r.x0().x( r.x0().x() - miss/2. );
		r.x1().x( r.x1().x() + miss/2. );
	}
	if (r.height() < lw) {
		sd::Coordinate miss =  lw - r.height();
		r.x0().y( r.x0().y() - miss/2. );
		r.x1().y( r.x1().y() + miss/2. );
	}
}
//-----------------------------------------------------------------------------
template <class CT>
void FrxConnectionUI<CT>::draw(sd::IDrawContext::Ptr cn, sdc::AComponentPtr c) {
	_ConcreteConnection::Ptr ccn = 
		boost::shared_dynamic_cast<_ConcreteConnection>(c);
	SAMBAG_ASSERT(ccn);

	sd::Point2D aLoc = ccn->getComponentA()->getLocation();
	boost::geometry::add_point(aLoc, ccn->getComponentA()->getPivot());
	boost::geometry::subtract_point(aLoc, c->getLocation());
	sd::Point2D bLoc = ccn->getComponentB()->getLocation();
	boost::geometry::add_point(bLoc, ccn->getComponentA()->getPivot());
	boost::geometry::subtract_point(bLoc, c->getLocation());

	setStyle<CT>(cn);
	cn->moveTo(aLoc);
	cn->lineTo(bLoc);
	cn->stroke();

}
}}}} // namespace(s)

#endif /* SAMBAG_FRXCONNECTIONUI_H */
