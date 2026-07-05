/*
 * THIS FILE IS AUTO CREATED BY THE LUACPP BUILD PROGRAM
 * EVERY CHANGES WILL BE OVERWRITTEN THE NEXT TIME 
 * THE THIS FILE IS GENERATED  
 *
 * LuaModelObjectBase.hpp
 *
 *  Created on: Tue Apr 29 22:18:02 2014
 *      Author: Samba Godschysnki
 */

#ifndef SAMBAG_LuaModelObjectBase_H
#define SAMBAG_LuaModelObjectBase_H

#include <memory>
#include <sambag/lua/Lua.hpp>
#include <loki/Typelist.h>
#include <sambag/lua/ALuaObject.hpp>



namespace frx { namespace scripts { 
//=============================================================================
class LuaModelObjectBase : public sambag::lua::ALuaObject {
//=============================================================================
public:
    //-------------------------------------------------------------------------
    typedef sambag::lua::ALuaObject Super;
    //-------------------------------------------------------------------------
    typedef std::shared_ptr<LuaModelObjectBase> Ptr;
    //-------------------------------------------------------------------------
    typedef std::weak_ptr<LuaModelObjectBase> WPtr;
    //-------------------------------------------------------------------------
    
private:
protected:
    //-------------------------------------------------------------------------
    LuaModelObjectBase() {}
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
    virtual ~LuaModelObjectBase() {}
private:
public:
}; // LuaModelObjectBase
}}

#endif /* SAMBAG_LuaModelObjectBase_H */

