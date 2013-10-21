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
#include <sambag/com/Thread.hpp>
#include <string>
#include <sambag/lua/Lua.hpp>
#include <list>
#include <boost/unordered_map.hpp>
#include <sambag/com/ArithmeticWrapper.hpp>
#include <processing/ModelObject.hpp>

namespace frx { namespace scripts {
struct ScriptExeFailedEvent{
    std::string errMsg;
    ScriptExeFailedEvent(const std::string &msg="unknown error") : errMsg(msg) {}
};
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
	typedef std::string LuaPtr;
protected:
	//-------------------------------------------------------------------------
	void runThread();
private:
	//-------------------------------------------------------------------------
	sambag::com::ArithmeticWrapper<bool> verbose;
	//-------------------------------------------------------------------------
	sambag::com::Mutex scriptCallMutex;
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
	typedef frx::processing::ModelObject::Ptr ModelObjectPtr;
	//-------------------------------------------------------------------------
	typedef boost::unordered_map<LuaPtr, FrxComponentPtr> ComponentMap;
	ComponentMap componentMap;
	//-------------------------------------------------------------------------
	typedef boost::unordered_map<LuaPtr, ModelObjectPtr> ModelObjectMap;
	ModelObjectMap modelObjectMap;
public:
	//-------------------------------------------------------------------------
	void setVerbose(bool val) { verbose = val; }
	//-------------------------------------------------------------------------
	bool isVerbose() const { return verbose; }
	//-------------------------------------------------------------------------
	LuaPtr getLuaPtr(FrxComponentPtr c);
	//-------------------------------------------------------------------------
	FrxComponentPtr getFrxComponent(const LuaPtr &c);
	//-------------------------------------------------------------------------
	LuaPtr getLuaPtr(ModelObjectPtr c);
	//-------------------------------------------------------------------------
	ModelObjectPtr getModelObject(const LuaPtr &c);
	//-------------------------------------------------------------------------
	void remove(const LuaPtr &c);
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
	/**
	 * appends job script. runs in seperate thread, every
	 * failure will abort the job thread.
	 * TODO: will only process scripts which was appended before
	 * start().
	 */
	void appendJob(const std::string &str);
	//-------------------------------------------------------------------------
	/**
	 * executes scripts in callers thread.
	 */
	void execute(const std::string &str);
	//-------------------------------------------------------------------------
	void setPlugin(frx::processing::VstForxPlug *plug);
	//-------------------------------------------------------------------------
	frx::processing::VstForxPlug * getPlugin() const { return plug; }
	//-------------------------------------------------------------------------
	frx::gui::components::VstForxEditor * getEditor() const { return editor; }
	//-------------------------------------------------------------------------
	PluginScriptCtrl();
}; // PluginScriptCtrl
}} // namespace(s)

#endif /* SAMBAG_PLUGINSCRIPTCTRL_H */
