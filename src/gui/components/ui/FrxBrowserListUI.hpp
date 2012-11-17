/*
 * FrxBrowserListUI.hpp
 *
 *  Created on: Thu Nov 15 20:33:55 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXBROWSERLISTUI_H
#define SAMBAG_FRXBROWSERLISTUI_H

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <sambag/disco/components/ui/basic/BasicListUI.hpp>
#include <gui/components/FrxColumnBrowser.hpp>
namespace frx { namespace gui { 
namespace components { namespace ui {
namespace sd = sambag::disco;
namespace sdc = sd::components;
namespace sdcu = sdc::ui;
//=============================================================================
/** 
  * @class FrxBrowserListUI.
  * Because of the "rubber-stamp" like implementation of list entries,
  * we need an extra listui to realize parameter label controlls.
  */
class FrxBrowserListUI : 
	public sdcu::basic::BasicListUI<FrxColumnBrowser::BrowserImpl::ListType> 
{
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef FrxColumnBrowser::BrowserImpl::ListType ListType;
	//-------------------------------------------------------------------------
	typedef FrxBrowserListUI Class;
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<Class> Ptr;
	//-------------------------------------------------------------------------
	typedef boost::weak_ptr<Class> WPtr;
protected:
private:
public:
	//-------------------------------------------------------------------------
	static Ptr create() {
		Ptr res(new Class());
		res->self = res;
		return res;
	}
}; // FrxBrowserListUI
}}}} // namespace(s)

#endif /* SAMBAG_FRXBROWSERLISTUI_H */
