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
void LuaImpl::onParameterChanged(void *src, float value, const std::string &id)
{
    TRY_TO_LOCK_TIMED(mutex);
    sambag::lua::executeString(luaState.get(),
            GP_PARAMETER_SETUP+"[\"" + id + "\"] = " + sambag::com::toString(value)
    );
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
	if (!getFlag(IsValid)) {
		return;
    }
    IF_LC_MISSING(lcProcessMidi) {
        return;
    }
	TRY_TO_LOCK_TIMED(mutex); // lock lua calls
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
    } catch(...) {
        scriptFailed("calling " + LC_STR(lcProcessMidi) + " failed");
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
		TRY_TO_LOCK_TIMED(mutex);
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
void LuaImpl::onTimer(const std::string &luaCallback) {
    if(!sambag::lua::hasFunction(luaState.get(), luaCallback.c_str())) {
        std::stringstream ss;
		ss<<"timer callback: '"<<luaCallback<<"' not found.";
		lua_pushstring (luaState.get(), ss.str().c_str());
		lua_error(luaState.get());
        return;
    }
    try {
       sambag::lua::callLuaFunc(luaState.get(), luaCallback.c_str());
    } catch( const sambag::lua::LuaException &ex ) {
		scriptFailed(ex.errMsg);
	} catch(...) {
        scriptFailed("calling " + luaCallback + " failed");
    }
}
//-----------------------------------------------------------------------------
void LuaImpl::frxAddTimer(const std::string &luaCallback, int ms, int numRepetitions)
{
    typedef FrxAsyncDSPTimer Timer;
    Timer::Ptr timer = Timer::create(ms);
    timer->sce::EventSender<Timer::Event>::addTrackedEventListener(
        boost::bind(&LuaImpl::onTimer, this, luaCallback),
        luaState
    );
    timer->setNumRepetitions(numRepetitions);
    timer->start();
}
///////////////////////////////////////////////////////////////////////////////
// template magic
//-----------------------------------------------------------------------------
namespace {

template <class Functions, class Master>
struct RegFunction {
    typedef typename Functions::Head T;
    typedef typename Functions::Tail Tail;
    static void reg(sambag::lua::LuaStateRef l, Master *master) {
        T::reg(l, master);
        RegFunction<Tail, Master>::reg(l, master);
    }
};

template <class Master>
struct RegFunction<Loki::NullType, Master> {
    static void reg(sambag::lua::LuaStateRef l, Master *master) {}
};

} // namespace

void LuaImpl::registerFunctions(sambag::lua::LuaStateRef luaState) {
    RegFunction<FrxFunctions::List, LuaImpl>::reg(luaState, this);
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
    log_trace(ss.str());
}


}}// namespace(s)
