/*
 * FrxHoverMouseListener.cpp
 *
 *  Created on: Mon Oct  1 10:41:49 2012
 *      Author: Johannes Unger
 */

#include "FrxHoverMouseListener.hpp"
#include <gui/components/FrxCircuidView.hpp>
#include <gui/components/FrxComponent.hpp>
#include <gui/components/FrxHover.hpp>

namespace frx { namespace gui {
namespace components { namespace ui { 
//=============================================================================
//  Class FrxHoverMouseListener
//=============================================================================
//-----------------------------------------------------------------------------
FrxHoverMouseListener::FrxHoverMouseListener() {
}
//-----------------------------------------------------------------------------
void FrxHoverMouseListener::mouseClicked(const sdc::events::MouseEvent &ev) {
	FrxHover::Ptr hover = _hover.lock();
	if (!hover) {
		return;
	}
	FrxCircuidView::Ptr circ = hover->getFirstContainer<FrxCircuidView>();
	SAMBAG_ASSERT(circ);
	circ->remove(hover);
	hover.reset();
}
//-----------------------------------------------------------------------------
void FrxHoverMouseListener::mouseMoved(const sdc::events::MouseEvent &ev) {
	FrxHover::Ptr hover = _hover.lock();
	if (!hover) {
		return;
	}
	FrxCircuidView::Ptr circ = hover->getFirstContainer<FrxCircuidView>();
	SAMBAG_ASSERT(circ);
	sd::Point2D distance = ev.getLocation();
	boost::geometry::subtract_point(distance, hover->getLocation());
	boost::geometry::add_point(distance, circ->getViewport()->getViewPosition());
	boost::geometry::subtract_point(distance, sd::Point2D(hover->getWidth()/2., 
		hover->getHeight()/2.));
	translateSelection(boost::shared_dynamic_cast<FrxHover>(hover), distance);
	hover->redraw();
}
//-----------------------------------------------------------------------------
void FrxHoverMouseListener::onMouse(void *src, const sdc::events::MouseEvent &ev)
{
	using namespace sdc::events;
	enum { Filter = 
		MouseEvent::DISCO_MOUSE_CLICKED |
		MouseEvent::DISCO_MOUSE_MOVED
	};
	sdc::events::MouseEventSwitch<Filter>::
		delegate(ev, *this);
}
//-----------------------------------------------------------------------------
FrxHoverMouseListener::~FrxHoverMouseListener() {
}
}}}} // namespace(s)
