/*
 * LuaImpl.hpp
 *
 *  Created on: Wen Feb  7 13:30:58 2014
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_LUAIMPL_H
#define SAMBAG_LUAIMPL_H

#include "PluginImpl.hpp"
#include "LuaImplBase.hpp"
#include <sambag/lua/Lua.hpp>
#include <sambag/lua/LuaMap.hpp>
#include <sambag/lua/LuaHelper.hpp>
#include <sambag/lua/LuaSequence.hpp>
#include <sambag/lua/ALuaObject.hpp>
#include <sambag/com/ArithmeticWrapper.hpp>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <processing/parameter/parameter.h>
#include <loki/Typelist.h>
#include <sambag/com/Thread.hpp>
#include <map>
#include <list>

#define LUA_CALL(_name)                                                        \
struct _name {                                                                 \
    static const char * name() { return #_name ; }                             \
}


namespace frx {
namespace gui { namespace components {
    class FrxScriptPluginEditor;
    typedef boost::shared_ptr<FrxScriptPluginEditor> ScriptPluginEditorPtr;
}}
namespace processing {
namespace oldPr = ::processing;
namespace oldPrPa = ::processing::parameter;
//=============================================================================
/** 
  * @class PluginImpl.
  */
class LuaImpl : public LuaImplBase,
    public APluginImpl
{
//=============================================================================
public:
    //-------------------------------------------------------------------------
    typedef boost::shared_ptr<LuaImpl> Ptr;
    //-------------------------------------------------------------------------
    typedef boost::weak_ptr<LuaImpl> WPtr;
    //-------------------------------------------------------------------------
    typedef std::multimap<std::string, std::string> PersistUserData;
    //-------------------------------------------------------------------------
    struct LuaCall { // frxlLua
        LUA_CALL(lcProcess);
        LUA_CALL(lcProcessMidi);
        LUA_CALL(lcSetAudioConfig);
        LUA_CALL(lcInit);
        LUA_CALL(lcOnSave);
        LUA_CALL(lcOnLoad);
        // num > 32 can violate flags integer bounds @see lcFlags
        // use boost::dynamic_bitset in that case
        typedef LOKI_TYPELIST_6(
            lcProcess,
            lcProcessMidi,
            lcSetAudioConfig,
            lcInit,
            lcOnSave,
            lcOnLoad) List;
    };
    //-------------------------------------------------------------------------
    enum Flag {
        IsValid,
        NeedsReload
    };
protected:
    ///////////////////////////////////////////////////////////////////////////
    // AWindowImpl
    //-------------------------------------------------------------------------
    /**
     * @param 
     * @param the plugin location
     * @param fills container with plugins parameter representations
     */
    LuaImpl(IHostInfo::Ptr hI,
        const std::string &location,
        Parameters *parameters
    );
    //-------------------------------------------------------------------------
    void onReloadScript();
    //-------------------------------------------------------------------------
    void onExecError(const std::string &msg);
private:
    //-------------------------------------------------------------------------
    void updateLuaParameterMap(float value, const std::string &id);
    //-------------------------------------------------------------------------
    std::string argsToString(lua_State *lua);
    //-------------------------------------------------------------------------
    sambag::com::RecursiveMutex logMutex;
    //-------------------------------------------------------------------------
    void closeLua();
    //-------------------------------------------------------------------------
    typedef std::list<std::string> LogHistory;
    LogHistory logHistory;
    //-------------------------------------------------------------------------
    frx::gui::components::ScriptPluginEditorPtr editor;
    //-------------------------------------------------------------------------
    PersistUserData persistUserData;
    mutable std::string stringBuffer;
    //-------------------------------------------------------------------------
    void addToEditor(const std::string &msg);
    //-------------------------------------------------------------------------
    typedef boost::unordered_set<std::string> Callbacks;
	typedef boost::tuple<oldPrPa::Parameter::Ptr,
        oldPrPa::Parameter::Connection,
        Callbacks> ParameterContainer;
	//-------------------------------------------------------------------------
	typedef boost::unordered_map<std::string, ParameterContainer> ParameterMap;
	//-------------------------------------------------------------------------
	ParameterMap parameterMap;
    //-------------------------------------------------------------------------
	sambag::lua::LuaStateRef luaState;
	//-------------------------------------------------------------------------
	void initLuaEnv(sambag::lua::LuaStateRef luaState);
    //-------------------------------------------------------------------------
    std::string scriptFile;
    //-------------------------------------------------------------------------
    std::string scriptName;
    //-------------------------------------------------------------------------
    typedef sambag::lua::LuaMap<std::string, std::string> Config;
    mutable Config config;
    //-------------------------------------------------------------------------
	// lock lua calls
    typedef sambag::com::RecursiveMutex Mutex;
	mutable Mutex mutex;
    typedef boost::unique_lock<sambag::com::RecursiveMutex> Lock;
    typedef boost::shared_ptr<Lock> LockPtr;
    //-------------------------------------------------------------------------
    LockPtr getLock();
    //-------------------------------------------------------------------------
    unsigned int lcFlags;
    //-------------------------------------------------------------------------
    unsigned int flags;
    //-------------------------------------------------------------------------
    size_t numInChannels, numOutChannels, currNumSamples;
    //-------------------------------------------------------------------------
    oldPr::Frames::T **currInChannels, **currOutChannels;
    protected:
    //-------------------------------------------------------------------------
    inline std::string logName() const {
        return scriptName + ": ";
    }
   //-------------------------------------------------------------------------
    void setFlag(Flag aFlag, bool b) {
        if (b) {
            flags |= (1 << aFlag);
        } else {
            flags &= ~(1 << aFlag);
        }
    }
    //-------------------------------------------------------------------------
    bool getFlag(Flag aFlag) const {
        unsigned int mask = (1 << (unsigned int)aFlag);
        return ((flags & mask) == mask);
    }
public:
    //-------------------------------------------------------------------------
    static Ptr create(IHostInfo::Ptr hI,
        const std::string &location,
        Parameters *parameters
    );
    //-------------------------------------------------------------------------
    void scriptFailed(const std::string &msg);
    //-------------------------------------------------------------------------
    void loadScript();
    //-------------------------------------------------------------------------
    void loadParameters();
    //-------------------------------------------------------------------------
    void loadIOs();
    //-------------------------------------------------------------------------
    void initScript();
    //-------------------------------------------------------------------------
    template <class LC>
    bool has() const {
        enum { Flag = Loki::TL::IndexOf<LuaCall::List, LC>::value };
        unsigned int mask = (1 << Flag);
        return ((lcFlags & mask) == mask);
    }
    //-------------------------------------------------------------------------
    void checkFunctions();
    //-------------------------------------------------------------------------
    void onParameterChanged(void *src, float value, const std::string &id);
    //-------------------------------------------------------------------------
    void log(const std::string &msg);
    //-------------------------------------------------------------------------
    void log(const std::string &scope, const std::string &msg);
    //-------------------------------------------------------------------------
    void logErr(const std::string &msg);
    //-------------------------------------------------------------------------
    void logWarn(const std::string &msg);
    //-------------------------------------------------------------------------
    void logTrace(const std::string &msg);
    ///////////////////////////////////////////////////////////////////////////
    // lua2frx impl
    //-------------------------------------------------------------------------
    void log(lua_State *lua) {
        log(argsToString(lua));
    }
    //-------------------------------------------------------------------------
    void logErr(lua_State *lua) {
        logErr(argsToString(lua));
    }
    //-------------------------------------------------------------------------
    void logWarn(lua_State *lua) {
        logWarn(argsToString(lua));
    }
    //-------------------------------------------------------------------------
    void logTrace(lua_State *lua) {
        logTrace(argsToString(lua));
    }
    //-------------------------------------------------------------------------
    void sendMidi(lua_State *lua);
    //-------------------------------------------------------------------------
    LuaFrames getChannel(lua_State *lua, int channel);
    //-------------------------------------------------------------------------
    FFTData fft(lua_State *lua);
    //-------------------------------------------------------------------------
    void setChannel(lua_State *lua);
    //-------------------------------------------------------------------------
    double getSamplePos(lua_State *lua);
    //-------------------------------------------------------------------------
    double getBarStartPos(lua_State *lua);
    //-------------------------------------------------------------------------
    double getPpqPos(lua_State *lua);
    //-------------------------------------------------------------------------
    int getTimeSigNumerator(lua_State *lua);
    //-------------------------------------------------------------------------
    int getTimeSigDenominator(lua_State *lua);
    //-------------------------------------------------------------------------
    double getTempo(lua_State *lua);
    //-------------------------------------------------------------------------
    void setParameterValue(lua_State *lua, const std::string &name, float value);
    //-------------------------------------------------------------------------
    void setParameterDisplay(lua_State *lua, const std::string &name,
        const std::string &value);
    //-------------------------------------------------------------------------
    float getParameterValue(lua_State *lua, const std::string &name);
    //-------------------------------------------------------------------------
    std::string getParameterDisplay(lua_State *lua, const std::string &name);
    //-------------------------------------------------------------------------
    sambag::lua::IgnoreReturn getPersistUserData(lua_State *lua,
        const std::string &key);
    //-------------------------------------------------------------------------
    void setPersistUserData(lua_State *lua);
    //-------------------------------------------------------------------------
    void addParameterListener(lua_State *lua, const std::string &id,
        const std::string &callBack);
    //-------------------------------------------------------------------------
    void removeParameterListener(lua_State *lua, const std::string &id,
        const std::string &callBack);
public:
    //-------------------------------------------------------------------------
    virtual void baseConfigChanged();
    //-------------------------------------------------------------------------
    virtual void turnOff();
    //-------------------------------------------------------------------------
    virtual void turnOn();
    //-------------------------------------------------------------------------
    virtual void openPlugin();
    //-------------------------------------------------------------------------
    virtual void closePlugin();
    //-------------------------------------------------------------------------
    virtual size_t getNumInputChannels() const;
    //-------------------------------------------------------------------------
    virtual size_t getNumOutputChannels() const;
	//-------------------------------------------------------------------------
	/**
	 * @return true, wenn Plugin ueber Editor verfuegt.
	 */
	virtual bool hasEditor() const;
	//-------------------------------------------------------------------------
	virtual void openEditor(sambag::disco::components::WindowPtr win);
	//-------------------------------------------------------------------------
	virtual void closeEditor(sambag::disco::components::WindowPtr win);
	//-------------------------------------------------------------------------
	virtual void onEditorIdle();
    //-------------------------------------------------------------------------
    virtual bool isAccessable() const;
	//-------------------------------------------------------------------------
	/**
	 * @return Anzahl aller Plugin-Programme (aka. Presets)
	 */
	virtual size_t getNumPrograms();
	//-------------------------------------------------------------------------
	/**
	 * @param index
	 * @return Program-Name zu index.
	 */
	virtual std::string getProgramName( size_t index );
	//-------------------------------------------------------------------------
	/**
	 * Aktiviert Program zu index.
	 * @param index
	 */
	virtual void setProgram( size_t index );
	//-------------------------------------------------------------------------
	/**
	 * @return index des akuell gesetzten Program, falls vorhanden. Andernfalls -1.
	 */
	virtual int getProgram();
	//-------------------------------------------------------------------------
	/**
	 * @return true, if plugin can handle MIDI events
	 */
	virtual bool canHandleMidiEvent() const;
	//-------------------------------------------------------------------------
	virtual void processMidiEvents( sambag::dsp::IMidiEvents * events );
	//-------------------------------------------------------------------------
	virtual size_t getInitialDelay() const;
    //-------------------------------------------------------------------------
    /**
     * @note fills out name, isSynth, uid, vendor, type
     */
    virtual void updatePluginInfo (::processing::PluginInfo &inf) const;
    //-------------------------------------------------------------------------
    virtual void processPlugin( oldPr::Frames::T **,
        oldPr::Frames::T **, size_t numSamples);
    //-------------------------------------------------------------------------
    virtual ~LuaImpl();
    //-------------------------------------------------------------------------
    virtual std::pair<size_t, void*> getStateData() const;
    //-------------------------------------------------------------------------
    virtual void setStateData(size_t size, void* data);
    //-------------------------------------------------------------------------
    /**
     * @return true if plugin is bridged.
     */
    virtual bool isBridged() const {
        return false;
    }
    //-------------------------------------------------------------------------
    /**
     * @return true if plugin is bridged.
     */
    virtual bool isInternal() const {
        return true;
    }
    //-------------------------------------------------------------------------
    /**
     * @return WindowImpl if the plugin has its own. Can be NULL. 
     * (Bridged plugins have its own impl.)
     */
    virtual AWindowImplPtr getWindowImpl() {
        return AWindowImplPtr();
    }
}; // PluginImpl
}} // namespace(s)

#endif /* SAMBAG_PLUGINIMPL_H */
