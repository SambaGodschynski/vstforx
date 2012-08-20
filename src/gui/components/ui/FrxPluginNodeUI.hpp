/*
 * FrxPluginNodeUI.hpp
 *
 *  Created on: Mon Aug 20 12:13:10 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXPLUGINNODEUI_H
#define SAMBAG_FRXPLUGINNODEUI_H

#include <boost/shared_ptr.hpp>
#include "FrxProcessorNodeUI.hpp"

namespace frx { namespace gui {
namespace components { namespace ui { 
namespace sd = sambag::disco;
namespace sdc = sd::components;
namespace sdcu = sdc::ui;
//=============================================================================
/** 
  * @class FrxPluginNodeUI.
  */
class FrxPluginNodeUI : public FrxProcessorNodeUI {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef FrxProcessorNodeUI Super;
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxPluginNodeUI> Ptr;
protected:
	//-------------------------------------------------------------------------
	FrxPluginNodeUI();
private:
public:
	//-------------------------------------------------------------------------
	static Ptr create();
	//-------------------------------------------------------------------------
	virtual bool contains(sdc::AComponentPtr c, const sd::Point2D &p);
	//-------------------------------------------------------------------------
	/**
	 * Configures the specified component appropriately for the look and feel.
	 * @param c
	 */
	virtual void installUI(sdc::AComponentPtr c);
	//-------------------------------------------------------------------------
	/**
	 * Paints the specified component appropriately for the look and feel.
	 * @param cn
	 * @param c
	 */
	virtual void draw(sd::IDrawContext::Ptr cn, sdc::AComponentPtr c);
}; // FrxPluginNodeUI
}}}} // namespace(s)

#endif /* SAMBAG_FRXPLUGINNODEUI_H */
