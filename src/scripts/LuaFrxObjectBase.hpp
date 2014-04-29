/*
 * THIS FILE IS AUTO CREATED BY THE LUACPP BUILD PROGRAM
 * EVERY CHANGES WILL BE OVERWRITTEN THE NEXT TIME 
 * THE THIS FILE IS GENERATED  
 *
 * LuaFrxObjectBase.hpp
 *
 *  Created on: Tue Apr 29 17:41:44 2014
 *      Author: Samba Godschysnki
 */

#ifndef SAMBAG_LuaFrxObjectBase_H
#define SAMBAG_LuaFrxObjectBase_H

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <sambag/lua/Lua.hpp>
#include <loki/TypeList.h>
#include <sambag/lua/ALuaObject.hpp>



namespace frx { namespace scripts { 
//=============================================================================
class LuaFrxObjectBase : public sambag::lua::ALuaObject {
//=============================================================================
public:
    //-------------------------------------------------------------------------
    typedef sambag::lua::ALuaObject Super;
    //-------------------------------------------------------------------------
    typedef boost::shared_ptr<LuaFrxObjectBase> Ptr;
    //-------------------------------------------------------------------------
    typedef boost::weak_ptr<LuaFrxObjectBase> WPtr;
    //-------------------------------------------------------------------------
    typedef  boost::tuple<float, float>  Point;
	
private:
protected:
    //-------------------------------------------------------------------------
    LuaFrxObjectBase() {}
    //-------------------------------------------------------------------------
    SAMBAG_LUA_FTAG(getLocation, Point ());
	SAMBAG_LUA_FTAG(setLocation, void (float, float));
	SAMBAG_LUA_FTAG(getSize, Point ());
	SAMBAG_LUA_FTAG(setSize, void (float, float));
	SAMBAG_LUA_FTAG(setName, void (std::string));
	SAMBAG_LUA_FTAG(getName, std::string ());
	SAMBAG_LUA_FTAG(getTypeId, std::string ());
	SAMBAG_LUA_FTAG(setMenu, void ());
	SAMBAG_LUA_FTAG(getViewId, std::string ());
    typedef LOKI_TYPELIST_9(Frx_getLocation_Tag, 
	Frx_setLocation_Tag, 
	Frx_getSize_Tag, 
	Frx_setSize_Tag, 
	Frx_setName_Tag, 
	Frx_getName_Tag, 
	Frx_getTypeId_Tag, 
	Frx_setMenu_Tag, 
	Frx_getViewId_Tag) Functions1;

	
    ///////////////////////////////////////////////////////////////////////////
    /**
	* @brief TODO
	*/
	virtual Point getLocation(lua_State *lua) = 0;
	/**
	* @brief TODO
	*/
	virtual void setLocation(lua_State *lua, float x, float y) = 0;
	/**
	* @brief TODO
	*/
	virtual Point getSize(lua_State *lua) = 0;
	/**
	* @brief TODO
	*/
	virtual void setSize(lua_State *lua, float x, float y) = 0;
	/**
	* @brief TODO
	*/
	virtual void setName(lua_State *lua, const std::string & name) = 0;
	/**
	* @brief TODO
	*/
	virtual std::string getName(lua_State *lua) = 0;
	/**
	* @brief TODO
	*/
	virtual std::string getTypeId(lua_State *lua) = 0;
	/**
	* @brief TODO
	*/
	virtual void setMenu(lua_State *lua) = 0;
	/**
	* @brief TODO
	*/
	virtual std::string getViewId(lua_State *lua) = 0;
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
    virtual ~LuaFrxObjectBase() {}
private:
public:
}; // LuaFrxObjectBase
}}

#endif /* SAMBAG_LuaFrxObjectBase_H */

