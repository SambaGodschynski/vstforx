/*
 * FrxPluginEditorCtrl.cpp
 *
 *  Created on: Tue Nov  6 10:39:00 2012
 *      Author: Johannes Unger
 */

#include "FrxPluginEditorCtrl.hpp"

namespace frx { namespace gui { namespace components {
//=============================================================================
//  Class FrxPluginEditorCtrl
//=============================================================================
//-----------------------------------------------------------------------------
void FrxPluginEditorCtrl::open(sdc::WindowPtr win) {
	if (!plugin)
		return;
	plugin->openEditor(win);
	idleTimer = sdc::Timer::create(10);
	idleTimer->setNumRepetitions(-1);
	idleTimer->EventSender<sdc::TimerEvent>::addTrackedEventListener(
		boost::bind(&FrxPluginEditorCtrl::onIdleTimer, this, _1, _2),
		win
	);
	idleTimer->start();
}
//-----------------------------------------------------------------------------
void FrxPluginEditorCtrl::close(sdc::WindowPtr win) {
	if (!plugin)
		return;
	plugin->closeEditor(win);
	idleTimer->stop();
}
//-----------------------------------------------------------------------------
void FrxPluginEditorCtrl::setPlugin(Plugin::Ptr plugin) {
	this->plugin = plugin;
}
//-----------------------------------------------------------------------------
void FrxPluginEditorCtrl::onIdleTimer(void *src, const sdc::TimerEvent &ev) {
	if (!plugin)
		return;
	plugin->onEditorIdle();
}
}}} // namespace(s)
