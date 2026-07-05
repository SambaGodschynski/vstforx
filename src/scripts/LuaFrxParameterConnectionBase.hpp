/*
 * THIS FILE IS AUTO CREATED BY THE LUACPP BUILD PROGRAM
 * EVERY CHANGES WILL BE OVERWRITTEN THE NEXT TIME 
 * THE THIS FILE IS GENERATED  
 *
 * LuaFrxParameterConnectionBase.hpp
 *
 *  Created on: Tue Apr 29 22:18:01 2014
 *      Author: Samba Godschysnki
 */

#ifndef SAMBAG_LuaFrxParameterConnectionBase_H
#define SAMBAG_LuaFrxParameterConnectionBase_H

#include <memory>
#include <sambag/lua/Lua.hpp>
#include <loki/Typelist.h>
#include <sambag/lua/ALuaObject.hpp>

#include <scripts/LuaFrxConnection.hpp>
#include <sambag/lua/LuaSequence.hpp>

namespace frx { namespace scripts { 
//=============================================================================
class LuaFrxParameterConnectionBase : public LuaFrxConnection {
//=============================================================================
public:
    //-------------------------------------------------------------------------
    typedef LuaFrxConnection Super;
    //-------------------------------------------------------------------------
    typedef std::shared_ptr<LuaFrxParameterConnectionBase> Ptr;
    //-------------------------------------------------------------------------
    typedef std::weak_ptr<LuaFrxParameterConnectionBase> WPtr;
    //-------------------------------------------------------------------------
    typedef  sambag::lua::LuaSequence<std::string>  OpNames;
	
private:
protected:
    //-------------------------------------------------------------------------
    LuaFrxParameterConnectionBase() {}
    //-------------------------------------------------------------------------
    SAMBAG_LUA_FTAG(getOperatorNames, OpNames ());
	SAMBAG_LUA_FTAG(removeOperatorAt, void (int));
	SAMBAG_LUA_FTAG(addOperator, void (std::string));
	SAMBAG_LUA_FTAG(getParameters, sambag::lua::IgnoreReturn ());
    typedef LOKI_TYPELIST_4(Frx_getOperatorNames_Tag, 
	Frx_removeOperatorAt_Tag, 
	Frx_addOperator_Tag, 
	Frx_getParameters_Tag) Functions1;

	
    ///////////////////////////////////////////////////////////////////////////
    /**
	* @brief TODO
	*/
	virtual OpNames getOperatorNames(lua_State *lua) = 0;
	/**
	* @brief TODO
	*/
	virtual void removeOperatorAt(lua_State *lua, int index) = 0;
	/**
	* @brief TODO
	*/
	virtual void addOperator(lua_State *lua, const std::string & id) = 0;
	/**
	* @brief TODO
	*/
	virtual sambag::lua::IgnoreReturn getParameters(lua_State *lua) = 0;
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
    virtual ~LuaFrxParameterConnectionBase() {}
private:
public:
}; // LuaFrxParameterConnectionBase
}}

#endif /* SAMBAG_LuaFrxParameterConnectionBase_H */

