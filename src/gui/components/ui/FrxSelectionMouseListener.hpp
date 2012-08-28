/*
 * FrxSelectionMouseListener.hpp
 *
 *  Created on: Tue Aug 28 13:22:07 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXSELECTIONMOUSELISTENER_H
#define SAMBAG_FRXSELECTIONMOUSELISTENER_H

#include <boost/shared_ptr.hpp>

#include <sambag/disco/components/events/MouseEvent.hpp>
#include <sambag/disco/Geometry.hpp>

namespace frx { namespace gui {
namespace components { namespace ui { 
namespace sd = sambag::disco;
namespace sdc = sd::components;
//=============================================================================
/** 
  * @class FrxSelectionMouseListener.
  */
class FrxSelectionMouseListener {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxSelectionMouseListener> Ptr;
protected:
private:
public:
protected:
private:
	//-------------------------------------------------------------------------
	sd::Point2D clickLoc;
protected:
	//-------------------------------------------------------------------------
	void drag(const sdc::events::MouseEvent &ev);
public:
	//-------------------------------------------------------------------------
	// MouseEvents
	void mousePressed(const sdc::events::MouseEvent &ev);
	//-------------------------------------------------------------------------
	void mouseDragged(const sdc::events::MouseEvent &ev);
	//-------------------------------------------------------------------------
public:
	//-------------------------------------------------------------------------
	void onMouse(void *src, const sdc::events::MouseEvent &ev);
}; // FrxSelectionMouseListener
}}}} // namespace(s)

#endif /* SAMBAG_FRXSELECTIONMOUSELISTENER_H */
