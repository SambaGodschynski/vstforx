/*
 * FrxHoverMouseListener.hpp
 *
 *  Created on: Mon Oct  1 10:41:49 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXHOVERMOUSELISTENER_H
#define SAMBAG_FRXHOVERMOUSELISTENER_H
#include <sambag/disco/components/Forward.hpp>
#include <memory>
#include "FrxSelectionMouseListener.hpp"
#include <gui/components/FrxHover.hpp>
#include <gui/HandyNamespaces.hpp>

namespace frx { namespace gui {
namespace components { namespace ui { 
//=============================================================================
/** 
  * @class FrxHoverMouseListener.
  */
class FrxHoverMouseListener : public FrxSelectionMouseListener {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef std::shared_ptr<FrxHoverMouseListener> Ptr;
	//-------------------------------------------------------------------------
	typedef FrxSelectionMouseListener Super;
protected:
	//-------------------------------------------------------------------------
	FrxHoverMouseListener();
	//-------------------------------------------------------------------------
	FrxHover::WPtr _hover;
public:
	//-------------------------------------------------------------------------
	void setHover(FrxHover::Ptr hover) {
		this->_hover = hover;
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
