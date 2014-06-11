/*
 * THIS FILE IS AUTO CREATED BY THE LUACPP BUILD PROGRAM
 * EVERY CHANGES WILL BE OVERWRITTEN THE NEXT TIME 
 * THE THIS FILE IS GENERATED  
 *
 * LuaFrxWindowBase.hpp
 *
 *  Created on: Wed Jun 11 17:08:28 2014
 *      Author: Samba Godschysnki
 */

#ifndef SAMBAG_LuaFrxWindowBase_H
#define SAMBAG_LuaFrxWindowBase_H

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <sambag/lua/Lua.hpp>
#include <loki/Typelist.h>
#include <sambag/lua/ALuaObject.hpp>



namespace frx { namespace scripts { 
//=============================================================================
class LuaFrxWindowBase : public sambag::lua::ALuaObject {
//=============================================================================
public:
    //-------------------------------------------------------------------------
    typedef sambag::lua::ALuaObject Super;
    //-------------------------------------------------------------------------
    typedef boost::shared_ptr<LuaFrxWindowBase> Ptr;
    //-------------------------------------------------------------------------
    typedef boost::weak_ptr<LuaFrxWindowBase> WPtr;
    //-------------------------------------------------------------------------
    
private:
protected:
    //-------------------------------------------------------------------------
    LuaFrxWindowBase() {}
    //-------------------------------------------------------------------------
    SAMBAG_LUA_FTAG(open, void (float, float));
	SAMBAG_LUA_FTAG(close, void ());
	SAMBAG_LUA_FTAG(addCloseListener, void (std::string));
    typedef LOKI_TYPELIST_3(Frx_open_Tag, 
	Frx_close_Tag, 
	Frx_addCloseListener_Tag) Functions1;

	
    ///////////////////////////////////////////////////////////////////////////
    /**
	* @brief Opens the window
	* @param the window width, if < 0 size will be calculated
	* @param the window height, if < 0 size will be calculated
	* @version 1.0.52
	*/
	virtual void open(lua_State *lua, float width, float height) = 0;
	/**
	* @brief Closes the window.
	* @version 1.0.52
	*/
	virtual void close(lua_State *lua) = 0;
	/**
	* @brief Adds a listener which will be called when window is closing
	* @param a valid lua expression
	* @version 1.0.52
	*/
	virtual void addCloseListener(lua_State *lua, const std::string & expr) = 0;
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
    virtual ~LuaFrxWindowBase() {}
private:
public:
}; // LuaFrxWindowBase
}}

#endif /* SAMBAG_LuaFrxWindowBase_H */

