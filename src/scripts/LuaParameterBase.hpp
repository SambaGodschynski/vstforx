/*
 * THIS FILE IS AUTO CREATED BY THE LUACPP BUILD PROGRAM
 * EVERY CHANGES WILL BE OVERWRITTEN THE NEXT TIME 
 * THE THIS FILE IS GENERATED  
 *
 * LuaParameterBase.hpp
 *
 *  Created on: Tue Apr 29 22:18:02 2014
 *      Author: Samba Godschysnki
 */

#ifndef SAMBAG_LuaParameterBase_H
#define SAMBAG_LuaParameterBase_H

#include <memory>
#include <sambag/lua/Lua.hpp>
#include <loki/Typelist.h>
#include <sambag/lua/ALuaObject.hpp>

#include <scripts/LuaModelObject.hpp>

namespace frx { namespace scripts { 
//=============================================================================
class LuaParameterBase : public LuaModelObject {
//=============================================================================
public:
    //-------------------------------------------------------------------------
    typedef LuaModelObject Super;
    //-------------------------------------------------------------------------
    typedef std::shared_ptr<LuaParameterBase> Ptr;
    //-------------------------------------------------------------------------
    typedef std::weak_ptr<LuaParameterBase> WPtr;
    //-------------------------------------------------------------------------
    
private:
protected:
    //-------------------------------------------------------------------------
    LuaParameterBase() {}
    //-------------------------------------------------------------------------
    SAMBAG_LUA_FTAG(setValue, void (float));
	SAMBAG_LUA_FTAG(getValue, float ());
	SAMBAG_LUA_FTAG(getName, std::string ());
    typedef LOKI_TYPELIST_3(Frx_setValue_Tag, 
	Frx_getValue_Tag, 
	Frx_getName_Tag) Functions1;

	
    ///////////////////////////////////////////////////////////////////////////
    /**
	* @brief TODO
	*/
	virtual void setValue(lua_State *lua, float x) = 0;
	/**
	* @brief TODO
	*/
	virtual float getValue(lua_State *lua) = 0;
	/**
	* @brief TODO
	*/
	virtual std::string getName(lua_State *lua) = 0;
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
    virtual ~LuaParameterBase() {}
private:
public:
}; // LuaParameterBase
}}

#endif /* SAMBAG_LuaParameterBase_H */

