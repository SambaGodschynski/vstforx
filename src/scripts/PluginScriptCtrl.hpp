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
#include <gui/components/Forward.hpp>
#include <sambag/com/events/Events.hpp>
#include <boost/thread.hpp>
#include <string>
#include <sambag/lua/Lua.hpp>
#include <list>
#include <boost/unordered_map.hpp>
#include <sambag/com/ArithmeticWrapper.hpp>

namespace frx { namespace scripts {
struct ScriptExeFailedEvent{};
struct ScriptEnded{};
//=============================================================================
/** 
  * @class PluginScriptCtrl.
  */
class PluginScriptCtrl :
	public sambag::com::events::EventSender<ScriptExeFailedEvent>,
	public sambag::com::events::EventSender<ScriptEnded>
{
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef std::string LuaFrxComponent;
protected:
	//-------------------------------------------------------------------------
	void runThread();
private:
	//-------------------------------------------------------------------------
	sambag::com::ArithmeticWrapper<bool> verbose;
	//-------------------------------------------------------------------------
	std::string lastCall;
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
	//-------------------------------------------------------------------------
	typedef frx::gui::components::FrxComponentPtr FrxComponentPtr;
	//-------------------------------------------------------------------------
	typedef boost::unordered_map<LuaFrxComponent, FrxComponentPtr> ComponentMap;
	ComponentMap componentMap;
public:
	//-------------------------------------------------------------------------
	void setVerbose(bool val) { verbose = val; }
	//-------------------------------------------------------------------------
	bool isVerbose() const { return verbose; }
	//-------------------------------------------------------------------------
	LuaFrxComponent getLuaFrxComponent(FrxComponentPtr c);
	//-------------------------------------------------------------------------
	FrxComponentPtr getFrxComponent(const LuaFrxComponent &c);
	//-------------------------------------------------------------------------
	void remove(const LuaFrxComponent &c);
	//-------------------------------------------------------------------------
	/**
	 * @return last called function name (lua->this)
	 */
	const std::string & getLastCall() { return lastCall; }
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
