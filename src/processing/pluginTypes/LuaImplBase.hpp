/*
 * THIS FILE IS AUTO CREATED BY THE LUACPP BUILD PROGRAM
 * EVERY CHANGES WILL BE OVERWRITTEN THE NEXT TIME 
 * THE THIS FILE IS GENERATED  
 *
 * LuaImplBase.hpp
 *
 *  Created on: Fri May 16 11:50:31 2014
 *      Author: Samba Godschysnki
 */

#ifndef SAMBAG_LuaImplBase_H
#define SAMBAG_LuaImplBase_H

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <sambag/lua/Lua.hpp>
#include <loki/Typelist.h>
#include <sambag/lua/ALuaObject.hpp>

#include <processing/pluginTypes/LuaDefs.hpp>

namespace frx { namespace processing { 
//=============================================================================
class LuaImplBase : public sambag::lua::ALuaObject {
//=============================================================================
public:
    //-------------------------------------------------------------------------
    typedef sambag::lua::ALuaObject Super;
    //-------------------------------------------------------------------------
    typedef boost::shared_ptr<LuaImplBase> Ptr;
    //-------------------------------------------------------------------------
    typedef boost::weak_ptr<LuaImplBase> WPtr;
    //-------------------------------------------------------------------------
    
private:
protected:
    //-------------------------------------------------------------------------
    LuaImplBase() {}
    //-------------------------------------------------------------------------
    SAMBAG_LUA_FTAG(log, void ());
	SAMBAG_LUA_FTAG(logWarn, void ());
	SAMBAG_LUA_FTAG(logErr, void ());
	SAMBAG_LUA_FTAG(logTrace, void ());
	SAMBAG_LUA_FTAG(sendMidi, void ());
	SAMBAG_LUA_FTAG(getChannel, LuaFrames (int));
	SAMBAG_LUA_FTAG(fft, FFTData ());
	SAMBAG_LUA_FTAG(setChannel, void ());
	SAMBAG_LUA_FTAG(getSamplePos, double ());
	SAMBAG_LUA_FTAG(getBarStartPos, double ());
	SAMBAG_LUA_FTAG(getPpqPos, double ());
	SAMBAG_LUA_FTAG(getTimeSigNumerator, int ());
	SAMBAG_LUA_FTAG(getTimeSigDenominator, int ());
	SAMBAG_LUA_FTAG(setParameterValue, void (std::string, float));
	SAMBAG_LUA_FTAG(setParameterDisplay, void (std::string, std::string));
	SAMBAG_LUA_FTAG(getPersistUserData, sambag::lua::IgnoreReturn (std::string));
	SAMBAG_LUA_FTAG(setPersistUserData, void ());
    typedef LOKI_TYPELIST_10(Frx_log_Tag, 
	Frx_logWarn_Tag, 
	Frx_logErr_Tag, 
	Frx_logTrace_Tag, 
	Frx_sendMidi_Tag, 
	Frx_getChannel_Tag, 
	Frx_fft_Tag, 
	Frx_setChannel_Tag, 
	Frx_getSamplePos_Tag, 
	Frx_getBarStartPos_Tag) Functions1;

