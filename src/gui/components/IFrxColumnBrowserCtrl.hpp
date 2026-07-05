/*
 * IFrxColumnBrowserCtrl.hpp
 *
 *  Created on: Tue Oct 30 19:57:57 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_IFRXCOLUMNBROWSERCTRL_H
#define SAMBAG_IFRXCOLUMNBROWSERCTRL_H

#include <memory>
#include "Forward.hpp"

namespace frx { namespace gui { namespace components {
//=============================================================================
/** 
  * @class IFrxColumnBrowserCtrl.
  */
class IFrxColumnBrowserCtrl {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef std::shared_ptr<IFrxColumnBrowserCtrl> Ptr;
protected:
private:
public:
	//-------------------------------------------------------------------------
	virtual void initListeners(FrxColumnBrowserPtr brws) = 0;
	//-------------------------------------------------------------------------
	virtual void initTree(FrxCircuidViewPtr view, FrxColumnBrowserPtr brws) = 0;
	//-------------------------------------------------------------------------
	virtual ~IFrxColumnBrowserCtrl(){}
}; // IFrxColumnBrowserCtrl
}}} // namespace(s)

#endif /* SAMBAG_IFRXCOLUMNBROWSERCTRL_H */
