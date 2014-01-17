/*
 * FrxPluginEditorCtrl.cpp
 *
 *  Created on: Tue Nov  6 10:39:00 2012
 *      Author: Johannes Unger
 */

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
    
    if (!plugin->isBridged()) {
        idleTimer = sdc::Timer::create(10);
        idleTimer->setNumRepetitions(-1);
        idleTimer->sce::EventSender<sdc::TimerEvent>::addTrackedEventListener(
            boost::bind(&FrxPluginEditorCtrl::onIdleTimer, this, _1, _2),
            win);
        idleTimer->start();
    }
}
//-----------------------------------------------------------------------------
void FrxPluginEditorCtrl::close(sdc::WindowPtr win) {
    Plugin::Ptr plugin = getPlugin();
	if (!plugin)
		return;
	__isOpen = false;
	plugin->closeEditor(win);
    if (idleTimer) {
        idleTimer->stop();
    }
}
//-----------------------------------------------------------------------------
void FrxPluginEditorCtrl::setPlugin(Plugin::Ptr plugin) {
	this->_plugin = plugin;
}
//-----------------------------------------------------------------------------
void FrxPluginEditorCtrl::onIdleTimer(void *src, const sdc::TimerEvent &ev) {
    Plugin::Ptr plugin = getPlugin();
	if (!plugin || !__isOpen)
		return;
	plugin->onEditorIdle();
}
//-----------------------------------------------------------------------------
FrxPluginEditorCtrl::~FrxPluginEditorCtrl() {
}
}}} // namespace(s)