	typedef LOKI_TYPELIST_7(Frx_getPpqPos_Tag, 
	Frx_getTimeSigNumerator_Tag, 
	Frx_getTimeSigDenominator_Tag, 
	Frx_setParameterValue_Tag, 
	Frx_setParameterDisplay_Tag, 
	Frx_getPersistUserData_Tag, 
	Frx_setPersistUserData_Tag) Functions2;

	
    ///////////////////////////////////////////////////////////////////////////
    /**
	* @brief Sends a log message to the plugin editor console.
	* @hiddenParam mixed_values ... 
	* @version 1.0.5
	*/
	virtual void log(lua_State *lua) = 0;
	/**
	* @brief Sends a log warning message to the plugin editor console.
	* @hiddenParam mixed_values ...
	* @version 1.0.5
	*/
	virtual void logWarn(lua_State *lua) = 0;
	/**
	* @brief Sends a log error message to the plugin editor console.
	* @hiddenParam mixed_values ...
	* @version 1.0.5
	*/
	virtual void logErr(lua_State *lua) = 0;
	/**
	* @brief Sends a log trace message to the plugin editor console.
	* @hiddenParam mixed_values ...
	* @version 1.0.5
	*/
	virtual void logTrace(lua_State *lua) = 0;
	/**
	* @brief Sends a midi message to VSTForx.
	* @hiddenParam sequence msg
	* @param the midi byte message
	* @version 1.0.5
	*/
	virtual void sendMidi(lua_State *lua) = 0;
	/**
	* @return The input data of the given channel.
	* @returnType sequence
	* @param the channel number
	* @note this function can only be called during the lcProcess callback 
	* @version 1.0.5
	*/
	virtual LuaFrames getChannel(lua_State *lua, int nb) = 0;
	/**
	* @brief Calculates fft data according to this algorithm: 
	* <a href="http://fftguru.com/fftguru.com.tutorial.pdf">fftguru.com</a>
	* @return the fft data output sequence
	* @hiddenParam sequence r
	* @hiddenParam sequence i     
	* @param the r input sequence
	* @param the i input sequence	
	* @returnType sequence
	* @version 1.0.5
	*/
	virtual FFTData fft(lua_State *lua) = 0;
	/**
	* @brief Set the channel output data.
	* @hiddenParam int nb
	* @hiddenParam sequence data
	* @param the channel number
	* @param the channel data sequence
	* @note this function can only be called during the lcProcess callback 
	* @version 1.0.5
	*/
	virtual void setChannel(lua_State *lua) = 0;
	/**
	* @return the current position in audio samples
	* @version 1.0.5
	*/
	virtual double getSamplePos(lua_State *lua) = 0;
	/**
	* @return the last bar start position, in quarter note 
	* @version 1.0.5
	*/
	virtual double getBarStartPos(lua_State *lua) = 0;
	/**
	* @return musical position, in quarter note (1.0 equals 1 quarter note). 
	* @version 1.0.5
	*/
	virtual double getPpqPos(lua_State *lua) = 0;
	/**
	* @return time signature numerator (e.g. 3 for 3/4).
	* @version 1.0.5
	*/
	virtual int getTimeSigNumerator(lua_State *lua) = 0;
	/**
	* @return time signature denominator (e.g. 4 for 3/4).
	* @version 1.0.5
	*/
	virtual int getTimeSigDenominator(lua_State *lua) = 0;
	/**
	* @return the current tempo in BPM
	* @version 1.0.5
	*
	double getTempo();
	/**
	* @brief set the parameter value
	* @param the parameter id
	* @param the parameter value (0..1)
	* @version 1.0.5
	*/
	virtual void setParameterValue(lua_State *lua, const std::string & id, float value) = 0;
	/**
	* @brief Set the parameter display text.
	* @param the parameter id
	* @param the text
	* @version 1.0.5
	*/
	virtual void setParameterDisplay(lua_State *lua, const std::string & id, const std::string & txt) = 0;
	/**
	* @return Tte persist user data for a key.
	* @returnType sequence
	* @param the key
	* @version 1.0.5
	*/
	virtual sambag::lua::IgnoreReturn getPersistUserData(lua_State *lua, const std::string & key) = 0;
	/**
	* @brief Set persist user data for a given key.
	* @hiddenParam string key
	* @hiddenParam sequence data
	* @param the key
	* @param the data value sequence e.g {"first value","second value", 100}
	* @version 1.0.5
	*/
	virtual void setPersistUserData(lua_State *lua) = 0;
    //-------------------------------------------------------------------------
    /**
     * @brief field getter and setter
     */
    
    //-------------------------------------------------------------------------
    /**
     * @override 
     */	
    virtual void addLuaFields(lua_State *lua, int index);
public:
    //-------------------------------------------------------------------------
    /**
     * @brief called when lua object will be removed.
     */
    virtual void __lua_gc(lua_State *lua);
public:
    //-------------------------------------------------------------------------
    virtual ~LuaImplBase() {}
private:
public:
}; // LuaImplBase
}}

#endif /* SAMBAG_LuaImplBase_H */

