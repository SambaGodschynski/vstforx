/*
 * ===========================================================================================================
 * LuaProcessor.h
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#ifndef FORX_LUAPROCESSOR_H
#define FORX_LUAPROCESSOR_H

#include "processing/processing.h"
#include "processing/parameter/parameter.h"
#include "com/Serialization.h"
#include <sambag/lua/LuaMap.hpp>
#include <sambag/lua/LuaHelper.hpp>
#include <boost/tuple/tuple.hpp>
#include "com/ScriptInfo.h"
#include <sambag/lua/LuaSequence.hpp>

namespace processing {
using namespace parameter;
//============================================================================================================
/**
 * @class ProcessorScriptInfo.
 * Script-Info POD-Kontainer
 */
//============================================================================================================
struct ProcessorScriptInfo : public com::ScriptInfo {
	// processor setup
	size_t numInputs;
	size_t numOutputs;
	size_t latency;
	bool valid;
	// parameter
	// Key = parameterName, Value = parameter init value
	typedef sambag::lua::LuaMap<std::string, float> ParameterMap;
	ParameterMap parameterMap;
	bool hasParameterChangedHandler;
	bool hasInitFunction;
	// constructor
	ProcessorScriptInfo(const std::string &location="") :
		numInputs(0),
		numOutputs(0),
		latency(0),
		valid(false),
		hasParameterChangedHandler(false),
		hasInitFunction(false),
		ScriptInfo(location)
	{
	}
};
//============================================================================================================
/**
 * @class LuaProcessor.
 * Leitet process an lua-script weiter.
 */
class LuaProcessor : 
public ProcessAdapter, 
public HasParameter, 
public Serializable
{
//============================================================================================================
friend class boost::serialization::access;
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<LuaProcessor> Ptr;
	//--------------------------------------------------------------------------------------------------------
	typedef boost::tuple< sambag::lua::LuaSequenceEx<float>, sambag::lua::LuaSequenceEx<float> > LuaFrames;
	//--------------------------------------------------------------------------------------------------------
	typedef sambag::lua::LuaSequenceEx<float> FFTData;
private:
	//--------------------------------------------------------------------------------------------------------
	void loadScript();
	//--------------------------------------------------------------------------------------------------------
	// lock lua calls 
	com::Mutex mutex;
	//--------------------------------------------------------------------------------------------------------
	static void getScriptInfo(sambag::lua::LuaStateRef, ProcessorScriptInfo &outValue);
	//--------------------------------------------------------------------------------------------------------
	ProcessorScriptInfo scriptInfo;
	//--------------------------------------------------------------------------------------------------------
	void initListener();
	//--------------------------------------------------------------------------------------------------------
	void initScript();
	//--------------------------------------------------------------------------------------------------------
	void initIO();
	//--------------------------------------------------------------------------------------------------------
	void initCallbackFunctions();
	//--------------------------------------------------------------------------------------------------------
	void initParameter();
	//--------------------------------------------------------------------------------------------------------
	sambag::lua::LuaStateRef luaState;
	//--------------------------------------------------------------------------------------------------------
	std::string scriptfile;
	//--------------------------------------------------------------------------------------------------------
	typedef vector<Parameter::Ptr> Parameters;
	//--------------------------------------------------------------------------------------------------------
	Parameters parameters;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * (De)Serialisiert PeakTracker-Objekt
	 * @param ar boost::Archive-
	 * @param version
	 */
	template < typename Archive >
	void serialize ( Archive &ar, const unsigned int version ) {
		ar & boost::serialization::base_object < ProcessAdapter > ( *this );
		ar & scriptfile;
		// TODO: handle io changes
		if (Archive::is_loading::value) {
			luaState = sambag::lua::createLuaStateRef();
			loadScript();
		}
	}
	//--------------------------------------------------------------------------------------------------------
	LuaProcessor (){} // wird nur von boost::serial. benutzt
protected:
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	// LuaCallbacks
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	// configuration
	//--------------------------------------------------------------------------------------------------------
	void frxSetModuleLatency(int latency);
	//--------------------------------------------------------------------------------------------------------
	FFTData frxCalcFFT(const FFTData &inData);
	//--------------------------------------------------------------------------------------------------------
	FFTData frxCalcInverseFFT(const FFTData &inData);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	// processing
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
protected:
	//--------------------------------------------------------------------------------------------------------
	vector<Frames*> iodata;
	//--------------------------------------------------------------------------------------------------------
	/** 
	 * args will be poped from lua stack, to use faster LuaSeuqenceX structure.
	 */
	void frxSetFramesToOutput();
	//--------------------------------------------------------------------------------------------------------
	LuaFrames frxGetFramesFromInput(int channel);
protected:
	//--------------------------------------------------------------------------------------------------------
	LuaProcessor (IHostInfo *hostInfo, const std::string &scriptfile);
public:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * resetet SampleRate
	 */
	virtual void hostInfoChanged();
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param hostInfo
	 * @return neues MidiProcessor-Objekt
	 */
	static Ptr create(IHostInfo *hostInfo, const std::string &scriptfile) {
		Ptr neu( new LuaProcessor(hostInfo, scriptfile) );
		neu->self = neu;
		return neu;
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param src
	 * @param value
	 */
	virtual void parameterValueChanged( void *src, const float &value );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Verarbeitet Samplemenge der Eingangsknoten und fuegt Ergebniss Ausgangsknoten hinzu.
	 * @param numSamples Anzahl der zu bearbeitenden Samples
	 */
	virtual void processAdapter( Processor::Int numSamples );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param index
	 * @return liefert Parameter zu index. Wirft: std::out_of_range
	 */
	virtual Parameter::Ptr getParameter ( size_t index = 0 ) const { return parameters[index]; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Anzahl aller MidiProcessor-Parameter
	 */
	virtual size_t getNumParameter () const { return parameters.size(); }
	//--------------------------------------------------------------------------------------------------------
	virtual ~LuaProcessor ();
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Signal-Verabeitungs-Verzoegerung des uebergeordneten ProcessAdapter
	 */
	virtual size_t getProcessDelay() const { return scriptInfo.latency; }
};
}// namespace processing

#endif  // FORX_LUAPROCESSOR_H


