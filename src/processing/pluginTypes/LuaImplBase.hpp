/*
 * THIS FILE IS AUTO CREATED BY THE LUACPP BUILD PROGRAM
 * EVERY CHANGES WILL BE OVERWRITTEN THE NEXT TIME 
 * THE THIS FILE IS GENERATED  
 *
 * LuaImplBase.hpp
 *
 *  Created on: Tue Apr 29 11:39:53 2014
 *      Author: Samba Godschysnki
 */

#ifndef SAMBAG_LuaImplBase_H
#define SAMBAG_LuaImplBase_H

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <sambag/lua/Lua.hpp>
#include <loki/TypeList.h>
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
private:
protected:
    //-------------------------------------------------------------------------
    LuaImplBase() {}
    //-------------------------------------------------------------------------
    SAMBAG_LUA_FTAG(log, void (std::string));
	SAMBAG_LUA_FTAG(logWarn, void (std::string));
	SAMBAG_LUA_FTAG(logErr, void (std::string));
	SAMBAG_LUA_FTAG(logTrace, void (std::string));
	SAMBAG_LUA_FTAG(sendMidi, void ());
	SAMBAG_LUA_FTAG(getChannel, LuaFrames (int));
	SAMBAG_LUA_FTAG(fft, FFTData ());
	SAMBAG_LUA_FTAG(setChannel, void ());
	SAMBAG_LUA_FTAG(getSamplePos, double ());
	SAMBAG_LUA_FTAG(getBarStartPos, double ());
	SAMBAG_LUA_FTAG(getPpqPos, double ());
	SAMBAG_LUA_FTAG(getTimeSigNumerator, int ());
	SAMBAG_LUA_FTAG(getTimeSigDenominator, int ());
	SAMBAG_LUA_FTAG(getTempo, double ());
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

	typedef LOKI_TYPELIST_8(Frx_getPpqPos_Tag, 
	Frx_getTimeSigNumerator_Tag, 
	Frx_getTimeSigDenominator_Tag, 
	Frx_getTempo_Tag, 
	Frx_setParameterValue_Tag, 
	Frx_setParameterDisplay_Tag, 
	Frx_getPersistUserData_Tag, 
	Frx_setPersistUserData_Tag) Functions2;

	
    ///////////////////////////////////////////////////////////////////////////
    /**
	* @brief TODO
	*/
	virtual void log(lua_State *lua, const std::string & msg) = 0;
	/**
	* @brief TODO
	*/
	virtual void logWarn(lua_State *lua, const std::string & msg) = 0;
	/**
	* @brief TODO
	*/
	virtual void logErr(lua_State *lua, const std::string & msg) = 0;
	/**
	* @brief TODO
	*/
	virtual void logTrace(lua_State *lua, const std::string & msg) = 0;
	/**
	* @brief TODO
	*/
	virtual void sendMidi(lua_State *lua) = 0;
	/**
	* @brief TODO
	*/
	virtual LuaFrames getChannel(lua_State *lua, int nb) = 0;
	/**
	* @brief TODO
	*/
	virtual FFTData fft(lua_State *lua) = 0;
	/**
	* @brief TODO
	*/
	virtual void setChannel(lua_State *lua) = 0;
	/**
	* @brief TODO
	*/
	virtual double getSamplePos(lua_State *lua) = 0;
	/**
	* @brief TODO
	*/
	virtual double getBarStartPos(lua_State *lua) = 0;
	/**
	* @brief TODO
	*/
	virtual double getPpqPos(lua_State *lua) = 0;
	/**
	* @brief TODO
	*/
	virtual int getTimeSigNumerator(lua_State *lua) = 0;
	/**
	* @brief TODO
	*/
	virtual int getTimeSigDenominator(lua_State *lua) = 0;
	/**
	* @brief TODO
	*/
	virtual double getTempo(lua_State *lua) = 0;
	/**
	* @brief TODO
	*/
	virtual void setParameterValue(lua_State *lua, const std::string & id, float value) = 0;
	/**
	* @brief TODO
	*/
	virtual void setParameterDisplay(lua_State *lua, const std::string & id, const std::string & txt) = 0;
	/**
	* @brief TODO
	*/
	virtual sambag::lua::IgnoreReturn getPersistUserData(lua_State *lua, const std::string & data) = 0;
	/**
	* @brief TODO
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

