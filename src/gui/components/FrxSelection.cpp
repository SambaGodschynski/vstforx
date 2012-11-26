/*
 * FrxSelection.cpp
 *
 *  Created on: Tue Aug 28 10:30:08 2012
 *      Author: Johannes Unger
 */

#include "FrxSelection.hpp"
#include "FrxCircuidView.hpp"
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
	formatter = VerticalFormatter::create();
}
//-----------------------------------------------------------------------------
void FrxSelection::setContent(const FrxSelection::ContentContainer &container) 
{
	contentViaSelection = true;
	clearContent();
	if (!container.empty())
		setVisible(true);
	this->content = container;
	updateBounds();
}
//-----------------------------------------------------------------------------
void FrxSelection::setFormatter(IFormatter::Ptr fomatter) {
	this->formatter = formatter;
}
//-----------------------------------------------------------------------------
void FrxSelection::addElement(sdc::AComponent::Ptr c) {
	if (contentViaSelection) { // clear previous selected content
		clearContent();
		contentViaSelection = false;
	}
	if (!c)
		return;
	if (formatter) {
		if (content.empty()) { // first element
			FrxCircuidView::Ptr view = getFirstContainer<FrxCircuidView>();
			SAMBAG_ASSERT(view);
			// place view
			sd::Point2D loc = view->getViewport()->getViewPosition(); 
			formatter->setCursor(loc);
		}
		formatter->addElement(c);
	}
	content.push_back(c);
	updateBounds();
	setVisible(true);
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
	if (formatter)
		formatter->reset();
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
