/*
 * FrxCircuidView.hpp
 *
 *  Created on: Mon Aug 20 12:12:59 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXCIRCUIDVIEW_H
#define SAMBAG_FRXCIRCUIDVIEW_H

#include <boost/shared_ptr.hpp>
#include <sambag/disco/components/Forward.hpp>
#include <sambag/disco/components/AContainer.hpp>
#include <sambag/disco/components/Viewport.hpp>
#include <sambag/disco/components/Panel.hpp>
#include <sambag/com/ArbitraryType.hpp>
#include <boost/foreach.hpp>
#include <string>
#include "FrxSelection.hpp"

namespace frx { namespace gui { namespace components {
namespace sc = sambag::com;
namespace sd = sambag::disco;
namespace sdc = sd::components;
namespace sdcu = sdc::ui;
//=============================================================================
/** 
  * @class FrxCircuidView.
  */
class FrxCircuidView : public sdc::Viewport {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef sdc::Viewport Super;
	//-------------------------------------------------------------------------
	typedef float ZOrder;
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxCircuidView> Ptr;
	//-------------------------------------------------------------------------
	virtual sdcu::AComponentUIPtr createComponentUI(sdcu::ALookAndFeelPtr laf) const;
	//-------------------------------------------------------------------------
	static const std::string PROPERTY_ZORDER;
	//-------------------------------------------------------------------------
	static const float Z_ProcessorNodes;
	//-------------------------------------------------------------------------
	static const float Z_IO;
	//-------------------------------------------------------------------------
	static const float Z_Knobs;
	//-------------------------------------------------------------------------
	static const float Z_Wires;
	//-------------------------------------------------------------------------
	static const float Z_OnTop;
	//-------------------------------------------------------------------------
	static const float Z_Default;
	//-------------------------------------------------------------------------
	static const float Z_InteractiveStuff;
protected:
	//-------------------------------------------------------------------------
	sdc::Panel::Ptr content;
	//-------------------------------------------------------------------------
	FrxSelection::Ptr selection;
	//-------------------------------------------------------------------------
	FrxCircuidView();
	//-------------------------------------------------------------------------
	virtual void postConstructor();
	//-------------------------------------------------------------------------
	void handleMousePopup(const sdc::events::MouseEvent &ev);
	//-------------------------------------------------------------------------
	sdc::PopupMenuPtr currPopup;
private:
public:
	//-------------------------------------------------------------------------
	void onMouse(void *src, const sdc::events::MouseEvent &ev);
	//-------------------------------------------------------------------------
	AContainer::Ptr getContentPane() const {
		return content;
	}
	//-------------------------------------------------------------------------
	/**
	 * @note: (Z)Orders is done during insert. So avoid frequently add/remove.
	 */
	virtual void add(sdc::AComponentPtr comp, ZOrder zord = Z_Default);
	//-------------------------------------------------------------------------
	virtual void remove(sdc::AComponentPtr comp);
	//-------------------------------------------------------------------------
	FrxSelection::Ptr getSelection() const { return selection; }
	//-------------------------------------------------------------------------
	SAMBAG_STD_STATIC_COMPONENT_CREATOR(FrxCircuidView)
	//-------------------------------------------------------------------------
	/**
	 * fills (stl-)container with components which passes a filter.
	 * The filter has to be a callable object (overloaded () operator or a
	 * function signature) with an interger result type:
	 * 1: accepted
	 * 0: discarded
	 * any other: cancel searching
	 */
	template <class Container, class Filter>
	void findComponents(Container &container, Filter &f); 
	//-------------------------------------------------------------------------
	/**
	 * search for components that are in given area. The components center has 
	 * to be inside of the area.
	 * @param area
	 * @param z-order start (inclusive)
	 * @param z-order end (inclusive)
	 */
	template <class Container>
	void findComponentsInArea(Container &container, const sd::Rectangle &area,
		ZOrder start = FLT_MIN, 
		ZOrder end = FLT_MAX
	); 
	//-------------------------------------------------------------------------
	/**
	 * search for component on point p.
	 * @param point
	 * @param z-order start (inclusive)
	 * @param z-order end (inclusive)
	 */
	sdc::AComponentPtr findComponentOnPoint(const sd::Point2D &p,
		ZOrder start = FLT_MIN, 
		ZOrder end = FLT_MAX
	); 

}; // FrxCircuidView
///////////////////////////////////////////////////////////////////////////////
template <class Container, class Filter>
void FrxCircuidView::findComponents(Container &container, Filter &filter) 
{
	BOOST_FOREACH(AComponent::Ptr c, getContentPane()->getComponents()) {
		if (filter(c) == 1) {
			container.push_back(c);
			continue;
		}
		if (filter(c) == 0)
			continue;
		// else
		break;
	}
}
//-------------------------------------------------------------------------
template <class Container>
void FrxCircuidView::findComponentsInArea(Container &container, 
	const sd::Rectangle &area, FrxCircuidView::ZOrder _start, 
	FrxCircuidView::ZOrder _end) 
{
	struct Filter {
		const sd::Rectangle &area;
		ZOrder start, end;
		Filter(const sd::Rectangle &area, ZOrder start, ZOrder end) :
		area(area), start(start), end(end) {}
		int operator()( sdc::AComponent::Ptr p ) {
			if (!p)
				return 0;
			ZOrder z = FLT_MIN;
			p->getClientProperty(PROPERTY_ZORDER, z);
			if (z > end)
				return -1;
			if (z < start)
				return 0;
			sd::Point2D loc = p->getLocation();
			boost::geometry::add_point(loc, 
				sd::Point2D(p->getWidth()/2., p->getHeight()/2.)
			);
			return boost::geometry::intersects<sd::Point2D, 
				sd::Rectangle::Base>(loc, area) ? 1 : 0;
		}
	};
	ZOrder start = std::min(_start, _end);
	ZOrder end = std::max(_start, _end);
	findComponents(container, Filter(area, start, end));
}
}}} // namespace(s)

#endif /* SAMBAG_FRXCIRCUIDVIEW_H */
