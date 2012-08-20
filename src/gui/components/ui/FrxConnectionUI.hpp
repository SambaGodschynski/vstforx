/*
 * FrxConnectionUI.hpp
 *
 *  Created on: Mon Aug 20 10:46:11 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXCONNECTIONUI_H
#define SAMBAG_FRXCONNECTIONUI_H

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
  * @class FrxConnectionUI.
  */
class FrxConnectionUI : public FrxComponentUI {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef FrxComponentUI Super;
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxConnectionUI> Ptr;
protected:
	//-------------------------------------------------------------------------
	FrxConnectionUI();
private:
public:
	//-------------------------------------------------------------------------
	static Ptr create();
}; // FrxConnectionUI
}}}} // namespace(s)

#endif /* SAMBAG_FRXCONNECTIONUI_H */
