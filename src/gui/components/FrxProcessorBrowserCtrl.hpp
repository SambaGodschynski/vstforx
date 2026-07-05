/*
 * FrxProcessorBrowserCtrl.hpp
 *
 *  Created on: Tue Oct 30 21:04:22 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXPROCESSORBROWSERCTRL_H
#define SAMBAG_FRXPROCESSORBROWSERCTRL_H

#include <memory>
#include "FrxMainBrowserCtrl.hpp"
#include "Forward.hpp"

namespace frx { namespace gui { namespace components {
//=============================================================================
/** 
  * @class FrxProcessorBrowserCtrl.
  */
class FrxProcessorBrowserCtrl : public FrxMainBrowserCtrl {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef std::shared_ptr<FrxProcessorBrowserCtrl> Ptr;
	//-------------------------------------------------------------------------
	typedef FrxMainBrowserCtrl Super;
protected:
	//-------------------------------------------------------------------------
	FrxProcessorBrowserCtrl(){}
	//-------------------------------------------------------------------------
	FrxComponentPtr component;
public:
	//-------------------------------------------------------------------------
	void setComponent(FrxComponentPtr c);
	//-------------------------------------------------------------------------
	FrxComponentPtr getComponent() const;
	//-------------------------------------------------------------------------
	static Ptr create() {
		Ptr res(new FrxProcessorBrowserCtrl());
		res->self = res;
		return res;
	}
	//-------------------------------------------------------------------------
	virtual void initTree(FrxCircuidViewPtr view, FrxColumnBrowserPtr brws);
}; // FrxProcessorBrowserCtrl
}}} // namespace(s)

#endif /* SAMBAG_FRXPROCESSORBROWSERCTRL_H */
