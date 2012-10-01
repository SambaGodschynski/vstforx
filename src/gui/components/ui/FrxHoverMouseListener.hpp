/*
 * FrxHoverMouseListener.hpp
 *
 *  Created on: Mon Oct  1 10:41:49 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXHOVERMOUSELISTENER_H
#define SAMBAG_FRXHOVERMOUSELISTENER_H
#include <sambag/disco/components/Forward.hpp>
#include <boost/shared_ptr.hpp>
#include "FrxSelectionMouseListener.hpp"
namespace frx { namespace gui {
namespace components { namespace ui { 
namespace sd = sambag::disco;
namespace sdc = sd::components;
//=============================================================================
/** 
  * @class FrxHoverMouseListener.
  */
class FrxHoverMouseListener : public FrxSelectionMouseListener {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxHoverMouseListener> Ptr;
	//-------------------------------------------------------------------------
	typedef FrxSelectionMouseListener Super;
protected:
	//-------------------------------------------------------------------------
	FrxHoverMouseListener();
	//-------------------------------------------------------------------------
	sdc::AComponentPtr hover;
public:
	//-------------------------------------------------------------------------
	void setHover(sdc::AComponentPtr hover) {
		this->hover = hover;
	}
	//-------------------------------------------------------------------------
	static Ptr create() {
		return Ptr(new FrxHoverMouseListener());
	}
	//-------------------------------------------------------------------------
	// MouseEvents
	virtual void mouseClicked(const sdc::events::MouseEvent &ev);
	//-------------------------------------------------------------------------
	virtual void mouseMoved(const sdc::events::MouseEvent &ev);
	//-------------------------------------------------------------------------
	virtual ~FrxHoverMouseListener();
	//-------------------------------------------------------------------------
	void onMouse(void *src, const sdc::events::MouseEvent &ev);
protected:
private:
public:
}; // FrxHoverMouseListener
}}}} // namespace(s)

#endif /* SAMBAG_FRXHOVERMOUSELISTENER_H */
