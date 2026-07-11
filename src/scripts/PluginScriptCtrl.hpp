/*
 * PluginScriptCtrl.hpp
 *
 *  Created on: Mon Dec 17 10:01:25 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_PLUGINSCRIPTCTRL_H
#define SAMBAG_PLUGINSCRIPTCTRL_H

#include <memory>
#include <gui/components/VstForxEditor.hpp>
#include <sambag/disco/components/Forward.hpp>
#include <gui/components/Forward.hpp>
#include <sambag/com/events/Events.hpp>
#include <sambag/com/Thread.hpp>
#include <thread>
#include <string>
#include <sambag/lua/Lua.hpp>
#include <list>
#include <unordered_map>
#include <sambag/com/ArithmeticWrapper.hpp>
#include <processing/ModelObject.hpp>
#include <map>
#include "LuaUserData.hpp"

namespace frx {

namespace processing {
    class VstForxPlug;
}

namespace scripts {
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
    typedef std::shared_ptr<PluginScriptCtrl> Ptr;
    //-------------------------------------------------------------------------
    typedef std::weak_ptr<PluginScriptCtrl> WPtr;
	//-------------------------------------------------------------------------
	typedef std::string LuaPtr;
    //-------------------------------------------------------------------------
    typedef LuaUserData PersistUserData;
protected:
	//-------------------------------------------------------------------------
	void runThread();
private:
    //-------------------------------------------------------------------------
    PersistUserData persistUserData;
	//-------------------------------------------------------------------------
	sambag::com::ArithmeticWrapper<bool> verbose;
	//-------------------------------------------------------------------------
	sambag::com::ArithmeticWrapper<bool> isPublic;
	//-------------------------------------------------------------------------
	typedef sambag::com::RecursiveMutex Mutex;
    mutable Mutex __scriptCallMutex;
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
	std::thread thread;
	//-------------------------------------------------------------------------
	sambag::lua::LuaStateRef __luaState;
	//-------------------------------------------------------------------------
	typedef frx::gui::components::FrxComponentPtr FrxComponentPtr;
	//-------------------------------------------------------------------------
	typedef frx::processing::ModelObject::Ptr ModelObjectPtr;
	//-------------------------------------------------------------------------
	typedef std::unordered_map<LuaPtr, FrxComponentPtr> ComponentMap;
	ComponentMap componentMap;
	//-------------------------------------------------------------------------
	typedef std::unordered_map<LuaPtr, ModelObjectPtr> ModelObjectMap;
	ModelObjectMap modelObjectMap;
public:
    //-------------------------------------------------------------------------
    const PersistUserData & getPersistUserData() const {
        return persistUserData;
    }
    //-------------------------------------------------------------------------
    PersistUserData & getPersistUserData() {
        return persistUserData;
    }
    //-------------------------------------------------------------------------
	/**
     * @param if is true functions are registered for public purpose
     */
    PluginScriptCtrl(bool isPublic = false);
    //-------------------------------------------------------------------------
	void setPlugin(frx::processing::VstForxPlug *plug);
    //-------------------------------------------------------------------------
    void setEditor(frx::gui::components::VstForxEditor *editor);
    //-------------------------------------------------------------------------
    typedef std::unique_lock<sambag::com::RecursiveMutex> Lock;
    typedef std::shared_ptr<Lock> LockPtr;
    /**
     * @brief a lua state with lock guard, to ensure
     * no async access while external use @see getLuaState()
     */
    typedef std::pair<sambag::lua::LuaStateRef, LockPtr> LuaState;
    LockPtr getLock();
    LuaState getLuaState();
    typedef std::function<void(std::string)> OnExecErrorF;
    typedef std::shared_ptr<void> AnyPtr;
    typedef std::weak_ptr<void> AnyWPtr;
    typedef std::function<AnyPtr()> GetLockObjectF;
    typedef std::tuple<sambag::lua::LuaStateWRef, // lua_state
        GetLockObjectF,                            // getLockObject
        OnExecErrorF,                              // executation fails callbk
        AnyWPtr                                    // signals track
    > LuaProcessor;
	//-------------------------------------------------------------------------
	void registerFunctions(const LuaProcessor &lp,
        bool publicOnly, bool includeView);
    //-------------------------------------------------------------------------
	void __startScriptCall(const std::string &fname="");
	//-------------------------------------------------------------------------
	void __endScriptCall();
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
	/**
	 * executes scriptfile in callers thread.
	 */
	void executeFile(const std::string &path);
	//-------------------------------------------------------------------------
	frx::processing::VstForxPlug * getPlugin() const { return plug; }
	//-------------------------------------------------------------------------
	frx::gui::components::VstForxEditor * getEditor() const { return editor; }
}; // PluginScriptCtrl
}} // namespace(s)

#endif /* SAMBAG_PLUGINSCRIPTCTRL_H */
