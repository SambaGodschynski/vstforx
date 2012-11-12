/*
 * FrxCircuidViewUI.hpp
 *
 *  Created on: Mon Aug 20 12:13:07 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXCIRCUIDVIEWUI_H
#define SAMBAG_FRXCIRCUIDVIEWUI_H

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <sambag/disco/components/ui/AComponentUI.hpp>
#include <sambag/disco/components/Forward.hpp>
#include <sambag/disco/Geometry.hpp>
#include "FrxCircuidMouseListener.hpp"

namespace frx { namespace gui {
namespace components { namespace ui { 
namespace sd = sambag::disco;
namespace sdc = sd::components;
namespace sdcu = sdc::ui;
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
	typedef boost::shared_ptr<FrxCircuidViewUI> Ptr;
	//-------------------------------------------------------------------------
	typedef boost::weak_ptr<FrxCircuidViewUI> WPtr;
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
}; // FrxCircuidViewUI
}}}} // namespace(s)

#endif /* SAMBAG_FRXCIRCUIDVIEWUI_H */
