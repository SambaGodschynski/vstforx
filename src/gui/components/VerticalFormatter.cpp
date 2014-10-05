/*
 * VerticalFormatter.cpp
 *
 *  Created on: Tue Nov  6 21:47:07 2012
 *      Author: Johannes Unger
 */

#include "VerticalFormatter.hpp"
#include <sambag/disco/components/AComponent.hpp>
#include <boost/geometry.hpp>
#include <boost/geometry/multi/geometries/multi_point.hpp>

namespace frx { namespace gui { namespace components {
//=============================================================================
//  Class VerticalFormatter
//=============================================================================
//-----------------------------------------------------------------------------
void VerticalFormatter::setGap(const sd::Coordinate &val) {
	gap = val;
}
//-----------------------------------------------------------------------------
void VerticalFormatter::translateComponent(sdc::AComponentPtr c) {
	c->setLocation(lastPos);
	lastPos = sd::Point2D(lastPos.x() + c->getWidth() + gap, lastPos.y());
	c->redraw();
}
//-----------------------------------------------------------------------------
void VerticalFormatter::setCursor(const sd::Point2D &p) {
	lastPos = p;
}
//-----------------------------------------------------------------------------
void VerticalFormatter::translateCompound() {
	namespace trans = boost::geometry::strategy::transform;
	std::vector<sd::Point2D> _points;
	_points.resize(tmp.size() * 2);
	size_t index = 0;
	BOOST_FOREACH(sdc::AComponentPtr c, tmp) {
		_points[index] = c->getLocation();
		// to get an correct bounding box we need a
		// second point.
		_points[_points.size() - index - 1] = 
			sd::Point2D(c->getX() + c->getWidth(),
			c->getY() + c->getHeight());
		++index;
	}
	// point container
	typedef boost::geometry::model::multi_point<sd::Point2D> Points;
	Points points(_points.begin(), _points.end());
	// calc. bounding box
	sd::Rectangle env;
	boost::geometry::envelope(points, env);
	// translate bunch of points
	trans::translate_transformer<double, 2, 2>
		translate(lastPos.x(), lastPos.y());
	Points res;
	boost::geometry::transform(points, res, translate);
	lastPos = sd::Point2D(lastPos.x() + env.width() + gap, lastPos.y());
	index = 0;
	// reassign points
	BOOST_FOREACH(sdc::AComponentPtr c, tmp) {
		c->setLocation(res[index++]);
		c->redraw();
	}
	// reset
	numCompound = 0;
	tmp.clear();
}
//-----------------------------------------------------------------------------
void VerticalFormatter::addElement(sdc::AComponentPtr c) {
	if ((int)tmp.size() < numCompound) {
		tmp.push_back(c);
		if ((int)tmp.size() == numCompound) {
			translateCompound();
		}
		return;
	}
	translateComponent(c);
}
//-----------------------------------------------------------------------------
void VerticalFormatter::resetOrigin(const sd::Point2D &p) {
	lastPos = p;
}
//-----------------------------------------------------------------------------
void VerticalFormatter::setCompoundCounter(size_t num) {
	numCompound = num;
	tmp.reserve(num);
}
}}} // namespace(s)
