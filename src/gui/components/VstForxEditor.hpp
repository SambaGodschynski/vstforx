
#ifndef DISCO_VSTFORXEDITOR_H
#define DISCO_VSTFORXEDITOR_H

#include "aeffeditor.h"
#include <sambag/disco/components/Forward.hpp>
#include <processing/VstForxPlug.hpp>
#include <sambag/disco/components/Window.hpp>
#include "gui/components/Forward.hpp"

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
class VstForxEditor : public AEffEditor {
//=============================================================================
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
	FrxCircuidViewPtr createEmptyView(sdc::Window::Ptr win);
	//-------------------------------------------------------------------------
	void initEntryExit(FrxCircuidViewPtr view);
protected:
	//-------------------------------------------------------------------------
	virtual bool open (void *ptr);
	//-------------------------------------------------------------------------
	virtual void close ();
	//-------------------------------------------------------------------------
	frx::processing::VstForxPlug *plug;
public:
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


