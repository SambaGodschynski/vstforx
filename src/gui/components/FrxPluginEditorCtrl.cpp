/*
 * FrxPluginEditorCtrl.cpp
 *
 *  Created on: Tue Nov  6 10:39:00 2012
 *      Author: Johannes Unger
 */

#include <boost/bind.hpp>
#include "FrxPluginEditorCtrl.hpp"
#include <sambag/com/Common.hpp>

namespace frx { namespace gui { namespace components {
//=============================================================================
//  Class FrxPluginEditorCtrl
//=============================================================================
//-----------------------------------------------------------------------------
void FrxPluginEditorCtrl::open(sdc::WindowPtr win) {
    Plugin::Ptr plugin = getPlugin();
	if (!plugin)
		return;
	
    plugin->openEditor(win);
    __isOpen = true;
    
    if (!plugin->isBridged() && !plugin->isInternal()) {
        idleTimer = sdc::Timer::create(10);
        idleTimer->setNumRepetitions(-1);
        idleTimer->sce::EventSender<sdc::TimerEvent>::addTrackedEventListener(
            boost::bind(&FrxPluginEditorCtrl::onIdleTimer, this, sdc::WindowWPtr(win)),
            self);
        idleTimer->start();
    }
}
//-----------------------------------------------------------------------------
void FrxPluginEditorCtrl::close(sdc::WindowPtr win) {
    Plugin::Ptr plugin = getPlugin();
	if (!plugin)
		return;
	__isOpen = false;
    if (idleTimer) {
        idleTimer->stop();
    }
    plugin->closeEditor(win);
}
//-----------------------------------------------------------------------------
void FrxPluginEditorCtrl::setPlugin(Plugin::Ptr plugin) {
	this->_plugin = plugin;
}
//-----------------------------------------------------------------------------
void FrxPluginEditorCtrl::onIdleTimer(sdc::WindowWPtr _win) {
    Plugin::Ptr plugin = getPlugin();
    sdc::WindowPtr win = _win.lock();
	if (!plugin || !win || !__isOpen) {
		return;
    }
	plugin->onEditorIdle();
}
//-----------------------------------------------------------------------------
FrxPluginEditorCtrl::~FrxPluginEditorCtrl() {
}
}}} // namespace(s)
