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
#include <sambag/disco/components/WindowToolkit.hpp>
#include <gui/HandyNamespaces.hpp>
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

void parToSrc(const sd::Dimension &win,
    const sd::Dimension &screen, sd::Point2D &p)
{
    p.x(p.x() * (screen.width()+win.width()) - win.width());
    p.y(p.y() * (screen.height()+win.height()) - win.height());
}

void scrToPar(const sd::Dimension &win,
    const sd::Dimension &screen, sd::Point2D &p)
{
    double x = ((screen.width()+win.width())/(win.width() + p.x()));
    double y = ((screen.height()+win.height())/(win.height() + p.y()));
    if (x==0 || y==0) {
        p.x(0);
        p.y(0);
        return;
    }
    p.x(1/x);
    p.y(1/y);
}

/*
 * will be called when editor pos parameter changed
 */
void editorPosition(const sce::PropertyChanged &ev, sdc::WindowPtr win)
{

    bool boundsChanged = false;
    win->getClientProperty("editorBoundsChanged", boundsChanged);
    if (boundsChanged) {
        return;
    }
    sd::Point2D p;
    ev.getNewValue(p);
    sd::Dimension screen = sdc::getWindowToolkit()->getScreenSize();
    sd::Dimension winSize = win->getWindowSize();
    parToSrc(winSize, screen, p);
    
    if (p!=NULL_POINT2D) {
        win->setWindowLocation(p);
    }
}
/*
 * will be called when editor was moved
 */
void editorBoundsChanged(Plugin *plugin, sdc::WindowWPtr _win)
{
    sdc::WindowPtr win = _win.lock();
    if (!win) {
        return;
    }
    win->putClientProperty("editorBoundsChanged", true);
    sd::Point2D p = win->getWindowLocation();
    sd::Dimension screen = sdc::getWindowToolkit()->getScreenSize();
    sd::Dimension winSize = win->getWindowSize();
    scrToPar(winSize, screen, p);
    plugin->getEditorPosX()->setValue(p.x());
    plugin->getEditorPosY()->setValue(p.y());
    win->putClientProperty("editorBoundsChanged", false);
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
}

} // namespace(s)
//=============================================================================
//  Class PluginAdapter
//=============================================================================
//-----------------------------------------------------------------------------
void initPos(Plugin *plugin, std::pair<double, double> x) {
    plugin->getEditorPosX()->setValue(x.first);
    plugin->getEditorPosY()->setValue(x.second);
}
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
    sd::Dimension screen = sdc::getWindowToolkit()->getScreenSize();
    sd::Dimension winSize = win->getWindowSize();
    std::pair<double, double> p(plug->getEditorPosX()->getValue(),
        plug->getEditorPosY()->getValue());
    sdc::getWindowToolkit()->invokeLater(
        boost::bind(&initPos, plug.get(), p), 100, plug
    );
    pr->sce::EventSender<sce::PropertyChanged>::addTrackedEventListener(
        boost::bind(&onPluginEditorChanged, _2, sdc::WindowWPtr(win)),
        win
    );
    
    win->getWindowImpl()->sce::EventSender<sdc::OnBoundsChanged>::addTrackedEventListener(
        boost::bind(&editorBoundsChanged, getPlugin().get(), sdc::WindowWPtr(win)),
        pr
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
std::string PluginAdapter::getLocation() const {
	Adaptee::Ptr plug = getPlugin();
	return plug->getPluginImpl()->location;
    
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
