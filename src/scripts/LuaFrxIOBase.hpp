/*
 * THIS FILE IS AUTO CREATED BY THE LUACPP BUILD PROGRAM
 * EVERY CHANGES WILL BE OVERWRITTEN THE NEXT TIME 
 * THE THIS FILE IS GENERATED  
 *
 * LuaFrxIOBase.hpp
 *
 *  Created on: Tue Apr 29 22:18:01 2014
 *      Author: Samba Godschysnki
 */

#ifndef SAMBAG_LuaFrxIOBase_H
#define SAMBAG_LuaFrxIOBase_H

#include <memory>
#include <sambag/lua/Lua.hpp>
#include <loki/Typelist.h>
#include <sambag/lua/ALuaObject.hpp>

#include <scripts/LuaFrxObject.hpp>

namespace frx { namespace scripts { 
//=============================================================================
class LuaFrxIOBase : public LuaFrxObject {
//=============================================================================
public:
    //-------------------------------------------------------------------------
    typedef LuaFrxObject Super;
    //-------------------------------------------------------------------------
    typedef std::shared_ptr<LuaFrxIOBase> Ptr;
    //-------------------------------------------------------------------------
    typedef std::weak_ptr<LuaFrxIOBase> WPtr;
    //-------------------------------------------------------------------------
    
private:
protected:
    //-------------------------------------------------------------------------
    LuaFrxIOBase() {}
    //-------------------------------------------------------------------------
    
    typedef Loki::NullType Functions1;
    ///////////////////////////////////////////////////////////////////////////
    
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
    virtual ~LuaFrxIOBase() {}
private:
public:
}; // LuaFrxIOBase
}}

#endif /* SAMBAG_LuaFrxIOBase_H */

