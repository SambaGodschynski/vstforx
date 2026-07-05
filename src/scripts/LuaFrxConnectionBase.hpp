/*
 * THIS FILE IS AUTO CREATED BY THE LUACPP BUILD PROGRAM
 * EVERY CHANGES WILL BE OVERWRITTEN THE NEXT TIME 
 * THE THIS FILE IS GENERATED  
 *
 * LuaFrxConnectionBase.hpp
 *
 *  Created on: Tue Apr 29 22:18:01 2014
 *      Author: Samba Godschysnki
 */

#ifndef SAMBAG_LuaFrxConnectionBase_H
#define SAMBAG_LuaFrxConnectionBase_H

#include <memory>
#include <sambag/lua/Lua.hpp>
#include <loki/Typelist.h>
#include <sambag/lua/ALuaObject.hpp>

#include <scripts/LuaFrxObject.hpp>

namespace frx { namespace scripts { 
//=============================================================================
class LuaFrxConnectionBase : public LuaFrxObject {
//=============================================================================
public:
    //-------------------------------------------------------------------------
    typedef LuaFrxObject Super;
    //-------------------------------------------------------------------------
    typedef std::shared_ptr<LuaFrxConnectionBase> Ptr;
    //-------------------------------------------------------------------------
    typedef std::weak_ptr<LuaFrxConnectionBase> WPtr;
    //-------------------------------------------------------------------------
    
private:
protected:
    //-------------------------------------------------------------------------
    LuaFrxConnectionBase() {}
    //-------------------------------------------------------------------------
    SAMBAG_LUA_FTAG(getObjects, sambag::lua::IgnoreReturn2 ());
    typedef LOKI_TYPELIST_1(Frx_getObjects_Tag) Functions1;

	
    ///////////////////////////////////////////////////////////////////////////
    /**
	* @brief TODO
	*/
	virtual sambag::lua::IgnoreReturn2 getObjects(lua_State *lua) = 0;
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
    virtual ~LuaFrxConnectionBase() {}
private:
public:
}; // LuaFrxConnectionBase
}}

#endif /* SAMBAG_LuaFrxConnectionBase_H */

