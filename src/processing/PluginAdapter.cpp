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
#include <sambag/disco/components/WindowToolkit.hpp>
#include <processing/Plugin.h>

/**
 * get the apropriate handler from a window.
 * HWND, WindowRef or NSView
 */
extern void * __getHandlerForVstPlugins_(void*);

namespace frx { namespace processing {
namespace {
void editorSize(const sce::PropertyChanged &ev, sdc::WindowPtr win)
{
    sd::Dimension dim;
    ev.getNewValue(dim);
    if (dim!=NULL_DIMENSION) {
        win->setWindowSize(dim);
    }
}
void editorPosition(const sce::PropertyChanged &ev, sdc::WindowPtr win)
{
    sd::Point2D p;
    ev.getNewValue(p);
    if (p!=NULL_POINT2D) {
        win->setWindowLocation(p);
    }
}
void editorOpenState(const sce::PropertyChanged &ev, sdc::WindowPtr win)
{
    bool open = false;
    ev.getNewValue(open);
    if (open) {
        win->open();
    } else {
        win->close();
    }
}
void onPluginEditorChanged(const sce::PropertyChanged &ev, sdc::WindowWPtr _win)
{
    sdc::WindowPtr win = _win.lock();
    if (!win) {
        return;
    }
    if (ev.getPropertyName() == Plugin::PROPERTY_PARAMETER_EDITOR_SIZE) {
        editorSize(ev, win);
    }
    if (ev.getPropertyName() == Plugin::PROPERTY_PARAMETER_EDITOR_POSITION) {
        editorPosition(ev, win);
    }
    if (ev.getPropertyName() == Plugin::PROPERTY_PARAMETER_EDITOR_OPENSTATE) {
        editorOpenState(ev, win);
    }
}

} // namespace(s)
//=============================================================================
//  Class PluginAdapter
//=============================================================================
//-----------------------------------------------------------------------------
void PluginAdapter::openEditor(sdc::WindowPtr win) {
	if (!win) {
		return;
    }
	sdc::AWindowImpl::Ptr winImpl = win->getWindowImpl();
	if (!winImpl) {
		return;
    }
	Adaptee::Ptr plug = getPlugin();
	if (!plug) {
		return;
    }
    ProcessorAdapter::Adaptee::Ptr pr = getAdaptee(); // processor is property changed ev sender
    if (!pr) {
        return;
    }
    pr->sce::EventSender<sce::PropertyChanged>::addTrackedEventListener(
        boost::bind(&onPluginEditorChanged, _2, sdc::WindowWPtr(win)),
        win
    );
        
	plug->openEditor(
        ::__getHandlerForVstPlugins_(winImpl->getSystemHandle())
    );
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
	std::string res = plug->getPlugName();
    if (isBridged()) {
        res+=" - bridged";
    }
    return res;
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
//-----------------------------------------------------------------------------
sdc::AWindowImplPtr PluginAdapter::getWindowImpl() {
    Adaptee::Ptr plug = getPlugin();
    APluginImpl *impl = plug->getPluginImpl();
    if (!impl) {
        return sdc::AWindowImplPtr();
    }
    return impl->getWindowImpl();
}
//-----------------------------------------------------------------------------
bool PluginAdapter::isBridged() const {
    Adaptee::Ptr plug = getPlugin();
    APluginImpl *impl = plug->getPluginImpl();
    if (!impl) {
        return false;
    }
    return impl->isBridged();
}
}} // namespace(s)
