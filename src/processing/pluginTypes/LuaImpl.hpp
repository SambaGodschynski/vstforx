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
#include <sambag/lua/ALuaObject.hpp>
#include <sambag/com/ArithmeticWrapper.hpp>
#include <boost/unordered_map.hpp>
#include <processing/parameter/parameter.h>
#include <loki/Typelist.h>


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
    SAMBAG_LUA_FTAG(getInput,  LuaImpl::LuaFrames(int));
    SAMBAG_LUA_FTAG(fft,  LuaImpl::FFTData());
    SAMBAG_LUA_FTAG(toOutput, void());
    SAMBAG_LUA_FTAG(getSamplePos, double());
    SAMBAG_LUA_FTAG(getBarStartPos, double());
    SAMBAG_LUA_FTAG(getPpqPos, double());
    SAMBAG_LUA_FTAG(getTempo, double());
    SAMBAG_LUA_FTAG(getTimeSigNumerator, int());
    SAMBAG_LUA_FTAG(getTimeSigDenominator, int());
    SAMBAG_LUA_FTAG(setParameterValue, void(std::string, float));
    SAMBAG_LUA_FTAG(setParameterDisplay, void(std::string, std::string));
    typedef LOKI_TYPELIST_10(Frx_getInput_Tag,
        Frx_fft_Tag,
        Frx_toOutput_Tag,
        Frx_getSamplePos_Tag,
        Frx_getBarStartPos_Tag,
        Frx_getPpqPos_Tag,
        Frx_getTimeSigNumerator_Tag,
        Frx_getTimeSigDenominator_Tag,
        Frx_getTempo_Tag,
        Frx_setParameterValue_Tag
    ) Functions1;
    typedef LOKI_TYPELIST_1(Frx_setParameterDisplay_Tag
    ) Functions2;
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
    void log(const std::string &msg);
    //-------------------------------------------------------------------------
    void log_err(const std::string &msg);
    //-------------------------------------------------------------------------
    void log_warn(const std::string &msg);
    //-------------------------------------------------------------------------
    void log_trace(const std::string &msg);
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
    void checkFunctions();
    //-------------------------------------------------------------------------
    void onParameterChanged(void *src, float value, const std::string &id);
    //-------------------------------------------------------------------------
    LuaFrames frxGetInput(int channel);
    //-------------------------------------------------------------------------
    FFTData frxFFT();
    //-------------------------------------------------------------------------
    void frxToOutput();
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
    void frxSetParameterValue(const std::string &name, float value);
    //-------------------------------------------------------------------------
    void frxSetParameterDisplay(const std::string &name,
        const std::string &value);
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
