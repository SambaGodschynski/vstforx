/*
 * VerticalFormatter.cpp
 *
 *  Created on: Tue Nov  6 21:47:07 2012
 *      Author: Johannes Unger
 */

#include "VerticalFormatter.hpp"
#include <sambag/disco/components/AComponent.hpp>

namespace frx { namespace gui { namespace components {
//=============================================================================
//  Class VerticalFormatter
//=============================================================================
//-----------------------------------------------------------------------------
void VerticalFormatter::setGap(const sd::Coordinate &val) {
	gap = val;
}
//-----------------------------------------------------------------------------
void VerticalFormatter::addElement(sdc::AComponentPtr c) {
	c->setLocation(lastPos);
	if (ccounter <= 1 ) {
		if (ccounter==1)
			ccounter=0;
		lastPos = sd::Point2D(lastPos.x() + c->getWidth() + gap, 0);
	} else {
		ccounter-=1;	
	}
	c->redraw();
}
//-----------------------------------------------------------------------------
void VerticalFormatter::resetOrigin(const sd::Point2D &p) {
	lastPos = p;
}
//-----------------------------------------------------------------------------
void VerticalFormatter::setCompoundCounter(size_t num) {
	ccounter = num;
}
}}} // namespace(s)
