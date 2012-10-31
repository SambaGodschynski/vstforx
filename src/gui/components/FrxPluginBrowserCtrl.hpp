/*
 * FrxPluginBrowserCtrl.hpp
 *
 *  Created on: Tue Oct 30 15:04:41 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXPLUGINBROWSERCTRL_H
#define SAMBAG_FRXPLUGINBROWSERCTRL_H

#include <boost/shared_ptr.hpp>
#include "FrxProcessorBrowserCtrl.hpp"

namespace frx { namespace gui { namespace components {
//=============================================================================
/** 
  * @class FrxPluginBrowserCtrl.
  */
class FrxPluginBrowserCtrl : public FrxProcessorBrowserCtrl {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxPluginBrowserCtrl> Ptr;
	//-------------------------------------------------------------------------
	typedef FrxProcessorBrowserCtrl Super;
protected:
	//-------------------------------------------------------------------------
	FrxPluginBrowserCtrl(){}
public:
	//-------------------------------------------------------------------------
	static Ptr create() {
		return Ptr(new FrxPluginBrowserCtrl());
	}
	//-------------------------------------------------------------------------
	virtual void initTree(FrxCircuidViewPtr view, FrxColumnBrowserPtr brws);
}; // FrxPluginBrowserCtrl
}}} // namespace(s)

#endif /* SAMBAG_FRXPLUGINBROWSERCTRL_H */
