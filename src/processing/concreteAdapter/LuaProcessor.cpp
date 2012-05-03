                                        /*
 * ===========================================================================================================
 * LuaProcessor.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#include "LuaProcessor.h"
#include <sambag/lua/LuaSequence.hpp>
#include <sambag/lua/LuaRegisterFunction.hpp>
#include <boost/foreach.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include "processing/DspTools.h"

namespace processing{
//============================================================================================================
// LuaProcessor:
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
namespace {
	// LC = lua call (vorx2lua)
	// GP = global parameter
	const std::string LC_PARAMETER_CHANGED = "lcOnParameterChanged";
	const std::string GP_NUM_INPUTS = "gpNumInputs";
	const std::string GP_NUM_OUTPUTS = "gpNumOutputs";
	const std::string GP_PARAMETER_SETUP = "gpParameterSetup";
	const std::string LC_PROCESS_FRAMES = "lcProcess";
	const std::string LC_SET_BLOCKSIZE = "lcSetBlockSize";
	const std::string LC_SET_SAMPLERATE = "lcSetSampleRate";
	const std::string LC_INIT = "lcInit";
	// function tags
	struct SetLatency_Tag {
		typedef boost::function<void(int)> Function;
		static const char * name() { return "frxSetModuleLatency"; }
	};
	struct SetFramesToOutput_Tag {
		typedef boost::function<void()> Function;
		static const char * name() { return "frxSetFramesToOutput"; }
	};
	struct GetFramesFromInput_Tag {
		typedef boost::function<LuaProcessor::LuaFrames(int)> Function;
		static const char * name() { return "frxGetFramesFromInput"; }
	};
	struct FFT_Tag {
		typedef boost::function<LuaProcessor::FFTData(size_t)> Function;
		static const char * name() { return "frxFFT"; }
	};
	struct SetParameterValue_Tag {
		typedef boost::function<void(std::string, float)> Function;
		static const char * name() { return "frxSetParameterValue"; }
	};
}
//------------------------------------------------------------------------------------------------------------
LuaProcessor::LuaProcessor (IHostInfo *iHost, const std::string &scriptfile) :
	ProcessAdapter( iHost, 1, 1 ),
	scriptfile(scriptfile)
{
	setName ("lua_processor");
	luaState = sambag::lua::createLuaStateRef();
	loadScript();
	TOLOG ( "+" + getName() );
}
//------------------------------------------------------------------------------------------------------------
LuaProcessor::~LuaProcessor() {
	int sizeInKb = lua_gc(luaState.get(), LUA_GCCOUNT, 0);
	TOLOG ( "-" + getName() + "[" + MyString(sizeInKb) + "kb]" );
	luaState.reset();
}
//------------------------------------------------------------------------------------------------------------
void LuaProcessor::initParameter() {
	using namespace processing::parameter;
	BOOST_FOREACH( 
		const ProcessorScriptInfo::ParameterMap::value_type &v, 
		scriptInfo.parameterMap) 
	{
		Parameter::Ptr p = Parameter::create();
		p->setName(v.first);
		p->setValue(v.second);
		// insert into map
		parameters[v.first] = std::make_pair(p, Parameter::Connection());
	}
	initListener();
}
//------------------------------------------------------------------------------------------------------------
Parameter::Ptr LuaProcessor::getParameter (size_t index) const {
	if (index>parameters.size())
		throw std::out_of_range("out of range");
	Parameters::const_iterator it = parameters.begin();
	while (index-- > 0)
		++it;
	return it->second.first;
}
//------------------------------------------------------------------------------------------------------------
void LuaProcessor::initListener() {
	BOOST_FOREACH(Parameters::value_type &p, parameters) {
		Parameter::Connection cn = p.second.first->addTrackedValueChangedListener(
			boost::bind(&LuaProcessor::parameterValueChanged, this, _1, _2),
			luaState
		);
		p.second.first->setValue(p.second.first->getValue());
		p.second.second = cn;
	}
}
//------------------------------------------------------------------------------------------------------------
void LuaProcessor::initCallbackFunctions() {
	using namespace sambag;
	lua::registerFunction<SetLatency_Tag>(
		luaState.get(),
		boost::bind(&LuaProcessor::frxSetModuleLatency, this, _1)
	);
	lua::registerFunction<SetFramesToOutput_Tag>(
		luaState.get(),
		boost::bind(&LuaProcessor::frxSetFramesToOutput, this)
	);
	lua::registerFunction<GetFramesFromInput_Tag>(
		luaState.get(),
		boost::bind(&LuaProcessor::frxGetFramesFromInput, this, _1)
	);
	lua::registerFunction<FFT_Tag>(
		luaState.get(),
		boost::bind(&LuaProcessor::frxFFT, this, _1)
	);
	lua::registerFunction<SetParameterValue_Tag>(
		luaState.get(),
		boost::bind(&LuaProcessor::frxSetParameterValue, this, _1, _2)
	);
}
//------------------------------------------------------------------------------------------------------------
void LuaProcessor::initIO() {
	iodata.resize(1);
/*	for (size_t i=0; i<scriptInfo.numInputs; ++i)
		ProcessAdapter::createInputNode("LuaProcessor Input: " + MyString(i));
	for (size_t i=0; i<scriptInfo.numOutputs; ++i)
		ProcessAdapter::createOutputNode("LuaProcessor Output: " + MyString(i));
	//com::events::EventSender<IOChangedEvent>::notifyEventListeners(this, IOChangedEvent());*/
}
//------------------------------------------------------------------------------------------------------------
void LuaProcessor::hostInfoChanged() {
	using namespace sambag;
	if ( lua::hasFunction(luaState.get(), LC_SET_BLOCKSIZE) ) {
		lua::callLuaFunc(luaState.get(), LC_SET_BLOCKSIZE, 
			boost::make_tuple(hostInfo->getBlockSize())
		);
	}
	if ( lua::hasFunction(luaState.get(), LC_SET_SAMPLERATE) ) {
		lua::callLuaFunc(luaState.get(), LC_SET_SAMPLERATE, 
			boost::make_tuple(hostInfo->getSampleRate())
		);
	}
}
//------------------------------------------------------------------------------------------------------------
void LuaProcessor::initScript() {
	try {
		initCallbackFunctions();
		getScriptInfo(luaState, scriptInfo); // calls script:lcInit()
		initIO();
		initParameter();
		hostInfoChanged(); // reset script sampleRate/blockSize
	} catch( const sambag::lua::LuaException &ex ) {
		TOLOG("lua script:" + scriptfile + " failed!\n  " + ex.errMsg);
		scriptInfo.valid = false;
	}
}
//------------------------------------------------------------------------------------------------------------
void LuaProcessor::getScriptInfo(sambag::lua::LuaStateRef luaState, ProcessorScriptInfo &outValue) {
	using namespace sambag;
	// processor setup
	outValue.hasInitFunction = lua::hasFunction(luaState.get(), LC_INIT);
	if (outValue.hasInitFunction)
			sambag::lua::callLuaFunc(luaState.get(), LC_INIT, 0);
	lua::getGlobal(luaState.get(), outValue.numInputs, (size_t)0, GP_NUM_INPUTS);
	lua::getGlobal(luaState.get(), outValue.numOutputs, (size_t)0, GP_NUM_OUTPUTS);
	if ( !lua::getGlobal(luaState.get(), outValue.parameterMap, GP_PARAMETER_SETUP) )
		outValue.parameterMap.clear();
	outValue.hasParameterChangedHandler = lua::hasFunction(luaState.get(), LC_PARAMETER_CHANGED);
}
//------------------------------------------------------------------------------------------------------------
void LuaProcessor::processAdapter(Processor::Int numSamples) {
	TRY_TO_LOCK_TIMED(mutex);
	Frames *frame = getInputNode(0)->popFrame();
	
	if (!scriptInfo.valid) { // script invalid
		outputNodes[0]->pushAndCopy(frame, numSamples);
		return;
	}

	using namespace sambag::lua;
	
	iodata[0] = frame;

	try {
		// execute processFunction
		callLuaFunc(luaState.get(), LC_PROCESS_FRAMES, boost::make_tuple(numSamples));
	} catch( const LuaException &ex ) {
		TOLOG("lua script:" + scriptfile + " failed!\n  " + ex.errMsg);
		scriptInfo.valid = false;
		outputNodes[0]->pushAndCopy(frame, numSamples);
		return;
	}
	outputNodes[0]->pushAndCopy(frame, numSamples);
}
//------------------------------------------------------------------------------------------------------------
void LuaProcessor::parameterValueChanged ( void *src, const float &value ) {
	using namespace sambag::lua;
	using namespace processing::parameter;
	TRY_TO_LOCK_TIMED(mutex);
	Parameter *p = static_cast<Parameter*>(src);
	if (!scriptInfo.valid || !scriptInfo.hasParameterChangedHandler) 
		return;
	// call lua function
	try {
		callLuaFunc(luaState.get(), LC_PARAMETER_CHANGED, boost::make_tuple(p->getName(), value));
	} catch (const LuaException &ex) {
		TOLOG("lua script:" + scriptfile + " failed!\n  " + ex.errMsg);
		scriptInfo.valid = false;
		return;
	}
}
//------------------------------------------------------------------------------------------------------------
void LuaProcessor::loadScript() {
	scriptInfo.valid = true;
	if (!boost::filesystem::exists(scriptfile)) {
		scriptInfo.valid = false;
		throw com::ppiError::FileIOException("loading failed: " + scriptfile, __FILE__, __LINE__ );
	}
	try {
		sambag::lua::executeFile(luaState.get(), scriptfile);
	} catch (const sambag::lua::LuaException &ex) {
		TOLOG("lua script:" + scriptfile + " failed!\n  " + ex.errMsg);
		scriptInfo.valid = false;
		return;
	}
	initScript();
}
//------------------------------------------------------------------------------------------------------------
void LuaProcessor::processMidiEvents( VstEvents * events ) {
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Callback Functions
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------------------------------------
void LuaProcessor::frxSetModuleLatency(int latency) {
	scriptInfo.latency = latency;
	hostInfo->ioChanged();
}
//------------------------------------------------------------------------------------------------------------
void LuaProcessor::frxSetFramesToOutput() {
	using namespace sambag::lua;
	Frames &frames = *iodata[0];
	size_t numSamples = frames.getSize();
	boost::tuple<LuaSequenceEx<float>, LuaSequenceEx<float>, int> args = boost::make_tuple ( 
		LuaSequenceEx<float>(frames[1], numSamples),
		LuaSequenceEx<float>(frames[0], numSamples),
		0
	);
	pop(luaState.get(), args);
}
//------------------------------------------------------------------------------------------------------------
LuaProcessor::LuaFrames LuaProcessor::frxGetFramesFromInput(int channel) {
	using namespace sambag::lua;
	Frames &frames = *iodata[0];
	size_t numSamples = frames.getSize();
	return boost::make_tuple ( 
		LuaSequenceEx<float>(frames[0], numSamples),
		LuaSequenceEx<float>(frames[1], numSamples)
	);
}
//------------------------------------------------------------------------------------------------------------
LuaProcessor::FFTData LuaProcessor::frxFFT(size_t numSamples) {
	FFTData data;
	pop(luaState.get(), data);
	fft( &(boost::get<0>(data)[0]), &(boost::get<1>(data)[0]), numSamples);
	return data;
}
//------------------------------------------------------------------------------------------------------------
void LuaProcessor::frxSetParameterValue(const std::string &name, float value) {
	Parameters::iterator it = parameters.find(name);
	if (it==parameters.end()) {
		// TODO: set lua error state
		return;
	}
	// block signal (would occur dead lock)
	boost::signals2::shared_connection_block block(it->second.second); 
	it->second.first->setValue(value);
}
}// namespace processing
