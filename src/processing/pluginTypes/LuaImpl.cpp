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
#include <sstream>
#include <com/MyString.h>
#include <processing/FrxAsyncDSPTimer.hpp>
#include <gui/HandyNamespaces.hpp>
#include <scripts/PluginScriptCtrl.hpp>
#include <scripts/LuaParameter.hpp>
#include <sambag/dsp/DefaultMidiEvents.hpp>
#include <processing/IMidiEventProcessor.h>
#include <com/Serialization.h>
#include <sstream>
#include <gui/components/FrxScriptPluginEditor.hpp>

#define LC_NAME(_name) LuaCall::_name::name()
#define LC_STR(_name) std::string(LuaCall::_name::name())

#define IF_LC_MISSING(_name)                                                   \
if (!has<LuaCall::_name>())

#define IF_HAS_LC(_name)                                                       \
if (has<LuaCall::_name>())


namespace frx { namespace processing {
namespace {
	// LC = lua call (frx2lua)
	// GP = global parameter
    const std::string GP_CONFIG = "gpConfig";
	const std::string GP_PARAMETER_SETUP = "gpParameterSetup";
    const size_t MAX_IO = 64;
    const std::string PROCESS_MIDI_DELTAFRAMES = "deltaFrames";
	const std::string PROCESS_MIDI_BYTESIZE = "size";
	const std::string PROCESS_MIDI_DATA = "data";
    const int ScriptEditorWidth = 730;
    const int ScriptEditorHeight = 300;
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
void LuaImpl::addToEditor(const std::string &msg) {
    SAMBAG_BEGIN_SYNCHRONIZED(logMutex)
        time_t rawtime;
        struct tm * timeinfo;
        char buffer [80];
        time (&rawtime);
        timeinfo = localtime (&rawtime);
        strftime (buffer,80," %H:%M:%S ",timeinfo);
        using gui::components::FrxScriptPluginEditor;
        if (logHistory.size()>FrxScriptPluginEditor::MaxLog) {
            size_t i=FrxScriptPluginEditor::MaxLog/2;
            while (i-- > 0) {
                logHistory.pop_front();
            }
        }
        logHistory.push_back(buffer + msg);

        if (!editor) {
            return;
        }
        editor->log(buffer + msg);
    SAMBAG_END_SYNCHRONIZED
}
//-----------------------------------------------------------------------------
void LuaImpl::onExecError(const std::string &msg) {
    log_err(msg);
}
//-----------------------------------------------------------------------------
void LuaImpl::log(const std::string &msg) {
    SAMBAG_LOG_INFO<<logName()<<": "<<msg;
    addToEditor("[INFO] " + msg);
}
//-----------------------------------------------------------------------------
void LuaImpl::log_err(const std::string &msg) {
    SAMBAG_LOG_ERR<<logName()<<": "<<msg;
    addToEditor("[ERROR] " + msg);
}
//-----------------------------------------------------------------------------
void LuaImpl::log_warn(const std::string &msg) {
    SAMBAG_LOG_WARN<<logName()<<": "<<msg;
    addToEditor("[WARNING] " + msg);
}
//-----------------------------------------------------------------------------
void LuaImpl::log_trace(const std::string &msg) {
    SAMBAG_LOG_TRACE<<logName()<<": "<<msg;
    addToEditor("[TRACE] " + msg);
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
    lcFlags(0),
    flags(0)
{
    scriptFile = location;
    loadScript();
}
//-----------------------------------------------------------------------------
void LuaImpl::initScript() {
    IF_LC_MISSING(lcInit) {
        return;
    }
    try {
        sambag::lua::callLuaFunc(luaState.get(), LC_NAME(lcInit));
    } catch(const sambag::lua::ExecutionFailed &ex) {
        scriptFailed("calling " + LC_STR(lcInit) + " failed: " + ex.errMsg);
    } catch(...) {
        scriptFailed("calling " + LC_STR(lcInit) + " failed");
    }
}
//-----------------------------------------------------------------------------
void LuaImpl::loadScript() {
    setFlag(IsValid, true);
    numInputs = 0;
    numOutputs = 0;
    try {
        luaState = sambag::lua::createLuaStateRef();
        initLuaEnv(luaState);
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
        loadParameters();
        log(scriptFile + " loaded:");
        log("numInputs: " + sambag::com::toString(numInputs));
        log("numOutputs: " + sambag::com::toString(numOutputs));
        log("numParameter: " + sambag::com::toString(parameters->size()));
        log("valid: " + std::string(getFlag(IsValid) ? "yes" : "no") );
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
void LuaImpl::onParameterChanged(void *src, float value, const std::string &id)
{
    
    SAMBAG_TRY_TO_LOCK_RECURSIVE(mutex);
    lua_getglobal(luaState.get(), GP_PARAMETER_SETUP.c_str());
    int tbl = lua_gettop(luaState.get());
    if (lua_istable(luaState.get(), -1)) {
        lua_pushnumber(luaState.get(), value);
        lua_setfield(luaState.get(), tbl, id.c_str());
    }
    lua_pop(luaState.get(), 1); // remove tbl
    IF_HAS_LC(lcOnParameterChanged) {
        try {
            sambag::lua::callLuaFunc(luaState.get(), LC_NAME(lcOnParameterChanged),
                boost::make_tuple(id, value)
            );
        } catch(const sambag::lua::ExecutionFailed &ex) {
            scriptFailed("calling " + LC_STR(lcOnParameterChanged) + " failed: " + ex.errMsg);
        } catch(...) {
            scriptFailed("calling " + LC_STR(lcOnParameterChanged) + " failed");
        }
    }}
//-----------------------------------------------------------------------------
void LuaImpl::loadParameters() {
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
    IF_LC_MISSING(lcSetAudioConfig) {
        return;
    }
    IHostInfo::Ptr hI = hostInfo.lock();
    if (!hI) {
        return;
    }
    try {
        sambag::lua::callLuaFunc(luaState.get(), LC_NAME(lcSetAudioConfig),
            boost::make_tuple(hI->getBlockSize(), hI->getSampleRate())
        );
    } catch(const sambag::lua::ExecutionFailed &ex) {
        scriptFailed("calling " + LC_STR(lcSetAudioConfig) + " failed: " + ex.errMsg);
    } catch(...) {
        scriptFailed("calling " + LC_STR(lcSetAudioConfig) + " failed");
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
    return true;
}
//-----------------------------------------------------------------------------
void LuaImpl::openEditor(void *window) {
	// set size
    EditorSize _new(ScriptEditorWidth, ScriptEditorHeight);
    sce::EventSender<sce::PropertyChanged>::notifyListeners(this,
        sce::PropertyChanged("editor size", _new, _new)
    );
    using frx::gui::components::FrxScriptPluginEditor;
    editor = FrxScriptPluginEditor::create(window,
        sambag::disco::Dimension(ScriptEditorWidth,ScriptEditorHeight)
    );
    BOOST_FOREACH(const std::string &x, logHistory) {
        editor->log(x);
    }
    editor->getReloadButton()->sce::EventSender<sdc::events::ActionEvent>::addEventListener(
		boost::bind(&LuaImpl::onReloadScript, this)
	);
}
//-----------------------------------------------------------------------------
void LuaImpl::onReloadScript() {
    if (getFlag(NeedsReload)) {
        return;
    }
    size_t oldi = getNumInputChannels();
    size_t oldo = getNumOutputChannels();
    size_t oldp = parameters->size();
    {
        SAMBAG_TRY_TO_LOCK_RECURSIVE(mutex);
        closeLua();
        loadScript();
    }
    if (
        oldi!=getNumInputChannels() ||
        oldo!=getNumOutputChannels() ||
        oldp!=parameters->size()
    ) {
        scriptFailed("i/o,parameter cofiguration changed, please remove and re-add");
        setFlag(NeedsReload, true);
    }
}
//-----------------------------------------------------------------------------
void LuaImpl::closeEditor(void *window) {
    editor.reset();
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
void LuaImpl::frxSetParameterValue(const std::string &name, float value) {
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
void LuaImpl::frxSetParameterDisplay(const std::string &name,
    const std::string &value)
{
    ParameterMap::iterator it = parameterMap.find(name);
    if (it==parameterMap.end()) {
        std::stringstream ss;
		ss<<"parameter "<<name<<" not found.";
		lua_pushstring (luaState.get(), ss.str().c_str());
		lua_error(luaState.get());
        return;
    }
    it->second.first->setDisplay(value);
}
//-----------------------------------------------------------------------------
void LuaImpl::processMidiEvents( sambag::dsp::IMidiEvents * events ) {
	using namespace sambag::lua;
    using namespace sambag::dsp;
	if (!getFlag(IsValid)) {
		return;
    }
    IF_LC_MISSING(lcProcessMidi) {
        return;
    }
	SAMBAG_TRY_TO_LOCK_RECURSIVE(mutex); // lock lua calls
	// prepare data
	// we can't use LuaMap because we have different value types (LuaMap<std::string,?>)
	// what occurs some unhandy circumstances:
	// - unable to use callLuaFunc
	// - do all sequence init. manually
	lua_State *L = luaState.get();
	lua_getglobal(L, LC_NAME(lcProcessMidi));
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
            push(L, PROCESS_MIDI_DATA); // midi data
            size_t byteSize = boost::get<0>(ev);
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
    } catch(...) {
        scriptFailed("calling " + LC_STR(lcProcessMidi) + " failed");
    }
}
//-----------------------------------------------------------------------------
namespace {
    std::pair<
        sambag::dsp::DefaultMidiEvents::DataPtr,
        size_t>
    __addMidiData(lua_State *L, sambag::dsp::DefaultMidiEvents &midiEvents)
    {
        using sambag::dsp::DefaultMidiEvents;
        if(!lua_istable(L, -1)) {
            throw std::runtime_error("invalid midi data");
        }
        size_t size = sambag::lua::getLen(L, -1);
        if (size==0) {
            return std::make_pair((DefaultMidiEvents::DataPtr)NULL, 0);
        }
        typedef DefaultMidiEvents::Data Data;
        DefaultMidiEvents::DataArray data(new Data[size]);
        lua_pushnil(L); /* first key */
        int i=0;
        while (lua_next(L, -2) != 0) {
            if(!lua_isnumber(L, -1)) {
                throw std::runtime_error("invalid midi data");
            }
            data[i++] = lua_tointeger(L, -1);
            lua_pop(L, 1);
        }
        midiEvents.dataContainer.push_back(data);
        return std::make_pair(data.get(), size);

    }
    void __addMidiEvent(lua_State *L, sambag::dsp::DefaultMidiEvents &midiEvents)
    {
        if(!lua_istable(L, -1)) {
            throw std::runtime_error("invalid midi event");
        }
        lua_pushnil(L); /* first key */
        int delta;
        size_t size;
        sambag::dsp::DefaultMidiEvents::DataPtr data;
        while (lua_next(L, -2) != 0) {
            if (!lua_isstring(L, -2)) {
                SAMBAG_LOG_WARN<<"sendMidi: invalid key found";
                return;
            }
            std::string key(lua_tostring(L, -2));
            if (key==PROCESS_MIDI_DELTAFRAMES) {
                if (!lua_isnumber(L, -1)) {
                    throw std::runtime_error("invalid midimessage delta frames");
                }
                delta = lua_tointeger(L, -1);
            }
            if (key==PROCESS_MIDI_DATA) {
                boost::tie(data, size) = __addMidiData(L, midiEvents);
            }
            lua_pop(L, 1);
        }
        midiEvents.events.push_back(
            sambag::dsp::IMidiEvents::MidiEvent(size, delta, data)
        );
    }
}
void LuaImpl::sendMidi() {
    lua_State *L = luaState.get();
    try {
        if(!lua_istable(L, -1)) {
            throw std::runtime_error("missing midi data argument");
        }
        size_t size = sambag::lua::getLen(L, -1);
        if (size==0) {
            return;
        }
        sambag::dsp::DefaultMidiEvents midiEvents;
        midiEvents.reserve(size);
        lua_pushnil(L); /* first key */
        while (lua_next(L, -2) != 0) {
            __addMidiEvent(L, midiEvents);
            lua_pop(L, 1);
        }
        using ::processing::IMidiEventProcessor;
        IMidiEventProcessor::EventSender::notifyListeners(this, &midiEvents);
    } catch (const std::exception &ex) {
        sambag::lua::pushLuaError(L, ex.what());
    } catch (...) {
        sambag::lua::pushLuaError(L, "unkown error");
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
	if (!getFlag(IsValid)) {
		return;
	}
    IF_LC_MISSING(lcProcess) {
        return;
    }
	// prepare input
	using namespace sambag::lua;
	try {
		SAMBAG_TRY_TO_LOCK_RECURSIVE(mutex);
		// execute processFunction
        currInputs = ins;
        currOutputs = outs;
        currNumSamples = numSamples;
		callLuaFunc(luaState.get(), LC_NAME(lcProcess), boost::make_tuple(numSamples));
        currInputs = NULL;
        currOutputs = NULL;
        currNumSamples = 0;
	} catch( const sambag::lua::LuaException &ex ) {
		scriptFailed(ex.errMsg);
	} catch(...) {
        scriptFailed("calling " + LC_STR(lcProcess) + " failed");
    }
}
//-----------------------------------------------------------------------------
LuaImpl::LuaFrames LuaImpl::frxGetInput(int channel) {
	using namespace sambag::lua;
	if (!currInputs) {
		std::stringstream ss;
		ss<<"inputs  not available. Call only within "<<LC_NAME(lcProcess)<<".";
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
		ss<<"outputs not available. Call only within "<<LC_NAME(lcProcess)<<".";
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
void LuaImpl::setPersistData() {
    namespace slua=sambag::lua;
    try {
        if(!lua_isstring(luaState.get(),  -2)) {
            throw std::runtime_error("arguments mismatch");
        }
        std::string key( lua_tostring(luaState.get(),  -2) );
        // first remove old values
        persistUserData.erase(key);
        
        if(!lua_istable(luaState.get(),  -1)) {
            throw std::runtime_error("arguments mismatch");
        }
        int index = -1;
        lua_pushnil(luaState.get()); /* first key */
        --index;
        while (lua_next(luaState.get(),  index) != 0) {
            boost::tuple<std::string> value;
            slua::pop(luaState.get(),  value);
            persistUserData.insert(std::make_pair(
                key,
                boost::get<0>(value)));
        }
    } catch(const std::exception &ex) {
        slua::pushLuaError(luaState.get(),  ex.what());
    } catch (...) {
        slua::pushLuaError(luaState.get(),  "unkown error");
    }
}
//-----------------------------------------------------------------------------
sambag::lua::IgnoreReturn LuaImpl::getPersistData(const std::string &key) {
    namespace slua=sambag::lua;
    PersistUserData::iterator it, end;
    boost::tie(it, end) = persistUserData.equal_range(key);
    lua_createtable(luaState.get(), 0, 0);
    int tbl = lua_gettop(luaState.get());
    int index=0;
    for(; it!=end; ++it) {
        lua_pushinteger(luaState.get(), ++index);
        lua_pushstring(luaState.get(), it->second.c_str());
        lua_settable(luaState.get(), tbl);
    }
    return slua::IgnoreReturn();
}

//-----------------------------------------------------------------------------
LuaImpl::FFTData LuaImpl::frxFFT() {
    try {
        FFTData data;
        if(!sambag::lua::pop(luaState.get(), data)) {
            throw std::runtime_error("arguments mismatch");
        }
        size_t ns =
            std::min(boost::get<0>(data).size(), boost::get<1>(data).size());
        if (ns==0) {
            return FFTData();
        }
        ::processing::fft( &(boost::get<0>(data)[0]),
            &(boost::get<1>(data)[0]),
            ns
        );
        return data;
    } catch(const std::exception &ex) {
        sambag::lua::pushLuaError(luaState.get(), ex.what());
    } catch(...) {
        sambag::lua::pushLuaError(luaState.get(), "unknown error");
    }
    return FFTData();
}
//-----------------------------------------------------------------------------
LuaImpl::~LuaImpl() {
    closeLua();
}
//-----------------------------------------------------------------------------
std::pair<size_t, void*> LuaImpl::getStateData() const {
    // call lua
    IF_HAS_LC(lcOnSave) {
        try {
            sambag::lua::callLuaFunc(luaState.get(), LC_NAME(lcOnSave));
        } catch(const sambag::lua::ExecutionFailed &ex) {
            SAMBAG_LOG_WARN<<"calling " << LC_STR(lcOnSave) << " failed: " << ex.errMsg;
        } catch(...) {
            SAMBAG_LOG_WARN<<"calling " << LC_STR(lcOnSave) << " failed";
        }
    }
    // save
    std::stringstream ss;
    com::oArchive ar(ss);
    ar<<persistUserData;
    stringBuffer = ss.str();
    return std::make_pair(stringBuffer.size(), (void*)stringBuffer.c_str());
}
//-----------------------------------------------------------------------------
void LuaImpl::setStateData(size_t size, void* data) {
    std::stringstream ss;
    ss.write((const char*)data, size);
    com::iArchive ar(ss);
    ar>>persistUserData;
    
    // call lua
    IF_LC_MISSING(lcOnLoad) {
        return;
    }
    try {
        sambag::lua::callLuaFunc(luaState.get(), LC_NAME(lcOnLoad));
    } catch(const sambag::lua::ExecutionFailed &ex) {
        SAMBAG_LOG_WARN<<"calling " << LC_STR(lcOnLoad) << " failed: " << ex.errMsg;
    } catch(...) {
        SAMBAG_LOG_WARN<<"calling " << LC_STR(lcOnLoad) << " failed";
    }
}
//-----------------------------------------------------------------------------
double LuaImpl::frxGetSamplePos() {
    using sambag::dsp::HostTimeInfo;
    IHostInfo::Ptr hI = hostInfo.lock();
    if (!hI) {
        return 0;
    }
    HostTimeInfo *inf = hI->getHostTimeInfo();
    if(!inf) {
        return 0;
    }
    return inf->samplePos;
}
//-----------------------------------------------------------------------------
double LuaImpl::frxGetBarStartPos() {
    using sambag::dsp::HostTimeInfo;
    IHostInfo::Ptr hI = hostInfo.lock();
    if (!hI) {
        return 0;
    }
    HostTimeInfo *inf = hI->getHostTimeInfo(HostTimeInfo::FrxBarsValid);
    if(!inf) {
        return 0;
    }
    return inf->barStartPos;
}
//-----------------------------------------------------------------------------
double LuaImpl::frxGetPpqPos() {
    using sambag::dsp::HostTimeInfo;
    IHostInfo::Ptr hI = hostInfo.lock();
    if (!hI) {
        return 0;
    }
    HostTimeInfo *inf = hI->getHostTimeInfo(HostTimeInfo::FrxPpqPos);
    if(!inf) {
        return 0;
    }
    return inf->ppqPos;
}
//-----------------------------------------------------------------------------
int LuaImpl::frxGetTimeSigNumerator() {
    using sambag::dsp::HostTimeInfo;
    IHostInfo::Ptr hI = hostInfo.lock();
    if (!hI) {
        return 0;
    }
    HostTimeInfo *inf = hI->getHostTimeInfo(HostTimeInfo::FrxTimeSigValid);
    if(!inf) {
        return 0;
    }
    return inf->timeSigNumerator;

}
//-----------------------------------------------------------------------------
int LuaImpl::frxGetTimeSigDenominator() {
    using sambag::dsp::HostTimeInfo;
    IHostInfo::Ptr hI = hostInfo.lock();
    if (!hI) {
        return 0;
    }
    HostTimeInfo *inf = hI->getHostTimeInfo(HostTimeInfo::FrxTimeSigValid);
    if(!inf) {
        return 0;
    }
    return inf->timeSigDenominator;
}
//-----------------------------------------------------------------------------
double LuaImpl::frxGetTempo() {
    using sambag::dsp::HostTimeInfo;
    IHostInfo::Ptr hI = hostInfo.lock();
    if (!hI) {
        return 0;
    }
    HostTimeInfo *inf = hI->getHostTimeInfo();
    if(!inf) {
        return 0;
    }
    return inf->tempo;
}
//-----------------------------------------------------------------------------
void LuaImpl::initLuaEnv(sambag::lua::LuaStateRef luaState) {
  
    IHostInfo::Ptr hI = hostInfo.lock();
    if (!hI) {
        SAMBAG_LOG_WARN<<"hostinfo == NULL";
        return;
    }
    scripts::PluginScriptCtrlPtr ctrl = hI->getScriptController();
    if (ctrl) {
        typedef scripts::PluginScriptCtrl::LuaProcessor LP;
        ctrl->registerFunctions(
            LP(luaState,&mutex,
                boost::bind(&LuaImpl::onExecError, this, _1)
            ), true, false);
    }
    
    lua_getglobal(luaState.get(), "frx");
    int frxTbl = lua_gettop(luaState.get());
    int plugTbl = sambag::lua::createClass<Functions1,
        sambag::lua::TupleAccessor>
    (
        luaState.get(),
        boost::make_tuple(boost::bind(&LuaImpl::frxGetInput, this, _1),
            boost::bind(&LuaImpl::frxFFT, this),
            boost::bind(&LuaImpl::frxToOutput, this),
            boost::bind(&LuaImpl::frxGetSamplePos, this),
            boost::bind(&LuaImpl::frxGetBarStartPos, this),
            boost::bind(&LuaImpl::frxGetPpqPos, this),
            boost::bind(&LuaImpl::frxGetTimeSigNumerator, this),
            boost::bind(&LuaImpl::frxGetTimeSigDenominator, this),
            boost::bind(&LuaImpl::frxGetTempo, this),
            boost::bind(&LuaImpl::frxSetParameterValue, this, _1, _2)
        ),
        "plugin"
    );
    
    boost::tuple<std::string> uid;
    lua_getfield(luaState.get(), plugTbl, SLUA_FIELDNAME_UID);
    sambag::lua::pop(luaState.get(), uid);
    this->uuid = boost::get<0>(uid);
    sambag::lua::registerClassFunctions<Functions2,
        sambag::lua::TupleAccessor>
    (
        luaState.get(),
        boost::make_tuple(
            boost::bind(&LuaImpl::frxSetParameterDisplay, this, _1, _2),
            boost::bind(&LuaImpl::sendMidi, this),
            boost::bind(&LuaImpl::getPersistData, this, _1),
            boost::bind(&LuaImpl::setPersistData, this),
            boost::bind(&LuaImpl::log, this, _1),
            boost::bind(&LuaImpl::log_warn, this, _1),
            boost::bind(&LuaImpl::log_err, this, _1),
            boost::bind(&LuaImpl::log_trace, this, _1)
        ),
        plugTbl,
        boost::get<0>(uid)
    );
    
    lua_setfield(luaState.get(), frxTbl, "plug");
    lua_pop(luaState.get(), 2); // remove frxtbl
}
//-----------------------------------------------------------------------------
void LuaImpl::closeLua() {
    luaState.reset();
    sambag::lua::unregisterClassFunctions<Functions1>(uuid);
    sambag::lua::unregisterClassFunctions<Functions2>(uuid);
}
//-----------------------------------------------------------------------------
namespace {

template <class It, class Functions = It>
struct CheckLCFunction {
    typedef typename It::Head T;
    typedef typename It::Tail Tail;
    static void check(sambag::lua::LuaStateRef l, unsigned int &flags) {
        enum { Flag = Loki::TL::IndexOf<Functions, T>::value };
        if(!sambag::lua::hasFunction(l.get(), T::name())) {
            flags &= ~(1 << Flag);
        } else {
            flags |= (1 << Flag);
        }
        CheckLCFunction<Tail, Functions>::check(l, flags);
    }
    static void toString(std::ostream &os, unsigned int flags, int indent = 0) {
        enum { Flag = Loki::TL::IndexOf<Functions, T>::value };
        unsigned int mask = (1 << Flag);
        bool has = (flags & mask) == mask;
        os<<com::MyString(" ")*indent;
        os<<T::name()<<": "<<(has?"yes":"no")<<std::endl;
        CheckLCFunction<Tail, Functions>::toString(os, flags, indent);
    }
};
template<class Functions>
struct CheckLCFunction<Loki::NullType, Functions> {
    static void check(sambag::lua::LuaStateRef, unsigned int&) {}
    static void toString(std::ostream &os, unsigned int flags, int indent = 0) {}
};


} // namespace

void LuaImpl::checkFunctions() {
    CheckLCFunction<LuaCall::List>::check(luaState, lcFlags);
    std::stringstream ss;
    CheckLCFunction<LuaCall::List>::toString(ss, lcFlags, 4);
    log("script callbacks detected:\n" + ss.str());
}


}}// namespace(s)
