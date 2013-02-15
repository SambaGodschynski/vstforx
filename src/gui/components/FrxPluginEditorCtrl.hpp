/*
 * FrxPluginEditorCtrl.hpp
 *
 *  Created on: Tue Nov  6 10:39:00 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXPLUGINEDITORCTRL_H
#define SAMBAG_FRXPLUGINEDITORCTRL_H

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include "IFrxProcessorEditorCtrl.hpp"
#include <processing/IPluginAdapter.hpp>
#include <sambag/disco/components/Timer.hpp>
#include <sambag/com/ArithmeticWrapper.hpp>

namespace frx { namespace gui { namespace components {
//=============================================================================
/** 
  * @class FrxPluginEditorCtrl.
  */
class FrxPluginEditorCtrl : public IFrxProcessorEditorCtrl {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef IFrxProcessorEditorCtrl Super;
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxPluginEditorCtrl> Ptr;
	//-------------------------------------------------------------------------
	typedef boost::weak_ptr<FrxPluginEditorCtrl> WPtr;
protected:
	//-------------------------------------------------------------------------
	WPtr self;
	//-------------------------------------------------------------------------
	typedef frx::processing::IPluginAdapter Plugin;
	//-------------------------------------------------------------------------
	Plugin::Ptr plugin;
	//-------------------------------------------------------------------------
	sdc::Timer::Ptr idleTimer;
private:
	//-------------------------------------------------------------------------
	/**
	 * need to lock closeWindow() against onIdleTimer().
	 * Window::isOpen() is to unsafe for that job (toolkit timer related).
	 */
	sambag::com::ArithmeticWrapper<bool> __isOpen;
public:
	//-------------------------------------------------------------------------
	void onIdleTimer(void *src, const sdc::TimerEvent &ev);
	//-------------------------------------------------------------------------
	static Ptr create() {
		Ptr res(new FrxPluginEditorCtrl());
		res->self = res;
		return res;
	}
	//-------------------------------------------------------------------------
	virtual void open(sdc::WindowPtr win);
	//-------------------------------------------------------------------------
	virtual void close(sdc::WindowPtr win);
	//-------------------------------------------------------------------------
	void setPlugin(Plugin::Ptr plugin);
	//-------------------------------------------------------------------------
	Plugin::Ptr getPlugin() const {
		return plugin;
	}
}; // FrxPluginEditorCtrl
}}} // namespace(s)

#endif /* SAMBAG_FRXPLUGINEDITORCTRL_H */
