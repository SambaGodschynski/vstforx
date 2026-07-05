/*
 * FrxCircuidViewUI.hpp
 *
 *  Created on: Mon Aug 20 12:13:07 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXCIRCUIDVIEWUI_H
#define SAMBAG_FRXCIRCUIDVIEWUI_H

#include <memory>
#include <sambag/disco/components/ui/AComponentUI.hpp>
#include <sambag/disco/components/Forward.hpp>
#include <sambag/disco/Geometry.hpp>
#include "FrxCircuidMouseListener.hpp"
#include <gui/HandyNamespaces.hpp>

namespace frx { namespace gui {
namespace components { namespace ui { 
//=============================================================================
/** 
  * @class FrxCircuidViewUI.
  */
class FrxCircuidViewUI : public sdcu::AComponentUI {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef sdcu::AComponentUI Super;
	//-------------------------------------------------------------------------
	typedef std::shared_ptr<FrxCircuidViewUI> Ptr;
	//-------------------------------------------------------------------------
	typedef std::weak_ptr<FrxCircuidViewUI> WPtr;
protected:
	//-------------------------------------------------------------------------
	WPtr self;
	//-------------------------------------------------------------------------
	FrxCircuidViewUI();
private:
	//-------------------------------------------------------------------------
	FrxCircuidMouseListener mouseListener;
public:
	//-------------------------------------------------------------------------
	Ptr getPtr() const {
		return self.lock();
	}
	//-------------------------------------------------------------------------
	virtual void installUI(sdc::AComponentPtr c);
	//-------------------------------------------------------------------------
	static Ptr create();
	//virtual void draw(sd::IDrawContext::Ptr cn, sdc::AComponentPtr c);
}; // FrxCircuidViewUI
}}}} // namespace(s)

#endif /* SAMBAG_FRXCIRCUIDVIEWUI_H */
