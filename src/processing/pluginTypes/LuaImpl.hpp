/*
 * LuaImpl.hpp
 *
 *  Created on: Wen Feb  7 13:30:58 2014
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_LUAIMPL_H
#define SAMBAG_LUAIMPL_H

#include "PluginImpl.hpp"
#include <sambag/lua/Lua.hpp>
#include <sambag/lua/LuaMap.hpp>
#include <sambag/lua/LuaHelper.hpp>
#include <sambag/lua/LuaSequence.hpp>
#include <sambag/com/ArithmeticWrapper.hpp>
#include <boost/unordered_map.hpp>
#include <processing/parameter/parameter.h>

#define FRX_LUA_FUNC(_name,r1) \
struct _name ## _Tag { \
    typedef boost::function<r1()> Function; \
    static const char * name() { return #_name ; } \
}

#define FRX_LUA_REG(name, impl) \
    sambag::lua::registerFunction<name ## _Tag>( \
		luaState.get(), \
		boost::bind(&LuaImpl::impl, this) \
	)

#define FRX_LUA_FUNC_1(_name,r1,a1) \
struct _name ## _Tag { \
    typedef boost::function<r1(a1)> Function; \
    static const char * name() { return #_name ; } \
}

#define FRX_LUA_REG_1(name, impl) \
    sambag::lua::registerFunction<name ## _Tag>( \
		luaState.get(), \
		boost::bind(&LuaImpl::impl, this, _1) \
	)

#define FRX_LUA_FUNC_2(_name,r1,a1,a2) \
struct _name ## _Tag { \
    typedef boost::function<r1(a1,a2)> Function; \
    static const char * name() { return #_name ; } \
}

#define FRX_LUA_REG_2(name, impl) \
    sambag::lua::registerFunction<name ## _Tag>( \
		luaState.get(), \
		boost::bind(&LuaImpl::impl, this, _1, _2) \
	)

#define FRX_LUA_FUNC_3(_name,r1,a1,a2,a3) \
struct _name ## _Tag { \
    typedef boost::function<r1(a1,a2,a3)> Function; \
    static const char * name() { return #_name ; } \
}

#define FRX_LUA_REG_3(name, impl) \
    sambag::lua::registerFunction<name ## _Tag>( \
		luaState.get(), \
		boost::bind(&LuaImpl::impl, this, _1, _2, _3) \
	)

#define FRX_LUA_FUNC_4(_name,r1,a1,a2,a3,a4) \
struct _name ## _Tag { \
    typedef boost::function<r1(a1,a2,a3,a4)> Function; \
    static const char * name() { return #_name ; } \
}

#define FRX_LUA_REG_4(name, impl) \
    sambag::lua::registerFunction<name ## _Tag>( \
		luaState.get(), \
		boost::bind(&LuaImpl::impl, this, _1, _2, _3, _4) \
	)

#define FRX_LUA_FUNC_5(_name,r1,a1,a2,a3,a4,a5) \
struct _name ## _Tag { \
    typedef boost::function<r1(a1,a2,a3,a4,a5)> Function; \
    static const char * name() { return #_name ; } \
}

#define FRX_LUA_REG_5(name, impl) \
    sambag::lua::registerFunction<name ## _Tag>( \
		luaState.get(), \
		boost::bind(&LuaImpl::impl, this, _1, _2, _3, _4, _5) \
	)



namespace frx { namespace processing {
namespace oldPr = ::processing;
namespace oldPrPa = ::processing::parameter;
//=============================================================================
/** 
  * @class PluginImpl.
  */
class LuaImpl : public APluginImpl {
//=============================================================================
public:
	//-------------------------------------------------------------------------
    typedef sambag::lua::LuaSequenceEx<oldPr::Frames::T> LuaFloatSeqEx;
	//-------------------------------------------------------------------------
    typedef sambag::lua::LuaSequence<oldPr::Frames::T> LuaFloatSeq;
    //-------------------------------------------------------------------------
	typedef boost::tuple< LuaFloatSeqEx > LuaFrames;
	//-------------------------------------------------------------------------
	// r,i
	typedef boost::tuple< LuaFloatSeq, LuaFloatSeq > FFTData;
    //-------------------------------------------------------------------------
    enum Flag {
        IsValid,
        HasParameterChangedFunction,
        HasProcessFunction,
        HasProcessMidiFunction,
        HasSetAudioConfigFunction,
        HasInitFunction
    };
private:
    //-------------------------------------------------------------------------
	typedef std::pair<oldPrPa::Parameter::Ptr,
        oldPrPa::Parameter::Connection> ParameterContainer;
	//-------------------------------------------------------------------------
	typedef boost::unordered_map<std::string, ParameterContainer> ParameterMap;
	//-------------------------------------------------------------------------
	ParameterMap parameterMap;
    //-------------------------------------------------------------------------
	sambag::lua::LuaStateRef luaState;
	//-------------------------------------------------------------------------
	void registerFunctions(sambag::lua::LuaStateRef luaState);
    //-------------------------------------------------------------------------
    std::string scriptFile;
    //-------------------------------------------------------------------------
    std::string scriptName;
    //-------------------------------------------------------------------------
    typedef sambag::lua::LuaMap<std::string, std::string> Config;
    mutable Config config;
    //-------------------------------------------------------------------------
	// lock lua calls 
	com::Mutex mutex;
    //-------------------------------------------------------------------------
    unsigned int flags;
    //-------------------------------------------------------------------------
    size_t numInputs, numOutputs, currNumSamples;
    //-------------------------------------------------------------------------
    oldPr::Frames::T **currInputs, **currOutputs;
    protected:
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
    //-------------------------------------------------------------------------
    inline std::string logName() const {
        return scriptName + ": ";
    }
    //-------------------------------------------------------------------------
    void log(const std::string &msg);
    //-------------------------------------------------------------------------
    void log_err(const std::string &msg);
    //-------------------------------------------------------------------------
    void log_warn(const std::string &msg);
    //-------------------------------------------------------------------------
    void log_trace(const std::string &msg);
    //-------------------------------------------------------------------------
    void scriptFailed(const std::string &msg);
    //-------------------------------------------------------------------------
    void loadScript();
    //-------------------------------------------------------------------------
    void loadParameter();
    //-------------------------------------------------------------------------
    void loadIOs();
    //-------------------------------------------------------------------------
    void initScript();
    //-------------------------------------------------------------------------
    void checkFunctions();
    //-------------------------------------------------------------------------
    void onParameterChanged(void *src, float value, std::string id);
    //-------------------------------------------------------------------------
    LuaFrames frxGetInput(int channel);
    //-------------------------------------------------------------------------
    FFTData frxFFT(int numSamples);
    //-------------------------------------------------------------------------
    void frxToOutput();
    //-------------------------------------------------------------------------
    void frxSetParameter(const std::string &name, float value);
public:
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
	virtual void openEditor(void *window);
	//-------------------------------------------------------------------------
	virtual void closeEditor(void *window);
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
     * @return WindowImpl if the plugin has its own. Can be NULL. 
     * (Bridged plugins have its own impl.)
     */
    virtual AWindowImplPtr getWindowImpl() {
        return AWindowImplPtr();
    }
}; // PluginImpl
}} // namespace(s)

#endif /* SAMBAG_PLUGINIMPL_H */
