/*
 * FrxConnection.cpp
 *
 *  Created on: Mon Aug 20 10:44:56 2012
 *      Author: Johannes Unger
 */

#include "FrxConnection.hpp"
#include "ui/FrxConnectionUI.hpp"


namespace frx { namespace gui { namespace components {
//=============================================================================
//  Class FrxConnection
//=============================================================================
//-----------------------------------------------------------------------------
FrxConnection::FrxConnection() {
	setName("FrxConnection");
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
void FrxConnection::setSrcComponent(FrxComponent::Ptr a) {
	if (frxA)
		cnA.disconnect();
	frxA = a;
	resetBounds();
	cnA = connect(frxA);
}
//-----------------------------------------------------------------------------
void FrxConnection::setDstComponent(FrxComponent::Ptr b) {
	if (frxB)
		cnB.disconnect();
	frxB = b;
	resetBounds();
	cnB = connect(frxB);
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
	if ( !frxA || !frxB )
		return;
	sd::Rectangle r = getBounds();
	sd::Point2D aLoc = frxA->getLocation();
	boost::geometry::add_point(aLoc, frxA->getPivot());
	sd::Point2D bLoc = frxB->getLocation();
	boost::geometry::add_point(bLoc, frxB->getPivot());
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
