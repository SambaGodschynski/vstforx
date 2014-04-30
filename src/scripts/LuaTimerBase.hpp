/*
 * THIS FILE IS AUTO CREATED BY THE LUACPP BUILD PROGRAM
 * EVERY CHANGES WILL BE OVERWRITTEN THE NEXT TIME 
 * THE THIS FILE IS GENERATED  
 *
 * LuaTimerBase.hpp
 *
 *  Created on: Tue Apr 29 22:18:02 2014
 *      Author: Samba Godschysnki
 */

#ifndef SAMBAG_LuaTimerBase_H
#define SAMBAG_LuaTimerBase_H

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <sambag/lua/Lua.hpp>
#include <loki/TypeList.h>
#include <sambag/lua/ALuaObject.hpp>



namespace frx { namespace scripts { 
//=============================================================================
class LuaTimerBase : public sambag::lua::ALuaObject {
//=============================================================================
public:
    //-------------------------------------------------------------------------
    typedef sambag::lua::ALuaObject Super;
    //-------------------------------------------------------------------------
    typedef boost::shared_ptr<LuaTimerBase> Ptr;
    //-------------------------------------------------------------------------
    typedef boost::weak_ptr<LuaTimerBase> WPtr;
    //-------------------------------------------------------------------------
    
private:
protected:
    //-------------------------------------------------------------------------
    LuaTimerBase() {}
    //-------------------------------------------------------------------------
    SAMBAG_LUA_FTAG(start, void ());
	SAMBAG_LUA_FTAG(stop, void ());
    typedef LOKI_TYPELIST_2(Frx_start_Tag, 
	Frx_stop_Tag) Functions1;

	
    ///////////////////////////////////////////////////////////////////////////
    /**
	* @brief TODO
	*/
	virtual void start(lua_State *lua) = 0;
	/**
	* @brief TODO
	*/
	virtual void stop(lua_State *lua) = 0;
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
    virtual ~LuaTimerBase() {}
private:
public:
}; // LuaTimerBase
}}

#endif /* SAMBAG_LuaTimerBase_H */

