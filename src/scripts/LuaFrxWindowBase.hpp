/*
 * THIS FILE IS AUTO CREATED BY THE LUACPP BUILD PROGRAM
 * EVERY CHANGES WILL BE OVERWRITTEN THE NEXT TIME 
 * THE THIS FILE IS GENERATED  
 *
 * LuaFrxWindowBase.hpp
 *
 *  Created on: Thu Jun 12 11:21:31 2014
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
    typedef  boost::tuple<float, float>  Point;
	
private:
protected:
    //-------------------------------------------------------------------------
    LuaFrxWindowBase() {}
    //-------------------------------------------------------------------------
    SAMBAG_LUA_FTAG(open, void ());
	SAMBAG_LUA_FTAG(setSize, void (int, int));
	SAMBAG_LUA_FTAG(setLocation, void (int, int));
	SAMBAG_LUA_FTAG(getSize,  Point  ());
	SAMBAG_LUA_FTAG(getLocation,  Point  ());
	SAMBAG_LUA_FTAG(setTitle, void (std::string));
	SAMBAG_LUA_FTAG(getTitle, std::string ());
	SAMBAG_LUA_FTAG(close, void ());
	SAMBAG_LUA_FTAG(addCloseListener, void (std::string));
    typedef LOKI_TYPELIST_9(Frx_open_Tag, 
	Frx_setSize_Tag, 
	Frx_setLocation_Tag, 
	Frx_getSize_Tag, 
	Frx_getLocation_Tag, 
	Frx_setTitle_Tag, 
	Frx_getTitle_Tag, 
	Frx_close_Tag, 
	Frx_addCloseListener_Tag) Functions1;

	
    ///////////////////////////////////////////////////////////////////////////
    /**
	* @brief Opens the window
	* @version 1.0.52
	*/
	virtual void open(lua_State *lua) = 0;
	/**
	* @brief set the window size
	* @param the window witdh
	* @param the window height
	* @version 1.0.52
	*/
	virtual void setSize(lua_State *lua, int width, int height) = 0;
	/**
	* @brief set the window location
	* @param the window's x coordinate
	* @param the window's y coordinate
	* @version 1.0.52
	*/
	virtual void setLocation(lua_State *lua, int x, int y) = 0;
	/**
	* @returnType Tuple
	* @return the window size (width, height)
	* @version 1.0.52
	*/
	virtual  Point  getSize(lua_State *lua) = 0;
	/**
	* @returnType Tuple
	* @return the window location (x, y)
	* @version 1.0.52
	*/
	virtual  Point  getLocation(lua_State *lua) = 0;
	/**
	* @brief Set the window title.
	* @param the title
	* @version 1.0.52
	*/
	virtual void setTitle(lua_State *lua, const std::string & title) = 0;
	/**
	* @return the window title
	* @version 1.0.52
	*/
	virtual std::string getTitle(lua_State *lua) = 0;
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

