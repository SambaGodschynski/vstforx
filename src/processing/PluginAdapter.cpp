/*
 * PluginAdapter.cpp
 *
 *  Created on: Wed Oct 31 15:02:15 2012
 *      Author: Johannes Unger
 */

#include "PluginAdapter.hpp"
#include "NodeAdapter.hpp"
#include "ParameterAdapter.hpp"
#include <sambag/disco/components/Window.hpp>

namespace frx { namespace processing {
namespace {
namespace pr = ::processing;
void onPluginEditorResize(void *, const pr::ResizeEditorEvent &ev, 
	sdc::WindowWPtr _win)
{
	sdc::WindowPtr win = _win.lock();
	if (!win)
		return;
	win->setWindowSize(
		sd::Dimension(
			(sd::Coordinate)ev.w, 
			(sd::Coordinate)ev.h
		)
	);
}
void onPluginEditorRepos(void *, const pr::EditorPositionEvent &ev, 
	sdc::WindowWPtr _win)
{
	sdc::WindowPtr win = _win.lock();
	if (!win)
		return;
}
void onPluginEditorOpenParameter(void *, const pr::EditorOpenParameterChanged &ev, 
	sdc::WindowWPtr _win)
{
	sdc::WindowPtr win = _win.lock();
	if (!win)
		return;
}

} // namespace(s)
//=============================================================================
//  Class PluginAdapter
//=============================================================================
//-----------------------------------------------------------------------------
void PluginAdapter::openEditor(sdc::WindowPtr win) {
	if (!win)
		return;
	sdc::AWindowImpl::Ptr winImpl = win->getWindowImpl();
	if (!winImpl)
		return;
	Adaptee::Ptr plug = getPlugin();
	if (!plug)
		return;

	plug->com::events::EventSender<pr::ResizeEditorEvent>::addTrackedEventListener(
		boost::bind(&onPluginEditorResize, _1, _2, sdc::WindowWPtr(win)),
		win
	);

	plug->openEditor(winImpl->getSystemHandle());
}
//-----------------------------------------------------------------------------
void PluginAdapter::closeEditor(sdc::WindowPtr win) {
	if (!win)
		return;
	sdc::AWindowImpl::Ptr winImpl = win->getWindowImpl();
	if (!winImpl)
		return;
	Adaptee::Ptr plug = getPlugin();
	if (!plug)
		return;
	plug->closeEditor(winImpl->getSystemHandle());
}
//-----------------------------------------------------------------------------
bool PluginAdapter::hasEditor() const {
    Adaptee::Ptr plug = getPlugin();
    if (!plug) {
        return false;
    }
    return plug->hasEditor();
}
//-----------------------------------------------------------------------------
void PluginAdapter::onEditorIdle() {
	Adaptee::Ptr plug = getPlugin();
	if (!plug)
		return;
	plug->onEditorIdle();
}
//-----------------------------------------------------------------------------
std::string PluginAdapter::getName() const {
	Adaptee::Ptr plug = getPlugin();
	return plug->getPlugName();
}
//-----------------------------------------------------------------------------
std::string PluginAdapter::getStatusMessage() const {
	Adaptee::Ptr plug = getPlugin();
	return plug->getStatusMsg();
}
//-----------------------------------------------------------------------------
bool PluginAdapter::isSynth() const {
	Adaptee::Ptr plug = getPlugin();
	return plug->isSynth();
}
//-----------------------------------------------------------------------------
size_t PluginAdapter::getNumPresets() const {
	Adaptee::Ptr plug = getPlugin();
	return plug->getNumPrograms();
}
//-----------------------------------------------------------------------------
std::string PluginAdapter::getPresetName(size_t i) const {
	Adaptee::Ptr plug = getPlugin();
	return plug->getProgramName(i);
}
//-----------------------------------------------------------------------------
void PluginAdapter::setPreset(int i) {
	Adaptee::Ptr plug = getPlugin();
	plug->setProgram(i);
}
}} // namespace(s)
