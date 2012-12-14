
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

namespace frx { namespace processing {
	class VstForxPlug;
}} // namespace(s)

namespace frx { namespace gui { namespace components {
namespace sd = sambag::disco;
namespace sdc = sd::components;
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
//=============================================================================
class VstForxEditor : 
	public AEffEditor,
	public sambag::dsp::IEditor
{
//=============================================================================
friend class frx::processing::VstForxPlug;
public:
private:
	//-------------------------------------------------------------------------
	sambag::disco::components::WindowPtr window;
	//-------------------------------------------------------------------------
	FrxCircuidViewPtr circView;
	//-------------------------------------------------------------------------
	ERect size;
	//-------------------------------------------------------------------------
	sdc::Window::Ptr createMainWindow(const sd::Rectangle &bounds);
	//-------------------------------------------------------------------------
	FrxCircuidViewPtr createEmptyView();
	//-------------------------------------------------------------------------
	/**
	 * assumes that view is added to window yet.
	 */
	void initEntryExit(FrxCircuidViewPtr view);
	//-------------------------------------------------------------------------
	std::stringstream hiChamber;
	//-------------------------------------------------------------------------
	sambag::com::RecursiveMutex mutex;
protected:
	//-------------------------------------------------------------------------
	virtual bool open (void *ptr);
	//-------------------------------------------------------------------------
	virtual void close ();
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
public:
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
	virtual void idle();
};
}}} // namespace(s)
#endif


