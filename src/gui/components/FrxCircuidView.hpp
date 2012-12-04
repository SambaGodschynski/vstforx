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
#include <gui/IViewModelMap.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/list.hpp> 
#include <boost/serialization/utility.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/weak_ptr.hpp>
#include <gui/IFrxControl.hpp>
#include "FrxHover.hpp"

namespace frx { namespace gui { namespace components {
namespace sc = sambag::com;
namespace sce = sc::events;
namespace sd = sambag::disco;
namespace sdc = sd::components;
namespace sdcu = sdc::ui;
//=============================================================================
/** 
  * @class FrxCircuidView.
  */
class FrxCircuidView : public sdc::AContainer {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef sdc::AContainer Super;
	//-------------------------------------------------------------------------
	typedef float ZOrder;
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxCircuidView> Ptr;
	//-------------------------------------------------------------------------
	typedef boost::weak_ptr<FrxCircuidView> WPtr;
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
	static const float ZArea_BeginNodes;
	//-------------------------------------------------------------------------
	static const float ZArea_EndNodes;
	//-------------------------------------------------------------------------
	static const float Z_InteractiveStuff;
	//-------------------------------------------------------------------------
	typedef std::pair<FrxComponentPtr, ZOrder> FrxComponentInfo;
	//-------------------------------------------------------------------------
	typedef boost::function<void(int width, int height)> EditorResizeHandler;
protected:
	//-------------------------------------------------------------------------
	void initStatusBar();
	//-------------------------------------------------------------------------
	sdc::Panel::Ptr content;
	//-------------------------------------------------------------------------
	FrxSelection::Ptr selection;
	//-------------------------------------------------------------------------
	FrxCircuidView();
	//-------------------------------------------------------------------------
	virtual void postConstructor();
	//-------------------------------------------------------------------------
	void setStatusMessage(const std::string &txt, const std::string &iconname);
private:
	//-------------------------------------------------------------------------
	EditorResizeHandler rszHandler;
	//-------------------------------------------------------------------------
	std::string usrMsg;
	//-------------------------------------------------------------------------
	sdc::Viewport::Ptr viewPort;
	//-------------------------------------------------------------------------
	sdc::LabelPtr statusMessage;
	///////////////////////////////////////////////////////////////////////////
	// Archive:
	//-------------------------------------------------------------------------
	friend class boost::serialization::access;
	//-------------------------------------------------------------------------
	WPtr tmpSelf;
	//-------------------------------------------------------------------------
	/**
     * we can't serialize "self" directly because it is a AComponent member,
	 * so we need this "trick".
	 */
	template <typename Archive> 
	void serializeSelfPtr(Archive &ar, const unsigned int version) {
		if (Archive::is_saving::value) {
			tmpSelf = boost::shared_dynamic_cast<FrxCircuidView>(self.lock());
		}
		ar & tmpSelf;
		if (Archive::is_loading::value) {
			self = tmpSelf;
			postConstructor();
		}
	}
	//-------------------------------------------------------------------------
	template <typename Archive> 
	void serializeComponents(Archive &ar, const unsigned int version) {
		SAMBAG_ASSERT(getPtr());
		std::list<FrxComponentInfo> l;
		if (Archive::is_saving::value) {
			collectFrxComponentInfo(l);
		}
		ar & l;
		if (Archive::is_loading::value) {
			BOOST_FOREACH(const FrxComponentInfo &i, l) {
				add(i.first, i.second, false);
			}
		}
		l.clear();
	}
	//-------------------------------------------------------------------------
	template <typename Archive> 
	void serialize(Archive &ar, const unsigned int version) {
		serializeSelfPtr(ar, version);
		serializeComponents(ar, version);
	}
public:
	//-------------------------------------------------------------------------
	void setEditorResizeHandler(const EditorResizeHandler &f);
	//-------------------------------------------------------------------------
	const EditorResizeHandler & getEditorResizeHandler() const {
		return rszHandler;
	}
	//-------------------------------------------------------------------------
	void requestEditorResize(const sd::Dimension &size);
	//-------------------------------------------------------------------------
	/**
	 * @return index of order or -1 if not found.
	 */
	int getIndexOf(ZOrder order) const;
	//-------------------------------------------------------------------------
	std::string componentsToString() const;
	//-------------------------------------------------------------------------
	/**
	 * hint messages will be ignored until unset with:
	 *		setUserMessage("").
	 */
	void setUserMessage(const std::string &txt, 
		const std::string &icon = "default");
	//-------------------------------------------------------------------------
	const std::string & getUserMessage() const {
		return usrMsg;
	}
	//-------------------------------------------------------------------------
	sdc::Viewport::Ptr getViewport() {
		return viewPort;
	}
	//-------------------------------------------------------------------------
	sdc::LabelPtr getStatusMessageLabel() const {
		return statusMessage;
	}
	//-------------------------------------------------------------------------
	/**
	 * @return current status bar message
	 */ 
	std::string getStatusMessage() const;
	//-------------------------------------------------------------------------
	/**
	 * will show text as status bar hint message
	 */
	void hintMessage(const std::string &str);
	//-------------------------------------------------------------------------
	/**
	 * will show text as status bar message
	 */
	void message(const std::string &str);
	//-------------------------------------------------------------------------
	/**
	 * will show warning as status bar warning
	 */
	void warnMessage(const std::string &str);
	//-------------------------------------------------------------------------
	/**
	 * will show error as message box
	 */
	void errorMessage(const std::string &str);
	//-------------------------------------------------------------------------
	AContainer::Ptr getContentPane() const {
		return content;
	}
	//-------------------------------------------------------------------------
	/**
	 * @note: (Z)Orders is done during insert. So avoid frequently add/remove.
	 * @param the component
	 * @param the z-order value
	 * @param set whether normalize components location to content pane translation
	 *        values
	 */
	virtual void add(sdc::AComponentPtr comp, 
		ZOrder zord = Z_Default, 
		bool normalizeLocation = false);
	//-------------------------------------------------------------------------
	virtual void remove(sdc::AComponentPtr comp);
	//-------------------------------------------------------------------------
	FrxSelection::Ptr getSelection() const { return selection; }
	//-------------------------------------------------------------------------
	SAMBAG_STD_STATIC_COMPONENT_CREATOR(FrxCircuidView)
	//-------------------------------------------------------------------------
	/**
	 * collects all FrxComponent objects with zorder info.
	 */ 
	template <class FrxComponentInfoContainer>
	void collectFrxComponentInfo(FrxComponentInfoContainer &out) const;
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
	void findComponents(Container &container, Filter &f, 
		int startIndex = 0, int endIndex = -1) const; 
	//-------------------------------------------------------------------------
	template <class Container>
	void findAllComponents(Container &container, ZOrder start, 
		ZOrder end) const; 
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
void FrxCircuidView::findComponents(Container &container, Filter &filter, 
	int startIndex, int endIndex) const
{
	sdc::AContainer::Ptr cnt = getContentPane(); 
	endIndex = endIndex < 0 ? cnt->getComponentCount() : endIndex;
	startIndex = std::max(0, startIndex);
	endIndex = std::min(cnt->getComponentCount(), (size_t)endIndex);
	for (int i=startIndex; i<endIndex; ++i) {
		sdc::AComponentPtr c = cnt->getComponent(i);
		int res = filter(c);
		if (res == 1) {
			container.push_back(c);
			continue;
		}
		if (res == 0)
			continue;
		// else
		break;
	}
}
//-----------------------------------------------------------------------------
template <class Container>
void FrxCircuidView::findAllComponents(Container &container, ZOrder start = FLT_MIN, 
	ZOrder end = FLT_MAX) const
{
	int startIndex = getIndexOf(start);
	sdc::AContainer::Ptr cnt = getContentPane(); 
	startIndex = std::max(0, startIndex);
	for (int i=startIndex; i<cnt->getComponentCount(); ++i) {
		sdc::AComponentPtr c = cnt->getComponent(i);
		ZOrder z = FLT_MIN;
		c->getClientProperty(PROPERTY_ZORDER, z);
		if (z > end)
			return; // stop searching
		container.push_back(c);
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
		ZOrder end;
		Filter(const sd::Rectangle &area, ZOrder end) :
		area(area), end(end) {}
		int operator()( sdc::AComponent::Ptr p ) {
			if (!p)
				return 0;
			ZOrder z = FLT_MIN;
			p->getClientProperty(PROPERTY_ZORDER, z);
			if (z > end)
				return -1; // stop searching
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
	int startIndex = getIndexOf(start);
	Filter filter(area, end);
	findComponents(container, filter, startIndex); // set the endindex isn't really
	                                         // useful because we have to iterate
	                                         // through the elements anyway
}
//-----------------------------------------------------------------------------
template <class FrxComponentInfoContainer>
void FrxCircuidView::collectFrxComponentInfo(FrxComponentInfoContainer &out) const 
{
	BOOST_FOREACH(AComponent::Ptr c, getContentPane()->getComponents()) {
		FrxComponentPtr frxC = 
			boost::shared_dynamic_cast<FrxComponent>(c);
		if (!frxC)
			continue;
		ZOrder z = Z_Default;
		frxC->getClientProperty(PROPERTY_ZORDER, z);
		out.push_back(FrxComponentInfo(frxC, z));
	}
}
}}} // namespace(s)
#endif /* SAMBAG_FRXCIRCUIDVIEW_H */
