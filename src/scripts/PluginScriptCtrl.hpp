/*
 * PluginScriptCtrl.hpp
 *
 *  Created on: Mon Dec 17 10:01:25 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_PLUGINSCRIPTCTRL_H
#define SAMBAG_PLUGINSCRIPTCTRL_H

#include <boost/shared_ptr.hpp>
#include <processing/VstForxPlug.hpp>
#include <gui/components/VstForxEditor.hpp>
#include <sambag/disco/components/Forward.hpp>
#include <boost/thread.hpp>
#include <string>
#include <sambag/lua/Lua.hpp>
#include <list>

namespace frx { namespace scripts {
//=============================================================================
/** 
  * @class PluginScriptCtrl.
  */
class PluginScriptCtrl {
//=============================================================================
public:
protected:
	//-------------------------------------------------------------------------
	void runThread();
private:
	//-------------------------------------------------------------------------
	std::list<std::string> scripts;
	//-------------------------------------------------------------------------
	frx::processing::VstForxPlug *plug;
	//-------------------------------------------------------------------------
	frx::gui::components::VstForxEditor *editor;
	//-------------------------------------------------------------------------
	std::string scriptStr;
	//-------------------------------------------------------------------------
	boost::thread thread;
	//-------------------------------------------------------------------------
	sambag::lua::LuaStateRef luaState;
	//-------------------------------------------------------------------------
	void registerFunctions(sambag::lua::LuaStateRef luaState);
public:
	//-------------------------------------------------------------------------
	void startScriptCall(const std::string &fname="");
	//-------------------------------------------------------------------------
	void endScriptCall();
	//-------------------------------------------------------------------------
	sambag::disco::components::WindowPtr getEditorWindow() const;
	//-------------------------------------------------------------------------
	void start();
	//-------------------------------------------------------------------------
	void join();
	//-------------------------------------------------------------------------
	void addScript(const std::string &str);
	//-------------------------------------------------------------------------
	void setPlugin(frx::processing::VstForxPlug *plug);
	//-------------------------------------------------------------------------
	frx::processing::VstForxPlug * getPlugin() const { return plug; }
	//-------------------------------------------------------------------------
	frx::gui::components::VstForxEditor * getEditor() const { return editor; }
	//-------------------------------------------------------------------------
	PluginScriptCtrl() : plug(NULL), editor(NULL) {}
}; // PluginScriptCtrl
}} // namespace(s)

#endif /* SAMBAG_PLUGINSCRIPTCTRL_H */
