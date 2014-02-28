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
#include <loki/Typelist.h>

#define FRX_LUA_FUNC(_name,impl,r1)                                            \
struct _name {                                                                 \
    typedef boost::function<r1()> Function;                                    \
    static const char * name() { return #_name ; }                             \
    template <class T>                                                         \
    static void reg(sambag::lua::LuaStateRef l, T *master) {                   \
        sambag::lua::registerFunction<_name>(                                  \
            l.get(),                                                           \
            boost::bind(&impl, master)                                         \
        );                                                                     \
    }                                                                          \
}
#define FRX_LUA_FUNC_1(_name,impl,r1,a1)                                       \
struct _name {                                                                 \
    typedef boost::function<r1(a1)> Function;                                  \
    static const char * name() { return #_name ; }                             \
    template <class T>                                                         \
    static void reg(sambag::lua::LuaStateRef l, T *master) {                   \
        sambag::lua::registerFunction<_name>(                                  \
            l.get(),                                                           \
            boost::bind(&impl, master, _1)                                     \
        );                                                                     \
    }                                                                          \
}
#define FRX_LUA_FUNC_2(_name,impl,r1,a1,a2)                                    \
struct _name {                                                                 \
    typedef boost::function<r1(a1,a2)> Function;                               \
    static const char * name() { return #_name ; }                             \
    template <class T>                                                         \
    static void reg(sambag::lua::LuaStateRef l, T *master) {                   \
        sambag::lua::registerFunction<_name>(                                  \
            l.get(),                                                           \
            boost::bind(&impl, master, _1, _2)                                 \
        );                                                                     \
    }                                                                          \
}
#define FRX_LUA_FUNC_3(_name,impl,r1,a1,a2,a3)                                 \
struct _name {                                                                 \
    typedef boost::function<r1(a1,a2,a3)> Function;                            \
    static const char * name() { return #_name ; }                             \
    template <class T>                                                         \
    static void reg(sambag::lua::LuaStateRef l, T *master) {                   \
        sambag::lua::registerFunction<_name>(                                  \
            l.get(),                                                           \
            boost::bind(&impl, master, _1, _2,_3)                              \
        );                                                                     \
    }                                                                          \
}
#define FRX_LUA_FUNC_4(_name,impl,r1,a1,a2,a3,a4)                              \
struct _name {                                                                 \
    typedef boost::function<r1(a1,a2,a3,a4)> Function;                         \
    static const char * name() { return #_name ; }                             \
    template <class T>                                                         \
    static void reg(sambag::lua::LuaStateRef l, T *master) {                   \
        sambag::lua::registerFunction<_name>(                                  \
            l.get(),                                                           \
            boost::bind(&impl, master, _1, _2,_3,_4)                           \
        );                                                                     \
    }                                                                          \
}
#define FRX_LUA_FUNC_5(_name,impl,r1,a1,a2,a3,a4,a5)                           \
struct _name {                                                                 \
    typedef boost::function<r1(a1,a2,a3,a4,a5)> Function;                      \
    static const char * name() { return #_name ; }                             \
    template <class T>                                                         \
    static void reg(sambag::lua::LuaStateRef l, T *master) {                   \
        sambag::lua::registerFunction<_name>(                                  \
            l.get(),                                                           \
            boost::bind(&impl, master, _1, _2,_3,_4,_5)                        \
        );                                                                     \
    }                                                                          \
}

#define LUA_CALL(_name)                                                        \
struct _name {                                                                 \
    static const char * name() { return #_name ; }                             \
}


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
    struct FrxFunctions { // lua2frx
        FRX_LUA_FUNC_1(frxLog, LuaImpl::log, void, std::string);
        FRX_LUA_FUNC_1(frxErr, LuaImpl::log_err,void, std::string);
        FRX_LUA_FUNC_1(frxWarn, LuaImpl::log_warn,void, std::string);
        FRX_LUA_FUNC_1(frxTrace, LuaImpl::log_trace,void, std::string);
        FRX_LUA_FUNC_2(frxSetParameterValue, LuaImpl::frxSetParameterValue,void, std::string, float);
        FRX_LUA_FUNC_2(frxSetParameterDisplay, LuaImpl::frxSetParameterDisplay,void, std::string, std::string);
        FRX_LUA_FUNC_1(frxGetInput, LuaImpl::frxGetInput, LuaImpl::LuaFrames, int);
        FRX_LUA_FUNC_1(frxFFT, LuaImpl::frxFFT,LuaImpl::FFTData, int);
        FRX_LUA_FUNC(frxToOutput, LuaImpl::frxToOutput, void);
        FRX_LUA_FUNC(frxGetSamplePos, LuaImpl::frxGetSamplePos, double);
        FRX_LUA_FUNC(frxGetBarStartPos, LuaImpl::frxGetBarStartPos, double);
        FRX_LUA_FUNC(frxGetPpqPos, LuaImpl::frxGetPpqPos, double);
        FRX_LUA_FUNC(frxGetTempo, LuaImpl::frxGetTempo, double);
        FRX_LUA_FUNC(frxGetTimeSigNumerator, LuaImpl::frxGetTimeSigNumerator, int);
        FRX_LUA_FUNC(frxGetTimeSigDenominator, LuaImpl::frxGetTimeSigDenominator, int);
        FRX_LUA_FUNC_3(frxAddTimer, LuaImpl::frxAddTimer, void, std::string, int, int);
        typedef LOKI_TYPELIST_16(frxLog,
            frxErr,
            frxWarn,
            frxTrace,
            frxSetParameterValue,
            frxSetParameterDisplay,
            frxGetInput,
            frxFFT,
            frxToOutput,
            frxGetSamplePos,
            frxGetBarStartPos, // 10
            frxGetPpqPos,
            frxGetTimeSigNumerator,
            frxGetTimeSigDenominator,
            frxGetTempo,
            frxAddTimer) List;
    };
    //-------------------------------------------------------------------------
    struct LuaCall { // frxlLua
       	LUA_CALL(lcOnParameterChanged);
        LUA_CALL(lcProcess);
        LUA_CALL(lcProcessMidi);
        LUA_CALL(lcSetAudioConfig);
        LUA_CALL(lcInit);
        // num > 32 can violate flags integer bounds @see lcFlags
        // use boost::dynamic_bitset in that case
        typedef LOKI_TYPELIST_5(lcOnParameterChanged,
            lcProcess,
            lcProcessMidi,
            lcSetAudioConfig,
            lcInit) List;
    };
    //-------------------------------------------------------------------------
    enum Flag {
        IsValid
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
    unsigned int lcFlags;
    //-------------------------------------------------------------------------
    unsigned int flags;
    //-------------------------------------------------------------------------
    size_t numInputs, numOutputs, currNumSamples;
    //-------------------------------------------------------------------------
    oldPr::Frames::T **currInputs, **currOutputs;
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
    //-------------------------------------------------------------------------
    void onTimer(const std::string &luaCallback);
public:
    //-------------------------------------------------------------------------
    template <class LC>
    bool has() const {
        enum { Flag = Loki::TL::IndexOf<LuaCall::List, LC>::value };
        unsigned int mask = (1 << Flag);
        return ((lcFlags & mask) == mask);
    }
    ///////////////////////////////////////////////////////////////////////////
    // lua2frx impl
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
    void onParameterChanged(void *src, float value, const std::string &id);
    //-------------------------------------------------------------------------
    LuaFrames frxGetInput(int channel);
    //-------------------------------------------------------------------------
    FFTData frxFFT(int numSamples);
    //-------------------------------------------------------------------------
    void frxToOutput();
    //-------------------------------------------------------------------------
    void frxSetParameterValue(const std::string &name, float value);
    //-------------------------------------------------------------------------
    void frxSetParameterDisplay(const std::string &name, const std::string &value);
    //-------------------------------------------------------------------------
    double frxGetSamplePos();
    //-------------------------------------------------------------------------
    double frxGetBarStartPos();
    //-------------------------------------------------------------------------
    double frxGetPpqPos();
    //-------------------------------------------------------------------------
    int frxGetTimeSigNumerator();
    //-------------------------------------------------------------------------
    int frxGetTimeSigDenominator();
    //-------------------------------------------------------------------------
    double frxGetTempo();
    //-------------------------------------------------------------------------
    void frxAddTimer(const std::string &luaCallback, int ms, int numRepetitions);
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
