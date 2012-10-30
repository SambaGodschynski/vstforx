/*
 * FrxPluginBrowserCtrl.hpp
 *
 *  Created on: Tue Oct 30 15:04:41 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXPLUGINBROWSERCTRL_H
#define SAMBAG_FRXPLUGINBROWSERCTRL_H

#include <boost/shared_ptr.hpp>
#include "IFrxColumnBrowserCtrl.hpp"
#include <processing/IHostInfo.h>

namespace frx { namespace gui { namespace components {
//=============================================================================
/** 
  * @class FrxPluginBrowserCtrl.
  */
class FrxPluginBrowserCtrl : public IFrxColumnBrowserCtrl {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxPluginBrowserCtrl> Ptr;
	//-------------------------------------------------------------------------
	typedef ::frx::processing::IHostInfo IHostInfo;
	//-------------------------------------------------------------------------
	typedef IFrxColumnBrowserCtrl Super;
protected:
	//-------------------------------------------------------------------------
	FrxPluginBrowserCtrl(){}
	//-------------------------------------------------------------------------
	IHostInfo::Ptr hostInfo;
public:
	//-------------------------------------------------------------------------
	IHostInfo::Ptr getHostInfo() const {
		return hostInfo;
	}
	//-------------------------------------------------------------------------
	void setHostInfo(IHostInfo::Ptr hostInfo);
	//-------------------------------------------------------------------------
	static Ptr create() {
		return Ptr(new FrxPluginBrowserCtrl());
	}
	//-------------------------------------------------------------------------
	virtual void initTree(FrxCircuidViewPtr view, FrxColumnBrowserPtr brws);
}; // FrxPluginBrowserCtrl
}}} // namespace(s)

#endif /* SAMBAG_FRXPLUGINBROWSERCTRL_H */
