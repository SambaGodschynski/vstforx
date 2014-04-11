/*
 * PluginScriptCtrl.hpp
 *
 *  Created on: Mon Dec 17 10:01:25 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_PLUGINSCRIPTCTRL_H
#define SAMBAG_PLUGINSCRIPTCTRL_H

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
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
#include <map>
#include <com/Serialization.h>
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
    typedef boost::shared_ptr<PluginScriptCtrl> Ptr;
    //-------------------------------------------------------------------------
    typedef boost::weak_ptr<PluginScriptCtrl> WPtr;
	//-------------------------------------------------------------------------
	typedef std::string LuaPtr;
    //-------------------------------------------------------------------------
    typedef std::multimap<std::string, std::string> PersistUserData;
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
    typedef boost::shared_ptr<Mutex> MutexPtr;
    typedef boost::weak_ptr<Mutex> MutexWPtr;
    Mutex __scriptCallMutex;
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
	sambag::lua::LuaStateRef __luaState;
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
    const PersistUserData & getPersistUserData() const {
        return persistUserData;
    }
    //-------------------------------------------------------------------------
    PersistUserData & getPersistUserData() {
        return persistUserData;
    }
    //-------------------------------------------------------------------------
    sambag::com::RecursiveMutex & getMutex() {
        return __scriptCallMutex;
    }
	//-------------------------------------------------------------------------
	/**
     * @param if is true functions are registered for public purpose
     */
    PluginScriptCtrl(bool isPublic = false);
    //-------------------------------------------------------------------------
	void setPlugin(frx::processing::VstForxPlug *plug);
    //-------------------------------------------------------------------------
    typedef boost::unique_lock<sambag::com::RecursiveMutex> Lock;
    typedef boost::shared_ptr<Lock> LockPtr;
    /**
     * @brief a lua state with lock guard, to ensure
     * no async access while external use @see getLuaState()
     */
    typedef std::pair<sambag::lua::LuaStateRef, LockPtr> LuaState;
    LuaState getLuaState();
    typedef boost::function<void(std::string)> OnExecErrorF;
    typedef boost::tuple<sambag::lua::LuaStateWRef, Mutex*, OnExecErrorF> LuaProcessor;
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
