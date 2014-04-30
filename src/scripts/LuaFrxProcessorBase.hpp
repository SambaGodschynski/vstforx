/*
 * THIS FILE IS AUTO CREATED BY THE LUACPP BUILD PROGRAM
 * EVERY CHANGES WILL BE OVERWRITTEN THE NEXT TIME 
 * THE THIS FILE IS GENERATED  
 *
 * LuaFrxProcessorBase.hpp
 *
 *  Created on: Tue Apr 29 22:18:01 2014
 *      Author: Samba Godschysnki
 */

#ifndef SAMBAG_LuaFrxProcessorBase_H
#define SAMBAG_LuaFrxProcessorBase_H

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <sambag/lua/Lua.hpp>
#include <loki/TypeList.h>
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
	SAMBAG_LUA_FTAG(addInput, sambag::lua::IgnoreReturn (bool));
	SAMBAG_LUA_FTAG(addOutput, sambag::lua::IgnoreReturn (bool));
	SAMBAG_LUA_FTAG(openCloseEditor, void ());
	SAMBAG_LUA_FTAG(getNumInputs, int ());
	SAMBAG_LUA_FTAG(getNumOutputs, int ());
	SAMBAG_LUA_FTAG(getPluginLocation, std::string ());
    typedef LOKI_TYPELIST_9(Frx_getInputs_Tag, 
	Frx_getOutputs_Tag, 
	Frx_getParameters_Tag, 
	Frx_addInput_Tag, 
	Frx_addOutput_Tag, 
	Frx_openCloseEditor_Tag, 
	Frx_getNumInputs_Tag, 
	Frx_getNumOutputs_Tag, 
	Frx_getPluginLocation_Tag) Functions1;

	
    ///////////////////////////////////////////////////////////////////////////
    /**
	* @brief TODO
	*/
	virtual sambag::lua::IgnoreReturn getInputs(lua_State *lua) = 0;
	/**
	* @brief TODO
	*/
	virtual sambag::lua::IgnoreReturn getOutputs(lua_State *lua) = 0;
	/**
	* @brief TODO
	*/
	virtual sambag::lua::IgnoreReturn getParameters(lua_State *lua) = 0;
	/**
	* @brief TODO
	*/
	virtual sambag::lua::IgnoreReturn addInput(lua_State *lua, bool followMouse) = 0;
	/**
	* @brief TODO
	*/
	virtual sambag::lua::IgnoreReturn addOutput(lua_State *lua, bool followMouse) = 0;
	/**
	* @brief TODO
	*/
	virtual void openCloseEditor(lua_State *lua) = 0;
	/**
	* @brief TODO
	*/
	virtual int getNumInputs(lua_State *lua) = 0;
	/**
	* @brief TODO
	*/
	virtual int getNumOutputs(lua_State *lua) = 0;
	/**
	* @brief TODO
	*/
	virtual std::string getPluginLocation(lua_State *lua) = 0;
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

