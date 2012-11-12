/*
 * FrxSelection.cpp
 *
 *  Created on: Tue Aug 28 10:30:08 2012
 *      Author: Johannes Unger
 */

#include "FrxSelection.hpp"
#include <sambag/disco/Geometry.hpp>
#include <boost/foreach.hpp>
#include "VerticalFormatter.hpp"

namespace frx { namespace gui { namespace components {
//=============================================================================
//  Class FrxSelection
//=============================================================================
//-----------------------------------------------------------------------------
void FrxSelection::postConstructor() {
	Super::postConstructor();
}
//-----------------------------------------------------------------------------
void FrxSelection::setContent(const FrxSelection::ContentContainer &container) 
{
	clearContent();
	if (!container.empty())
		setVisible(true);
	this->content = container;
	updateBounds();
}
//-----------------------------------------------------------------------------
void FrxSelection::addElement(sdc::AComponent::Ptr c) {
	if (!c)
		return;
	setVisible(true);
	content.push_back(c);
	updateBounds();
}
//-----------------------------------------------------------------------------
void FrxSelection::updateBounds() {
	using namespace sambag::disco;
	setBounds(NULL_RECTANGLE);
	BOOST_FOREACH(sdc::AComponent::WPtr _c, content) {
		sdc::AComponent::Ptr c = _c.lock();
		if (!c)
			continue;
		extendBounds(c);
	}
}
//-----------------------------------------------------------------------------
void FrxSelection::clearContent() {
	content.clear();
	setVisible(false);
}
//-----------------------------------------------------------------------------
void FrxSelection::extendBounds(sdc::AComponent::Ptr c) {
	using namespace sambag::disco;
	Rectangle r = getBounds();
	if (r==NULL_RECTANGLE) {
		setBounds(c->getBounds());
		return;
	}
	r = union_(r, c->getBounds());
	setBounds(r);
}

}}} // namespace(s)
