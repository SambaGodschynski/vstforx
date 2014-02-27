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


namespace {
	// LC = lua call (frx2lua)
	// GP = global parameter
	const std::string LC_PARAMETER_CHANGED = "lcOnParameterChanged";
    const std::string GP_CONFIG = "gpConfig";
	const std::string GP_PARAMETER_SETUP = "gpParameterSetup";
	const std::string LC_PROCESS = "lcProcess";
	const std::string LC_PROCESS_MIDI = "lcProcessMidiEvents";
	const std::string LC_SET_BLOCKSIZE = "lcSetBlockSize";
	const std::string LC_SET_SAMPLERATE = "lcSetSampleRate";
	const std::string LC_INIT = "lcInit";
    const size_t MAX_IO = 64;
}

namespace frx { namespace processing {
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
    SAMBAG_LOG_INFO<<msg;
}
//-----------------------------------------------------------------------------
void LuaImpl::log_err(const std::string &msg) {
    SAMBAG_LOG_ERR<<msg;
}
//-----------------------------------------------------------------------------
void LuaImpl::log_warn(const std::string &msg) {
    SAMBAG_LOG_WARN<<msg;
}
//-----------------------------------------------------------------------------
void LuaImpl::log_trace(const std::string &msg) {
    SAMBAG_LOG_TRACE<<msg;
}
//-----------------------------------------------------------------------------
void LuaImpl::scriptFailed(const std::string &msg) {
    log_err(msg);
    valid = false;
}
//-----------------------------------------------------------------------------
LuaImpl::LuaImpl(IHostInfo::Ptr hI, const std::string &location,
        Parameters *parameters) : APluginImpl(hI, location, parameters)
{
    scriptFile = location;
    loadScript();
}
//-----------------------------------------------------------------------------
void LuaImpl::checkFunctions() {
    if (!sambag::lua::hasFunction(luaState.get(), LC_PROCESS)) {
        scriptFailed("missing: "+LC_PROCESS);
    }
    if (!sambag::lua::hasFunction(luaState.get(), LC_PARAMETER_CHANGED)) {
        hasParameterListener = false;
    } else {
        hasParameterListener = true;
    }
}
//-----------------------------------------------------------------------------
void LuaImpl::loadScript() {
    valid = true;
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
            valid = false;
        }
        checkFunctions();
        loadIOs();
        loadParameter();
        log_trace(scriptFile + " loaded:");
        log_trace("numInputs: " + sambag::com::toString(numInputs));
        log_trace("numOutputs: " + sambag::com::toString(numOutputs));
        log_trace("numParameter: " + sambag::com::toString(parameters->size()));
        log_trace("valid: " + std::string(valid ? "yes" : "no") );
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
    //TODO: maybe a better approach
    sambag::lua::executeString(luaState.get(),
            GP_PARAMETER_SETUP+"[\"" + id + "\"] = " + sambag::com::toString(value)
    );
    if (hasParameterListener) {
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
            p->setValue (v.second);
            p->setName (v.first);
        }
        // add listener
		p->addValueChangedListener (
			boost::bind(&LuaImpl::onParameterChanged, this, _1, _2, v.first)
		);
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
}
//-----------------------------------------------------------------------------
size_t LuaImpl::getInitialDelay() const {
    return 0;
}
//-----------------------------------------------------------------------------
void LuaImpl::updatePluginInfo(::processing::PluginInfo &inf) const {
    inf.isSynth = 0;
    inf.pluginType = ::processing::PluginInfo::LUA;
    inf.name = config["name"];
    if (inf.name.empty()) {
        inf.name = boost::filesystem::path(scriptFile).filename().string();
    }
    inf.vendor = config["author"];
}
//-----------------------------------------------------------------------------
void LuaImpl::processPlugin(oldPr::Frames::T ** ins,
        oldPr::Frames::T **outs, size_t numSamples)
{
	if (!valid) { // script invalid
		return;
	}
	// prepare input
	using namespace sambag::lua;
	try {
		TRY_TO_LOCK_TIMED(mutex);
		// execute processFunction
        currInputs = ins;
        currNumSamples = numSamples;
		callLuaFunc(luaState.get(), LC_PROCESS, boost::make_tuple(numSamples));
	} catch( const sambag::lua::LuaException &ex ) {
		scriptFailed(ex.errMsg);
		return;
	}
}
//-----------------------------------------------------------------------------
LuaImpl::LuaFrames LuaImpl::frxGetFramesFromInput(int channel) {
	using namespace sambag::lua;
	channel--; // lua starts with 1 instead of 0
	if (channel < 0 || channel >= (int)numInputs ) {
		std::stringstream ss;
		ss<<"input "<<channel+1<<" not available.";
		lua_pushstring (luaState.get(), ss.str().c_str());
		lua_error(luaState.get());
	}
	
	return LuaFrames(LuaFloatSeq(currInputs[channel], currNumSamples));
}
//-----------------------------------------------------------------------------
void LuaImpl::frxSetFramesToOutput() {
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
namespace {
    FRX_LUA_FUNC_1(frxLog, void, std::string);
    FRX_LUA_FUNC_1(frxErr, void, std::string);
    FRX_LUA_FUNC_1(frxWarn, void, std::string);
    FRX_LUA_FUNC_1(frxTrace, void, std::string);
    FRX_LUA_FUNC_1(frxGetInput, LuaImpl::LuaFrames, int);
    FRX_LUA_FUNC(frxToOutput, void);
} // function tags
void LuaImpl::registerFunctions(sambag::lua::LuaStateRef luaState) {
    using namespace sambag;
	FRX_LUA_REG_1(frxLog, log);
    FRX_LUA_REG_1(frxErr, log_err);
    FRX_LUA_REG_1(frxWarn, log_warn);
    FRX_LUA_REG_1(frxTrace, log_trace);
    FRX_LUA_REG_1(frxGetInput, frxGetFramesFromInput);
    FRX_LUA_REG(frxToOutput, frxSetFramesToOutput);
}

}}// namespace(s)
