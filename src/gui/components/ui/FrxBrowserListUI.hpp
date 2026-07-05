/*
 * FrxBrowserListUI.hpp
 *
 *  Created on: Thu Nov 15 20:33:55 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXBROWSERLISTUI_H
#define SAMBAG_FRXBROWSERLISTUI_H

#include <memory>
#include <sambag/disco/components/ui/basic/BasicListUI.hpp>
#include <gui/components/FrxColumnBrowser.hpp>
#include <sambag/disco/svg/Style.hpp>
#include <gui/HandyNamespaces.hpp>
#include <sambag/com/ArithmeticWrapper.hpp>

namespace frx { namespace gui { 
namespace components { namespace ui {
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
	typedef sdcu::basic::BasicListUI<ListType> Super;
	//-------------------------------------------------------------------------
	typedef FrxBrowserListUI Class;
	//-------------------------------------------------------------------------
	typedef std::shared_ptr<Class> Ptr;
	//-------------------------------------------------------------------------
	typedef std::weak_ptr<Class> WPtr;
protected:
	//-------------------------------------------------------------------------
	void installDefaults(sdc::AComponentPtr c);
	//-------------------------------------------------------------------------
	void installListeners(sdc::AComponentPtr c);
private:
	//-------------------------------------------------------------------------
	sds::Style selectedStyle;
	//-------------------------------------------------------------------------
	sambag::com::ArithmeticWrapper<int, -1> currentIndex;
public:
	//-------------------------------------------------------------------------
	void mousePressed(const sdc::events::MouseEvent &ev);
	//-------------------------------------------------------------------------
	void mouseReleased(const sdc::events::MouseEvent &ev);
	//-------------------------------------------------------------------------
	void mouseDragged(const sdc::events::MouseEvent &ev);
	//-------------------------------------------------------------------------
	void onMouse(void *src, const sdc::events::MouseEvent &ev);
	//-------------------------------------------------------------------------
	virtual void installUI(sdc::AComponentPtr c);
	//-------------------------------------------------------------------------
	static Ptr create() {
		Ptr res(new Class());
		res->self = res;
		return res;
	}
}; // FrxBrowserListUI
}}}} // namespace(s)

#endif /* SAMBAG_FRXBROWSERLISTUI_H */
