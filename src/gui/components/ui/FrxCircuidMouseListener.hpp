/*
 * FrxCircuidViewMouseListener.hpp
 *
 *  Created on: Mon Aug 27 16:30:23 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXCIRCUIDVIEWMOUSELISTENER_H
#define SAMBAG_FRXCIRCUIDVIEWMOUSELISTENER_H

#include <boost/shared_ptr.hpp>
#include <sambag/disco/components/events/MouseEvent.hpp>
#include <sambag/disco/Geometry.hpp>
#include <sambag/disco/svg/graphicElements/Rect.hpp>
#include <sambag/disco/components/ComponentWrapper.hpp>
#include <gui/HandyNamespaces.hpp>

namespace frx { namespace gui {
namespace components { namespace ui { 
//=============================================================================
/** 
  * @class FrxCircuidMouseListener.
  */
class FrxCircuidMouseListener {
//=============================================================================
public:
protected:
private:
	//-------------------------------------------------------------------------
	sd::Point2D clickLoc;
	//-------------------------------------------------------------------------
	sd::Point2D clickLocScreen; // needed for view dragging
	//-------------------------------------------------------------------------
	typedef sdc::ComponentWrapper<sdsg::Rect> Rect;
	//-------------------------------------------------------------------------
	Rect::Ptr selection;
protected:
	// MouseActions on view:
	virtual void drag(const sdc::events::MouseEvent &ev);
	//-------------------------------------------------------------------------
	virtual void beginSpanning(const sdc::events::MouseEvent &ev);
	//-------------------------------------------------------------------------
	virtual void spanning(const sdc::events::MouseEvent &ev);
	//-------------------------------------------------------------------------
	virtual void endSpanning(const sdc::events::MouseEvent &ev);
public:
	//-------------------------------------------------------------------------
	FrxCircuidMouseListener();
	//-------------------------------------------------------------------------
	// MouseEvents
	virtual void mousePressed(const sdc::events::MouseEvent &ev);
	//-------------------------------------------------------------------------
	virtual void mouseReleased(const sdc::events::MouseEvent &ev);
	//-------------------------------------------------------------------------
	virtual void mouseEntered(const sdc::events::MouseEvent &ev);
	//-------------------------------------------------------------------------
	virtual void mouseExited(const sdc::events::MouseEvent &ev);
	//-------------------------------------------------------------------------
	virtual void mouseClicked(const sdc::events::MouseEvent &ev);
	//-------------------------------------------------------------------------
	virtual void mouseMoved(const sdc::events::MouseEvent &ev);
	//-------------------------------------------------------------------------
	virtual void mouseDragged(const sdc::events::MouseEvent &ev);
	//-------------------------------------------------------------------------
	virtual void mouseWheelMoved(const sdc::events::MouseEvent &ev);
	//-------------------------------------------------------------------------
public:
	void onMouse(void *src, const sdc::events::MouseEvent &ev);
}; // FrxCircuidMouseListener
}}}} // namespace(s)

#endif /* SAMBAG_FRXCIRCUIDVIEWMOUSELISTENER_H */
