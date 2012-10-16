/*
 * FrxConnection.cpp
 *
 *  Created on: Mon Aug 20 10:44:56 2012
 *      Author: Johannes Unger
 */

#include "FrxConnection.hpp"
#include "ui/FrxConnectionUI.hpp"
#include "FrxCircuidView.hpp"

namespace frx { namespace gui { namespace components {
//=============================================================================
//  Class FrxConnection
//=============================================================================
//-----------------------------------------------------------------------------
FrxConnection::FrxConnection() {
	setName("FrxConnection");
	installListeners();
}
//-----------------------------------------------------------------------------
void FrxConnection::installListeners() {
	EventSender<sce::PropertyChanged>::addEventListener(
		boost::bind(&FrxConnection::onPropertyChanged, this, _1, _2)
	);
}
//-----------------------------------------------------------------------------
FrxConnection::Connection FrxConnection::connect(FrxComponent::Ptr c) {
	return c->EventSender<sce::PropertyChanged>::addTrackedEventListener (
		boost::bind(&FrxConnection::onComponentsPropertyChanged, this, _1, _2),
		getPtr()
	);
}
//-----------------------------------------------------------------------------
void FrxConnection::onPropertyChanged(void*, const sce::PropertyChanged &ev) {
	if (ev.getPropertyName() == sdc::AComponent::PROPERTY_UI)
		resetBounds();
}
//-----------------------------------------------------------------------------
void FrxConnection::installComponentListeners(FrxComponent::Ptr c) {
	c->EventSender<OnRemoving>::addTrackedEventListener(
		boost::bind(&FrxConnection::onComponentRemoving, this, _1, _2),
		getPtr()
	);
}
//-----------------------------------------------------------------------------
void FrxConnection::onComponentRemoving(void *src, const OnRemoving &ev) {
	FrxConnection::Ptr saftey = getPtr(); // hold object
	srcConnection.disconnect();
	dstConnection.disconnect();
	ev.view->remove(getPtr());
}
//-----------------------------------------------------------------------------
void FrxConnection::setSrcComponent(FrxComponent::Ptr a) {
	if (src)
		srcConnection.disconnect();
	src = a;
	resetBounds();
	srcConnection = connect(src);
	installComponentListeners(src);
}
//-----------------------------------------------------------------------------
void FrxConnection::setDstComponent(FrxComponent::Ptr b) {
	if (dst)
		dstConnection.disconnect();
	dst = b;
	resetBounds();
	dstConnection = connect(dst);
	installComponentListeners(dst);
}
//-----------------------------------------------------------------------------
void FrxConnection::onComponentsPropertyChanged(void*, 
	const sce::PropertyChanged &ev) 
{
	if (ev.getPropertyName() == sdc::AComponent::PROPERTY_BOUNDS)
		resetBounds();
}
//-----------------------------------------------------------------------------
void FrxConnection::resetBounds() {
	if ( !src || !dst )
		return;
	sd::Rectangle r = getBounds();
	sd::Point2D aLoc = src->getLocation();
	boost::geometry::add_point(aLoc, src->getPivot());
	sd::Point2D bLoc = dst->getLocation();
	boost::geometry::add_point(bLoc, dst->getPivot());
	r = sd::Rectangle(
		sd::minimize(aLoc, bLoc),
		sd::maximize(aLoc, bLoc)
	);
	// adjust bounds (considering line style)
	ui::FrxConnectionUIBase::Ptr ui = 
		boost::shared_dynamic_cast<ui::FrxConnectionUIBase>(getUI());
	if (ui)
		ui->adjustBoundingRect(r, getPtr());

	setBounds(r);
}
}}} // namespace(s)
