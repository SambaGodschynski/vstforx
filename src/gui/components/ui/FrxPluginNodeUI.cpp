/*
 * FrxPluginNodeUI.cpp
 *
 *  Created on: Mon Aug 20 12:13:10 2012
 *      Author: Johannes Unger
 */

#include "FrxPluginNodeUI.hpp"
#include <sambag/disco/Geometry.hpp>

namespace frx { namespace gui {
namespace components { namespace ui { 

//=============================================================================
//  Class FrxPluginNodeUI
//=============================================================================
//-----------------------------------------------------------------------------
FrxPluginNodeUI::FrxPluginNodeUI() {
}
//-----------------------------------------------------------------------------
FrxPluginNodeUI::Ptr FrxPluginNodeUI::create() {
	Ptr res(new FrxPluginNodeUI());
	res->postConstructor(res);
	return res;
}
//-----------------------------------------------------------------------------
bool FrxPluginNodeUI::contains(sdc::AComponentPtr c, const sd::Point2D &p) {
	return Super::contains(c, p);
}
//-----------------------------------------------------------------------------
void FrxPluginNodeUI::installUI(sdc::AComponentPtr c) {
	return Super::installUI(c);
}
//-----------------------------------------------------------------------------
void FrxPluginNodeUI::draw(sd::IDrawContext::Ptr cn, sdc::AComponentPtr c) {
	Super::draw(cn, c);
	sd::Rectangle r(0, 0, c->getWidth(), c->getHeight());
	sd::Point2D c0 = r.x0();
	sd::Coordinate w = r.width(), h = r.height();
	boost::geometry::add_point(c0, sd::Point2D(w/2., h/2.));
	cn->translate(c0);
	cn->arc(sd::Point2D(), w / 2.5);
	cn->setFillColor(c->getForeground());
	cn->fill();
}
}}}} // namespace(s)
