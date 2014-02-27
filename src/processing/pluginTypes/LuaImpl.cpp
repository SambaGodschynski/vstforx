/*
 * LuaImpl.hpp
 *
 *  Created on: Wen Feb  7 13:30:58 2014
 *      Author: Johannes Unger
 */

#include "LuaImpl.hpp"
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <processing/parameter/parameter.h>
#include <processing/dspTools.h>

namespace frx { namespace processing {
namespace {
	// LC = lua call (frx2lua)
	// GP = global parameter
	const std::string LC_PARAMETER_CHANGED = "lcOnParameterChanged";
    const std::string GP_CONFIG = "gpConfig";
	const std::string GP_PARAMETER_SETUP = "gpParameterSetup";
	const std::string LC_PROCESS = "lcProcess";
	const std::string LC_PROCESS_MIDI = "lcProcessMidi";
    const std::string LC_SET_AUDIOCONFIG = "lcSetAudioConfig";
	const std::string LC_INIT = "lcInit";
    const size_t MAX_IO = 64;
    const std::string PROCESS_MIDI_DELTAFRAMES = "deltaFrames";
	const std::string PROCESS_MIDI_BYTESIZE = "size";
	const std::string PROCESS_MIDI_DATA = "data";

    FRX_LUA_FUNC_1(frxLog, void, std::string);
    FRX_LUA_FUNC_1(frxErr, void, std::string);
    FRX_LUA_FUNC_1(frxWarn, void, std::string);
    FRX_LUA_FUNC_1(frxTrace, void, std::string);
    FRX_LUA_FUNC_2(frxSetParameter, void, std::string, float);
    FRX_LUA_FUNC_1(frxGetInput, LuaImpl::LuaFrames, int);
    FRX_LUA_FUNC_1(frxFFT, LuaImpl::FFTData, int);
    FRX_LUA_FUNC(frxToOutput, void);
}
//=============================================================================
//-----------------------------------------------------------------------------
APluginImpl * createLuaImpl(IHostInfo::Ptr hI,
    APluginImpl::Parameters *parameters,
    const std::string &location)
{
    return new LuaImpl(hI, location, parameters);
}
//=============================================================================
//  Class LuaImpl
//=============================================================================
//-----------------------------------------------------------------------------
void LuaImpl::log(const std::string &msg) {
    SAMBAG_LOG_INFO<<logName()<<": "<<msg;
}
//-----------------------------------------------------------------------------
void LuaImpl::log_err(const std::string &msg) {
    SAMBAG_LOG_ERR<<logName()<<": "<<msg;
}
//-----------------------------------------------------------------------------
void LuaImpl::log_warn(const std::string &msg) {
    SAMBAG_LOG_WARN<<logName()<<": "<<msg;
}
//-----------------------------------------------------------------------------
void LuaImpl::log_trace(const std::string &msg) {
    SAMBAG_LOG_TRACE<<logName()<<": "<<msg;
}
//-----------------------------------------------------------------------------
void LuaImpl::scriptFailed(const std::string &msg) {
    log_err(msg);
    setFlag(IsValid, false);
}
//-----------------------------------------------------------------------------
LuaImpl::LuaImpl(IHostInfo::Ptr hI, const std::string &location,
        Parameters *parameters) :
    APluginImpl(hI, location, parameters),
    flags(0)
{
    scriptFile = location;
    loadScript();
}
//-----------------------------------------------------------------------------
void LuaImpl::checkFunctions() {
    if (!sambag::lua::hasFunction(luaState.get(), LC_PROCESS)) {
        setFlag(HasProcessFunction, false);
    } else {
        setFlag(HasProcessFunction, true);
    }
    if (!sambag::lua::hasFunction(luaState.get(), LC_PARAMETER_CHANGED)) {
        setFlag(HasParameterChangedFunction, false);
    } else {
        setFlag(HasParameterChangedFunction, true);
    }
    if (!sambag::lua::hasFunction(luaState.get(), LC_PROCESS_MIDI)) {
        setFlag(HasProcessMidiFunction, false);
    } else {
        setFlag(HasProcessMidiFunction, true);
    }
    if (!sambag::lua::hasFunction(luaState.get(), LC_SET_AUDIOCONFIG)) {
        setFlag(HasSetAudioConfigFunction, false);
    } else {
        setFlag(HasSetAudioConfigFunction, true);
    }
    if (!sambag::lua::hasFunction(luaState.get(), LC_INIT)) {
        setFlag(HasInitFunction, false);
    } else {
        setFlag(HasInitFunction, true);
    }
}
//-----------------------------------------------------------------------------
void LuaImpl::initScript() {
    if (!getFlag(HasInitFunction)) {
        return;
    }
    try {
        sambag::lua::callLuaFunc(luaState.get(), LC_INIT);
    } catch(const sambag::lua::ExecutionFailed &ex) {
        scriptFailed("calling " + LC_INIT + " failed: " + ex.errMsg);
    } catch(...) {
        scriptFailed("calling " + LC_INIT + " failed");
    }
}
//-----------------------------------------------------------------------------
void LuaImpl::loadScript() {
    setFlag(IsValid, true);
    numInputs = 0;
    numOutputs = 0;
    try {
        luaState = sambag::lua::createLuaStateRef();
        registerFunctions(luaState);
        sambag::lua::executeFile(luaState.get(), location);
        if(!sambag::lua::getGlobal(luaState.get(), config, GP_CONFIG)) {
            log_err("missing " + GP_CONFIG);
        }
        if (config["type"]!="frx_lua_plugin") {
            log_err("invalid gpConfig.type: " + config["type"]);
            setFlag(IsValid, false);
        }
        scriptName = config["name"];
        if (scriptName.empty()) {
            scriptName = boost::filesystem::path(scriptFile).filename().string();
        }
        checkFunctions();
        initScript();
        loadIOs();
        loadParameter();
        log_trace(scriptFile + " loaded:");
        log_trace("numInputs: " + sambag::com::toString(numInputs));
        log_trace("numOutputs: " + sambag::com::toString(numOutputs));
        log_trace("numParameter: " + sambag::com::toString(parameters->size()));
        log_trace("valid: " + std::string(getFlag(IsValid) ? "yes" : "no") );
    } catch(const sambag::lua::ExecutionFailed &ex) {
       log_err("loading script failed: " + ex.errMsg);
    } catch(const std::exception &ex) {
       log_err(std::string("loading script failed: ") + ex.what());
    }
}
//-----------------------------------------------------------------------------
void LuaImpl::loadIOs() {
    if (!config["numInputs"].empty()) {
        std::stringstream ss;
        ss<<config["numInputs"];
        ss>>numInputs;
    } else {
        log_warn("missing gpConfig.numInputs");
    }
    if (!config["numOutputs"].empty()) {
        std::stringstream ss;
        ss<<config["numOutputs"];
        ss>>numOutputs;
    } else {
        log_warn("missing gpConfig.numOutputs");
    }
    if (numInputs>MAX_IO) {
        scriptFailed("num input > " + sambag::com::toString(MAX_IO));
    }
    if (numOutputs>MAX_IO) {
        scriptFailed("num size > " + sambag::com::toString(MAX_IO));
    }
}
//-----------------------------------------------------------------------------
void LuaImpl::onParameterChanged(void *src, float value, std::string id) {
    TRY_TO_LOCK_TIMED(mutex);
    sambag::lua::executeString(luaState.get(),
            GP_PARAMETER_SETUP+"[\"" + id + "\"] = " + sambag::com::toString(value)
    );
    if (getFlag(HasParameterChangedFunction)) {
        try {
            sambag::lua::callLuaFunc(luaState.get(), LC_PARAMETER_CHANGED,
                boost::make_tuple(id, value)
            );
        } catch(const sambag::lua::ExecutionFailed &ex) {
            scriptFailed("calling " + LC_PARAMETER_CHANGED + " failed: " + ex.errMsg);
        } catch(...) {
            scriptFailed("calling " + LC_PARAMETER_CHANGED + " failed");
        }
    }
}
//-----------------------------------------------------------------------------
void LuaImpl::loadParameter() {
    using namespace ::processing::parameter;
    typedef sambag::lua::LuaMap<std::string, float> ParameterMap;
    ParameterMap pm;
    if(!sambag::lua::getGlobal(luaState.get(), pm, GP_PARAMETER_SETUP)) {
        return;
    }
    parameters->resize(pm.size());
    size_t i=0;
    BOOST_FOREACH(const ParameterMap::value_type &v, pm) {
        Parameter::Ptr p = parameters->at(i);
        if (!p) {
            (*parameters)[i] = p = Parameter::create(i);
            p->setMin( (com::VstNumber)INT_MIN ); //entferne min, max ( siehe issue: 0000049 )
            p->setMax( (com::VstNumber)INT_MAX );
            // hole Parameter wert
            p->setName (v.first);
        }
        ParameterContainer pc;
        pc.first = p;
        // add listener
		pc.second = p->addValueChangedListener (
			boost::bind(&LuaImpl::onParameterChanged, this, _1, _2, v.first)
		);
        parameterMap[v.first] = pc;
        p->setValue(v.second);
        ++i;
	}
}
//-----------------------------------------------------------------------------
void LuaImpl::baseConfigChanged() {
}
//-----------------------------------------------------------------------------
void LuaImpl::turnOff() {
}
//-----------------------------------------------------------------------------
void LuaImpl::turnOn() {
    if (!getFlag(HasSetAudioConfigFunction)) {
        return;
    }
    IHostInfo::Ptr hI = hostInfo.lock();
    if (!hI) {
        return;
    }
    try {
        sambag::lua::callLuaFunc(luaState.get(), LC_SET_AUDIOCONFIG,
            boost::make_tuple(hI->getBlockSize(), hI->getSampleRate())
        );
    } catch(const sambag::lua::ExecutionFailed &ex) {
        scriptFailed("calling " + LC_SET_AUDIOCONFIG + " failed: " + ex.errMsg);
    } catch(...) {
        scriptFailed("calling " + LC_SET_AUDIOCONFIG + " failed");
    }
}
//-----------------------------------------------------------------------------
void LuaImpl::openPlugin() {
}
//-----------------------------------------------------------------------------
void LuaImpl::closePlugin() {
}
//-----------------------------------------------------------------------------
size_t LuaImpl::getNumInputChannels() const {
    return numInputs;
}
//-----------------------------------------------------------------------------
size_t LuaImpl::getNumOutputChannels() const {
    return numOutputs;
}
//-----------------------------------------------------------------------------
bool LuaImpl::hasEditor() const {
    return false;
}
//-----------------------------------------------------------------------------
void LuaImpl::openEditor(void *window) {
}
//-----------------------------------------------------------------------------
void LuaImpl::closeEditor(void *window) {
}
//-----------------------------------------------------------------------------
void LuaImpl::onEditorIdle() {
}
//-----------------------------------------------------------------------------
bool LuaImpl::isAccessable() const {
    return true;
}
//-----------------------------------------------------------------------------
size_t LuaImpl::getNumPrograms() {
    return 0;
}
//-----------------------------------------------------------------------------
std::string LuaImpl::getProgramName( size_t index ) {
    return "";
}
//-----------------------------------------------------------------------------
void LuaImpl::setProgram( size_t index ) {
}
//-----------------------------------------------------------------------------
int LuaImpl::getProgram() {
    return 0;
}
//-----------------------------------------------------------------------------
bool LuaImpl::canHandleMidiEvent() const {
    return true;
}
//-----------------------------------------------------------------------------
void LuaImpl::processMidiEvents( sambag::dsp::IMidiEvents * events ) {
	using namespace sambag::lua;
    using namespace sambag::dsp;
	if (!getFlag(IsValid) || !getFlag(HasProcessMidiFunction)) {
		return;
    }
	TRY_TO_LOCK_TIMED(mutex); // lock lua calls
	// prepare data
	// we can't use LuaMap because we have different value types (LuaMap<std::string,?>)
	// what occurs some unhandy circumstances:
	// - unable to use callLuaFunc
	// - do all sequence init. manually
	lua_State *L = luaState.get();
	lua_getglobal(L, LC_PROCESS_MIDI.c_str());
	lua_newtable(L);
	int top = lua_gettop(L);
	for (size_t i = 0; i<(size_t)events->getNumEvents(); ++i) {
        IMidiEvents::MidiEvent ev = events->getMidiEvent(i);
		push(L, i+1); // index
		//insert map: {'deltaFrames'=0, 'size'=0, 'data'={} }
		{
			lua_newtable(L);
			int top = lua_gettop(L);
			push(L, PROCESS_MIDI_DELTAFRAMES); // deltaFrames
			push(L, boost::get<1>(ev));
			lua_settable(L, top); //<- 
			push(L, PROCESS_MIDI_BYTESIZE); // size
            size_t byteSize = boost::get<0>(ev);
			push(L, byteSize);
			lua_settable(L, top); //<- 
			push(L, PROCESS_MIDI_DATA); // midi data
			push(L, LuaSequenceEx<IMidiEvents::Data>(boost::get<2>(ev),byteSize) );
			lua_settable(L, top); //<- 
		}
		lua_settable(L, top);
	}
	// push numEvents argument
	push(L, events->getNumEvents());
	// call function
	try {
		if (lua_pcall(L, 2, 0, 0)!=0)
			throw ExecutionFailed(std::string(lua_tostring(L, -1)));
	} catch (const LuaException &ex) {
		scriptFailed(ex.errMsg);
		return;
    }
}
//-----------------------------------------------------------------------------
size_t LuaImpl::getInitialDelay() const {
    return 0;
}
//-----------------------------------------------------------------------------
void LuaImpl::updatePluginInfo(::processing::PluginInfo &inf) const {
    inf.isSynth = 0;
    inf.pluginType = ::processing::PluginInfo::LUA;
    inf.name = scriptName;
    inf.vendor = config["author"];
}
//-----------------------------------------------------------------------------
void LuaImpl::processPlugin(oldPr::Frames::T ** ins,
        oldPr::Frames::T **outs, size_t numSamples)
{
	if (!getFlag(IsValid) || !getFlag(HasProcessFunction)) {
		return;
	}
	// prepare input
	using namespace sambag::lua;
	try {
		TRY_TO_LOCK_TIMED(mutex);
		// execute processFunction
        currInputs = ins;
        currOutputs = outs;
        currNumSamples = numSamples;
		callLuaFunc(luaState.get(), LC_PROCESS, boost::make_tuple(numSamples));
        currInputs = NULL;
        currOutputs = NULL;
        currNumSamples = 0;
	} catch( const sambag::lua::LuaException &ex ) {
		scriptFailed(ex.errMsg);
		return;
	}
}
//-----------------------------------------------------------------------------
LuaImpl::LuaFrames LuaImpl::frxGetInput(int channel) {
	using namespace sambag::lua;
	if (!currInputs) {
		std::stringstream ss;
		ss<<"inputs  not available. Call only within "<<LC_PROCESS<<".";
		lua_pushstring (luaState.get(), ss.str().c_str());
		lua_error(luaState.get());
	}
	channel--; // lua starts with 1 instead of 0
	if (channel < 0 || channel >= (int)numInputs ) {
		std::stringstream ss;
		ss<<"input "<<channel+1<<" not available.";
		lua_pushstring (luaState.get(), ss.str().c_str());
		lua_error(luaState.get());
	}
	
	return LuaFrames(LuaFloatSeqEx(currInputs[channel], currNumSamples));
}
//-----------------------------------------------------------------------------
void LuaImpl::frxToOutput() {
    // to avoid redundant copying we maniupulate the lua stack directly
    using namespace sambag::lua;
	if (!currOutputs) {
		std::stringstream ss;
		ss<<"outputs not available. Call only within "<<LC_PROCESS<<".";
		lua_pushstring (luaState.get(), ss.str().c_str());
		lua_error(luaState.get());
	}
    
	int channel = -1;
	get(channel, luaState.get(), -2);
	channel--; // lua starts with 1 instead of 0
	if (channel < 0 || channel >= (int)numOutputs ) {
		std::stringstream ss;
		ss<<"input "<<channel+1<<" not available.";
		lua_pushstring (luaState.get(), ss.str().c_str());
		lua_error(luaState.get());
	}
    // pop all arguments from stack
	boost::tuple<LuaFloatSeqEx,int> arg =
        boost::make_tuple(LuaFloatSeqEx(currOutputs[channel], currNumSamples), 0);
    pop(luaState.get(), arg);
}
//-----------------------------------------------------------------------------
LuaImpl::FFTData LuaImpl::frxFFT(int numSamples) {
    FFTData data;
	pop(luaState.get(), data);
	::processing::fft( &(boost::get<0>(data)[0]),
        &(boost::get<1>(data)[0]), numSamples
    );
	return data;
}
//-----------------------------------------------------------------------------
LuaImpl::~LuaImpl() {
}
//-----------------------------------------------------------------------------
std::pair<size_t, void*> LuaImpl::getStateData() const {
    return std::make_pair((size_t)0, (void*)NULL);
}
//-----------------------------------------------------------------------------
void LuaImpl::setStateData(size_t size, void* data) {
}
//-----------------------------------------------------------------------------
void LuaImpl::frxSetParameter(const std::string &name, float value) {
    ParameterMap::iterator it = parameterMap.find(name);
    if (it==parameterMap.end()) {
        std::stringstream ss;
		ss<<"parameter "<<name<<" not found.";
		lua_pushstring (luaState.get(), ss.str().c_str());
		lua_error(luaState.get());
        return;
    }
   	// block signal (would otherwise occur dead lock)
	boost::signals2::shared_connection_block block(it->second.second);
    it->second.first->setValue(value);
}
//-----------------------------------------------------------------------------
void LuaImpl::registerFunctions(sambag::lua::LuaStateRef luaState) {
    using namespace sambag;
	FRX_LUA_REG_1(frxLog, log);
    FRX_LUA_REG_1(frxErr, log_err);
    FRX_LUA_REG_1(frxWarn, log_warn);
    FRX_LUA_REG_1(frxTrace, log_trace);
    FRX_LUA_REG_1(frxGetInput, frxGetInput);
    FRX_LUA_REG(frxToOutput, frxToOutput);
    FRX_LUA_REG_2(frxSetParameter, frxSetParameter);
    FRX_LUA_REG_1(frxFFT, frxFFT);
}

}}// namespace(s)
