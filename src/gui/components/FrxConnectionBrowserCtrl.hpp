/*
 * FrxConnectionBrowserCtrl.hpp
 *
 *  Created on: Fri Dec  7 14:45:52 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXCONNECTIONBROWSERCTRL_H
#define SAMBAG_FRXCONNECTIONBROWSERCTRL_H

#include <boost/shared_ptr.hpp>
#include "FrxMainBrowserCtrl.hpp"
namespace frx { namespace gui { namespace components {
//=============================================================================
/** 
  * @class FrxConnectionBrowser.
  */
class FrxConnectionBrowserCtrl : public FrxMainBrowserCtrl {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxConnectionBrowserCtrl> Ptr;
	//-------------------------------------------------------------------------
	typedef FrxMainBrowserCtrl Super;
protected:
	//-------------------------------------------------------------------------
	FrxConnectionBrowserCtrl(){}
	//-------------------------------------------------------------------------
	FrxComponentPtr component;
private:
public:
	//-------------------------------------------------------------------------
	void setComponent(FrxComponentPtr c);
	//-------------------------------------------------------------------------
	FrxComponentPtr getComponent() const;
	//-------------------------------------------------------------------------
	static Ptr create() {
		return Ptr(new FrxConnectionBrowserCtrl());
	}
	//-------------------------------------------------------------------------
	virtual void initTree(FrxCircuidViewPtr view, FrxColumnBrowserPtr brws);
}; // FrxConnectionBrowser
}}} // namespace(s)

#endif /* SAMBAG_FRXCONNECTIONBROWSERCTRL_H */
