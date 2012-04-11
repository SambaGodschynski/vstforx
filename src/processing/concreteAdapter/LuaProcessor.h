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


namespace processing {
using namespace parameter;
//============================================================================================================
/**
 * @class ProcessorScriptInfo.
 * Script-Info POD-Kontainer
 */
//============================================================================================================
struct ProcessorScriptInfo {
	// processor setup
	size_t numInputs;
	size_t numOutputs;
	bool valid;
	// parameter
	// Key = parameterName, Value = parameter init value
	typedef sambag::lua::LuaMap<std::string, float> ParameterMap;
	ParameterMap parameterMap;
	bool hasParameterChangedHandler;
	// constructor
	ProcessorScriptInfo() :
		numInputs(0),
		numOutputs(0),
		valid(false),
		hasParameterChangedHandler(false)
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
private:
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
		ar & parameters; 
		ar & scriptfile;
		if (Archive::is_loading::value) {
			loadScript(scriptfile);
		}
	}
	//--------------------------------------------------------------------------------------------------------
	LuaProcessor (){} // wird nur von boost::serial. benutzt
protected:
	//--------------------------------------------------------------------------------------------------------
	LuaProcessor ( IHostInfo *hostInfo );
public:
	//--------------------------------------------------------------------------------------------------------
	void loadScript(const std::string &scriptfile);
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param hostInfo
	 * @return neues MidiProcessor-Objekt
	 */
	static Ptr create( IHostInfo *hostInfo ) {
		Ptr neu( new LuaProcessor(hostInfo ) );
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
};
}// namespace processing

#endif  // FORX_LUAPROCESSOR_H


