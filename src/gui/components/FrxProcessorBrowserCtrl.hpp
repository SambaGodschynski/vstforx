/*
 * FrxProcessorBrowserCtrl.hpp
 *
 *  Created on: Tue Oct 30 21:04:22 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXPROCESSORBROWSERCTRL_H
#define SAMBAG_FRXPROCESSORBROWSERCTRL_H

#include <boost/shared_ptr.hpp>
#include "IFrxColumnBrowserCtrl.hpp"
namespace frx { namespace gui { namespace components {
//=============================================================================
/** 
  * @class FrxProcessorBrowserCtrl.
  */
class FrxProcessorBrowserCtrl : public IFrxColumnBrowserCtrl {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxProcessorBrowserCtrl> Ptr;
	//-------------------------------------------------------------------------
	typedef IFrxColumnBrowserCtrl Super;
protected:
	//-------------------------------------------------------------------------
	FrxProcessorBrowserCtrl(){}
public:
	//-------------------------------------------------------------------------
	static Ptr create() {
		return Ptr(new FrxProcessorBrowserCtrl());
	}
	//-------------------------------------------------------------------------
	virtual void initTree(FrxCircuidViewPtr view, FrxColumnBrowserPtr brws);
}; // FrxProcessorBrowserCtrl
}}} // namespace(s)

#endif /* SAMBAG_FRXPROCESSORBROWSERCTRL_H */
