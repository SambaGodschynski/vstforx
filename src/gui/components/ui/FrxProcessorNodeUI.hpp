/*
 * FrxProcessorNodeUI.hpp
 *
 *  Created on: Mon Aug 20 10:46:06 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXPROCESSORNODEUI_H
#define SAMBAG_FRXPROCESSORNODEUI_H

#include <boost/shared_ptr.hpp>
#include <gui/components/FrxComponent.hpp>
#include "FrxNodeUI.hpp"

namespace frx { namespace gui {
namespace components { namespace ui { 
namespace sd = sambag::disco;
namespace sdc = sd::components;
namespace sdcu = sdc::ui;
//=============================================================================
/** 
  * @class FrxProcessorNodeUI.
  */
class FrxProcessorNodeUI : public FrxNodeUI {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef FrxNodeUI Super;
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxProcessorNodeUI> Ptr;
protected:
	//-------------------------------------------------------------------------
	FrxProcessorNodeUI(){}
private:
public:
}; // FrxProcessorNodeUI
}}}} // namespace(s)

#endif /* SAMBAG_FRXPROCESSORNODEUI_H */
