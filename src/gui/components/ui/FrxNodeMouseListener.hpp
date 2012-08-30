/*
 * FrxNodeMouseListener.hpp
 *
 *  Created on: Mon Aug 20 16:59:36 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXNODEMOUSELISTENER_H
#define SAMBAG_FRXNODEMOUSELISTENER_H

#include <sambag/disco/components/events/MouseEvent.hpp>
#include <sambag/disco/Geometry.hpp>
#include <sambag/disco/svg/graphicElements/Line.hpp>
#include <sambag/disco/components/ComponentWrapper.hpp>

namespace frx { namespace gui {
namespace components { namespace ui { 
namespace sd = sambag::disco;
namespace sdsg = sd::svg::graphicElements;
namespace sdc = sd::components;
namespace sdcu = sdc::ui; 
//=============================================================================
/** 
  * @class FrxNodeMouseListener.
  */
class FrxNodeMouseListener {
//=============================================================================
public:
protected:
private:
	//-------------------------------------------------------------------------
	sd::Point2D clickLoc;
	//-------------------------------------------------------------------------
	typedef sdc::ComponentWrapper<sdsg::Line> Line;
	//-------------------------------------------------------------------------
	Line::Ptr toConnect;
	//-------------------------------------------------------------------------
protected:
	// MouseActions on object:
	virtual void drag(const sdc::events::MouseEvent &ev);
	//-------------------------------------------------------------------------
	virtual void beginConnecting(const sdc::events::MouseEvent &ev);
	//-------------------------------------------------------------------------
	virtual void connecting(const sdc::events::MouseEvent &ev);
	//-------------------------------------------------------------------------
	virtual void endConnecting(const sdc::events::MouseEvent &ev);
	//-------------------------------------------------------------------------
	virtual void use(const sdc::events::MouseEvent &ev) {}
public:
	//-------------------------------------------------------------------------
	FrxNodeMouseListener();
	//-------------------------------------------------------------------------
	// MouseEvents
	void mousePressed(const sdc::events::MouseEvent &ev);
	//-------------------------------------------------------------------------
	void mouseReleased(const sdc::events::MouseEvent &ev);
	//-------------------------------------------------------------------------
	void mouseEntered(const sdc::events::MouseEvent &ev);
	//-------------------------------------------------------------------------
	void mouseExited(const sdc::events::MouseEvent &ev);
	//-------------------------------------------------------------------------
	void mouseClicked(const sdc::events::MouseEvent &ev);
	//-------------------------------------------------------------------------
	void mouseMoved(const sdc::events::MouseEvent &ev);
	//-------------------------------------------------------------------------
	void mouseDragged(const sdc::events::MouseEvent &ev);
	//-------------------------------------------------------------------------
	void mouseWheelMoved(const sdc::events::MouseEvent &ev);
	//-------------------------------------------------------------------------
public:
	void onMouse(void *src, const sdc::events::MouseEvent &ev);
}; // FrxNodeMouseListener
}}}} // namespace(s)

#endif /* SAMBAG_FRXNODEMOUSELISTENER_H */
