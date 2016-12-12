/*
 * THIS FILE IS AUTO CREATED BY THE LUACPP BUILD PROGRAM
 * EVERY CHANGES WILL BE OVERWRITTEN THE NEXT TIME 
 * THE THIS FILE IS GENERATED  
 *
 * LuaFrxProcessorBase.hpp
 *
 *  Created on: Mon Dec 12 21:55:00 2016
 *      Author: Samba Godschysnki
 */

#ifndef SAMBAG_LuaFrxProcessorBase_H
#define SAMBAG_LuaFrxProcessorBase_H

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <sambag/lua/Lua.hpp>
#include <loki/Typelist.h>
#include <sambag/lua/ALuaObject.hpp>

#include <scripts/LuaFrxObject.hpp>

namespace frx { namespace scripts { 
//=============================================================================
class LuaFrxProcessorBase : public LuaFrxObject {
//=============================================================================
public:
    //-------------------------------------------------------------------------
    typedef LuaFrxObject Super;
    //-------------------------------------------------------------------------
    typedef boost::shared_ptr<LuaFrxProcessorBase> Ptr;
    //-------------------------------------------------------------------------
    typedef boost::weak_ptr<LuaFrxProcessorBase> WPtr;
    //-------------------------------------------------------------------------
    
private:
protected:
    //-------------------------------------------------------------------------
    LuaFrxProcessorBase() {}
    //-------------------------------------------------------------------------
    SAMBAG_LUA_FTAG(getInputs, sambag::lua::IgnoreReturn ());
	SAMBAG_LUA_FTAG(getOutputs, sambag::lua::IgnoreReturn ());
	SAMBAG_LUA_FTAG(getParameters, sambag::lua::IgnoreReturn ());
	SAMBAG_LUA_FTAG(getNumParameters, int ());
	SAMBAG_LUA_FTAG(addInput, sambag::lua::IgnoreReturn (bool));
	SAMBAG_LUA_FTAG(addOutput, sambag::lua::IgnoreReturn (bool));
	SAMBAG_LUA_FTAG(openCloseEditor, void ());
	SAMBAG_LUA_FTAG(getNumInputs, int ());
	SAMBAG_LUA_FTAG(getNumOutputs, int ());
	SAMBAG_LUA_FTAG(getPluginLocation, std::string ());
	SAMBAG_LUA_FTAG(sendMessage, std::string (std::string));
	SAMBAG_LUA_FTAG(serialize, sambag::lua::IgnoreReturn ());
	SAMBAG_LUA_FTAG(deserialize, void ());
    typedef LOKI_TYPELIST_10(Frx_getInputs_Tag, 
	Frx_getOutputs_Tag, 
	Frx_getParameters_Tag, 
	Frx_getNumParameters_Tag, 
	Frx_addInput_Tag, 
	Frx_addOutput_Tag, 
	Frx_openCloseEditor_Tag, 
	Frx_getNumInputs_Tag, 
	Frx_getNumOutputs_Tag, 
	Frx_getPluginLocation_Tag) Functions1;

	typedef LOKI_TYPELIST_3(Frx_sendMessage_Tag, 
	Frx_serialize_Tag, 
	Frx_deserialize_Tag) Functions2;

	
    ///////////////////////////////////////////////////////////////////////////
    /**
	* @returnType Sequence
	* @return a sequence of the processors @see IO input objects
	* @version 1.0.5
	*/
	virtual sambag::lua::IgnoreReturn getInputs(lua_State *lua) = 0;
	/**
	* @returnType Sequence
	* @return a sequence of the processors @see IO output objects
	* @version 1.0.5
	*/
	virtual sambag::lua::IgnoreReturn getOutputs(lua_State *lua) = 0;
	/**
	* @returnType Sequence
	* @return a sequence of the processors @see Parameter objects
	* @version 1.0.5
	*/
	virtual sambag::lua::IgnoreReturn getParameters(lua_State *lua) = 0;
	/**
	* @return number of parameters
	* @version 1.0.53
	*/
	virtual int getNumParameters(lua_State *lua) = 0;
	/**
	* @returnType IO
	* @brief Adds a further input to the processor 
	* (if the specific processor supports this)
	* @param if true the newly created view object 
	* will follow the mouse until a button was pressed
	* @return a @see IO object or nil if the specific processor dosen't 
	* support further inputs
	* @version 1.0.5
	*/
	virtual sambag::lua::IgnoreReturn addInput(lua_State *lua, bool followMouse) = 0;
	/**
	* @returnType IO
	* @brief Adds a further output to the processor 
	* (if the specific processor supports this)
	* @param if true the newly created view object 
	* will follow the mouse until a button was pressed
	* @return a @see IO object or nil if the specific processor dosen't 
	* support further outputs
	* @version 1.0.5
	*/
	virtual sambag::lua::IgnoreReturn addOutput(lua_State *lua, bool followMouse) = 0;
	/**
	* @brief Open the processors editor or close it if
	* already open.
	* @version 1.0.5
	*/
	virtual void openCloseEditor(lua_State *lua) = 0;
	/**
	* @return the number of inputs
	* @version 1.0.5
	*/
	virtual int getNumInputs(lua_State *lua) = 0;
	/**
	* @return the number of outputs
	* @version 1.0.5
	*/
	virtual int getNumOutputs(lua_State *lua) = 0;
	/**
	* @return if the processor is an external plugin the plugin path
	* will be returned.
	* @version 1.0.5
	*/
	virtual std::string getPluginLocation(lua_State *lua) = 0;
	/**
	* @brief Sends a internal message to the processor.
	*        The behaviour depends on its specific 
	*        processor implementation.
	* @return a result string
	* @param a message string
	* @version 1.0.53
	*/
	virtual std::string sendMessage(lua_State *lua, const std::string & msg) = 0;
	/**
	* @return the processor serialized as byte string
	* @version 1.0.72
	*/
	virtual sambag::lua::IgnoreReturn serialize(lua_State *lua) = 0;
	/**
	* @brief loads a byte string
	* @hiddenParam string data
	* @param a preset data byte string
	* @version 1.0.72
	*/
	virtual void deserialize(lua_State *lua) = 0;
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
    virtual ~LuaFrxProcessorBase() {}
private:
public:
}; // LuaFrxProcessorBase
}}

#endif /* SAMBAG_LuaFrxProcessorBase_H */

