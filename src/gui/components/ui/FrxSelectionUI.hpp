/*
 * FrxSelectionUI.hpp
 *
 *  Created on: Tue Aug 28 10:47:37 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXSELECTIONUI_H
#define SAMBAG_FRXSELECTIONUI_H

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <sambag/disco/components/ui/AComponentUI.hpp>
#include "FrxSelectionMouseListener.hpp"
#include <sambag/disco/svg/graphicElements/Style.hpp>
#include <gui/HandyNamespaces.hpp>

namespace frx { namespace gui {
namespace components { namespace ui { 
//=============================================================================
/** 
  * @class FrxSelectionUI.
  */
class FrxSelectionUI : public sdcu::AComponentUI {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef sdcu::AComponentUI Super;
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxSelectionUI> Ptr;
	//-------------------------------------------------------------------------
	typedef boost::weak_ptr<FrxSelectionUI> WPtr;
protected:
	//-------------------------------------------------------------------------
	// to be found in FrxCircuidMouseListener's constructor:
	// sd::svg::graphicElements::Style selectingStyle;
	//-------------------------------------------------------------------------
	sd::svg::graphicElements::Style selectedStyle;
	//-------------------------------------------------------------------------
	WPtr self;
	//-------------------------------------------------------------------------
	FrxSelectionUI(){}
	//-------------------------------------------------------------------------
	virtual void installListeners(sdc::AComponentPtr c);
	//-------------------------------------------------------------------------
	virtual void installDefaults(sdc::AComponentPtr c);
	//-------------------------------------------------------------------------
	virtual void installUI(sdc::AComponentPtr c);
	//-------------------------------------------------------------------------
	FrxSelectionMouseListener::Ptr mouseListener;
public:
	//-------------------------------------------------------------------------
	Ptr getPtr() {
		return self.lock();
	}
	//-------------------------------------------------------------------------
	static Ptr create() {
		Ptr res(new FrxSelectionUI());
		res->self = res;
		return res;
	}
	//-------------------------------------------------------------------------
	virtual void draw(sd::IDrawContext::Ptr cn, sdc::AComponentPtr c);
	//-------------------------------------------------------------------------
}; // FrxSelectionUI
}}}} // namespace(s)

#endif /* SAMBAG_FRXSELECTIONUI_H */
