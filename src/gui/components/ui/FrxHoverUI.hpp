/*
 * FrxHoverUI.hpp
 *
 *  Created on: Mon Oct  1 10:41:33 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXHOVERUI_H
#define SAMBAG_FRXHOVERUI_H

#include <boost/shared_ptr.hpp>
#include "FrxSelectionUI.hpp"
#include "FrxHoverMouseListener.hpp"
#include <gui/HandyNamespaces.hpp>

namespace frx { namespace gui {
namespace components { namespace ui { 
//=============================================================================
/** 
  * @class FrxHoverUI.
  */
class FrxHoverUI : public FrxSelectionUI {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxHoverUI> Ptr;
	//-------------------------------------------------------------------------
	typedef FrxSelectionUI Super;
protected:
	//-------------------------------------------------------------------------
	FrxHoverUI(){}
	//-------------------------------------------------------------------------
	virtual void installListeners(sdc::AComponentPtr c);
	//-------------------------------------------------------------------------
	virtual void installDefaults(sdc::AComponentPtr c);
private:
public:
	//-------------------------------------------------------------------------
	static Ptr create();
}; // FrxHoverUI
}}}} // namespace(s)

#endif /* SAMBAG_FRXHOVERUI_H */
