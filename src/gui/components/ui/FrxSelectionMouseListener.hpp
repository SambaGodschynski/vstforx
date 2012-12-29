/*
 * FrxSelectionMouseListener.hpp
 *
 *  Created on: Tue Aug 28 13:22:07 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXSELECTIONMOUSELISTENER_H
#define SAMBAG_FRXSELECTIONMOUSELISTENER_H

#include <boost/shared_ptr.hpp>
#include <gui/components/Forward.hpp>
#include <sambag/disco/components/events/MouseEvent.hpp>
#include <sambag/disco/Geometry.hpp>
#include <gui/HandyNamespaces.hpp>

namespace frx { namespace gui {
namespace components { namespace ui { 
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
	//-------------------------------------------------------------------------
	FrxSelectionMouseListener();
private:
protected:
	//-------------------------------------------------------------------------
	sd::Point2D tmpPoint;
	//-------------------------------------------------------------------------
	void translateSelection(FrxSelectionPtr sel, const sd::Point2D &distance);
	//-------------------------------------------------------------------------
	void moveSelection(const sdc::events::MouseEvent &ev);
public:
	//-------------------------------------------------------------------------
	// MouseEvents
	virtual void mousePressed(const sdc::events::MouseEvent &ev);
	//-------------------------------------------------------------------------
	virtual void mouseDragged(const sdc::events::MouseEvent &ev);
	//-------------------------------------------------------------------------
public:
	//-------------------------------------------------------------------------
	static Ptr create() {
		return Ptr(new FrxSelectionMouseListener());
	}
	//-------------------------------------------------------------------------
	void onMouse(void *src, const sdc::events::MouseEvent &ev);
	//----------------------------------------------------------------------------
	virtual ~FrxSelectionMouseListener(){}
}; // FrxSelectionMouseListener
}}}} // namespace(s)

#endif /* SAMBAG_FRXSELECTIONMOUSELISTENER_H */
