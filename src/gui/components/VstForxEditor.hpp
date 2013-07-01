
#ifndef DISCO_VSTFORXEDITOR_H
#define DISCO_VSTFORXEDITOR_H

#include <sambag/disco/components/Forward.hpp>

#include <sambag/disco/components/Window.hpp>
#include "gui/components/Forward.hpp"
#include <iostream>
#include <sstream>
#include "aeffeditor.h"
#include <sambag/dsp/IEditor.hpp>
#include <com/Serialization.h>
#include <sambag/com/Thread.hpp>
#include <gui/HandyNamespaces.hpp>

namespace frx { namespace processing {
	class VstForxPlug;
}} // namespace(s)

namespace frx { namespace gui { namespace components {
struct ViewIsReadyEvent {};
//=============================================================================
class VstForxEditor : 
	public AEffEditor,
	public sambag::dsp::IEditor,
    public sce::EventSender<ViewIsReadyEvent>
{
//=============================================================================
friend class frx::processing::VstForxPlug;
public:
private:
	//-------------------------------------------------------------------------
	void onHostWindowOpen(void *src, const sdc::OnOpenEvent &ev);
	//-------------------------------------------------------------------------
	/**
	 * only setted when editor opened via open() ( instead of open(ptr) where
	 * ptr is a native nestedWindow handle given by host )
	 */
	sambag::disco::components::WindowPtr clientWindow;
	//-------------------------------------------------------------------------
	sambag::disco::components::WindowPtr nestedWindow;
	//-------------------------------------------------------------------------
	FrxCircuidViewPtr circView;
	//-------------------------------------------------------------------------
	sdc::Window::Ptr createMainWindow(const sd::Rectangle &bounds);
	//-------------------------------------------------------------------------
	FrxCircuidViewPtr createEmptyView();
	//-------------------------------------------------------------------------
	/**
	 * assumes that view is added to nestedWindow yet.
	 */
	void initEntryExit(FrxCircuidViewPtr view);
	//-------------------------------------------------------------------------
	std::string hiChamber;
	//-------------------------------------------------------------------------
	sambag::com::RecursiveMutex mutex;
	//-------------------------------------------------------------------------
	ERect tmpRect; // to return as ptr. see getRect().
protected:
	//-------------------------------------------------------------------------
	frx::processing::VstForxPlug *plug;
	//-------------------------------------------------------------------------
	/**
	 * serializes view in the case that
	 * editor will be closed and plug(the model) still exists.
	 */
	void serializeViewTemp(::com::oArchive &ar, FrxCircuidViewPtr view);
	//-------------------------------------------------------------------------
	/**
	 * deserializes the view wich was serializeViewTemp.
	 */
	FrxCircuidViewPtr deserializeViewTemp(::com::iArchive &ar);
	//-------------------------------------------------------------------------
	FrxCircuidViewPtr createView(sdc::Window::Ptr win);
	//-------------------------------------------------------------------------
	void setCircuidView(FrxCircuidViewPtr view);
	//-------------------------------------------------------------------------
	/**
	 * later invoked.
	 */
	void _open(void *ptr);
public:
	//-------------------------------------------------------------------------
	void setEditorSize(int width, int height);
	//-------------------------------------------------------------------------
	/**
	 * @return the nestedWindow where the editor is nested in. 
	 * Is NULL when the editor was opened by a host via open(ptr) call.
	 * ( the host produce its own nestedWindow )
	 */
	sambag::disco::components::WindowPtr getHostWindow() const {
		return clientWindow;
	}
	//-------------------------------------------------------------------------
	/**
	 * creates nestedWindow and init editor.
	 */
	void open();
	//-------------------------------------------------------------------------
	/**
	 * init editor on nestedWindow (mainly called by host)
	 * @param raw system handle ptr
	 */
	virtual bool open(void *ptr);
	//-------------------------------------------------------------------------
	virtual void close();
	//-------------------------------------------------------------------------
	FrxCircuidViewPtr getCircuidView() const {
		return circView;
	}
	//-------------------------------------------------------------------------
	static void message(const std::string &str);
	//-------------------------------------------------------------------------
	static void warnMessage(const std::string &str);
	//-------------------------------------------------------------------------
	static void errorMessage(const std::string &str);
	//-------------------------------------------------------------------------
	void setPlugin(frx::processing::VstForxPlug *aEff);
	//-------------------------------------------------------------------------
	frx::processing::VstForxPlug * getPlugin() const { 
		return plug; 
	}
	//-------------------------------------------------------------------------
	VstForxEditor(AudioEffect *ptr);
	//-------------------------------------------------------------------------
	virtual ~VstForxEditor();
	//-------------------------------------------------------------------------
	virtual bool getRect (ERect** rect);
	//-------------------------------------------------------------------------
	sd::Dimension getEditorSize() const;
	//-------------------------------------------------------------------------
	virtual void idle();
};
//=============================================================================
// class CreateDiscoEditor
//=============================================================================
struct CreateVstForxEditor {
	template <class Editor, class Plugin>
	Editor * createEditor(Plugin* plugin) 
	{
		AudioEffect *aEff = plugin;
		frx::gui::components::VstForxEditor *res 
			= new frx::gui::components::VstForxEditor(aEff); 
		res->setPlugin(plugin);
		return res;
	}
};
}}} // namespace(s)
#endif


