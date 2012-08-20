/*
 * FrxNodeUI.hpp
 *
 *  Created on: Mon Aug 20 10:46:25 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXNODEUI_H
#define SAMBAG_FRXNODEUI_H

#include <boost/shared_ptr.hpp>
#include <gui/components/FrxComponent.hpp>
#include "FrxComponentUI.hpp"

namespace frx { namespace gui {
namespace components { namespace ui { 
namespace sd = sambag::disco;
namespace sdc = sd::components;
namespace sdcu = sdc::ui;
//=============================================================================
/** 
  * @class FrxNodeUI.
  */
class FrxNodeUI : public FrxComponentUI {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef FrxComponentUI Super;
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxNodeUI> Ptr;
protected:
	//-------------------------------------------------------------------------
	FrxNodeUI(){}
private:
public:
}; // FrxNodeUI
}}}} // namespace(s)

#endif /* SAMBAG_FRXNODEUI_H */
