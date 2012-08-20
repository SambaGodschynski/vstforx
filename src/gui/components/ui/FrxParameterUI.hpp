/*
 * FrxParameterUI.hpp
 *
 *  Created on: Mon Aug 20 10:46:01 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXPARAMETERUI_H
#define SAMBAG_FRXPARAMETERUI_H

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
  * @class FrxParameterUI.
  */
class FrxParameterUI : public FrxNodeUI {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef FrxNodeUI Super;
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxParameterUI> Ptr;
protected:
	//-------------------------------------------------------------------------
	FrxParameterUI();
private:
public:
	//-------------------------------------------------------------------------
	static Ptr create();
}; // FrxParameterUI
}}}} // namespace(s)

#endif /* SAMBAG_FRXPARAMETERUI_H */
