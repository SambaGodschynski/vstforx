/*
 * THIS FILE IS AUTO CREATED BY THE LUACPP BUILD PROGRAM
 * EVERY CHANGES WILL BE OVERWRITTEN THE NEXT TIME 
 * THE THIS FILE IS GENERATED  
 *
 * LuaFrxViewBase.hpp
 *
 *  Created on: Tue Apr 29 22:18:02 2014
 *      Author: Samba Godschysnki
 */

#ifndef SAMBAG_LuaFrxViewBase_H
#define SAMBAG_LuaFrxViewBase_H

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <sambag/lua/Lua.hpp>
#include <loki/TypeList.h>
#include <sambag/lua/ALuaObject.hpp>



namespace frx { namespace scripts { 
//=============================================================================
class LuaFrxViewBase : public sambag::lua::ALuaObject {
//=============================================================================
public:
    //-------------------------------------------------------------------------
    typedef sambag::lua::ALuaObject Super;
    //-------------------------------------------------------------------------
    typedef boost::shared_ptr<LuaFrxViewBase> Ptr;
    //-------------------------------------------------------------------------
    typedef boost::weak_ptr<LuaFrxViewBase> WPtr;
    //-------------------------------------------------------------------------
    typedef  boost::tuple<float, float>  Point;
	
private:
protected:
    //-------------------------------------------------------------------------
    LuaFrxViewBase() {}
    //-------------------------------------------------------------------------
    SAMBAG_LUA_FTAG(add, sambag::lua::IgnoreReturn ());
	SAMBAG_LUA_FTAG(remove, void ());
	SAMBAG_LUA_FTAG(getObjects, sambag::lua::IgnoreReturn ());
	SAMBAG_LUA_FTAG(connect, sambag::lua::IgnoreReturn ());
	SAMBAG_LUA_FTAG(addKnob, sambag::lua::IgnoreReturn ());
	SAMBAG_LUA_FTAG(addHostKnob, sambag::lua::IgnoreReturn (int));
	SAMBAG_LUA_FTAG(getLocation, Point ());
	SAMBAG_LUA_FTAG(setLocation, void (float, float));
	SAMBAG_LUA_FTAG(getSize, Point ());
	SAMBAG_LUA_FTAG(setSize, void (float, float));
	SAMBAG_LUA_FTAG(getEntry, sambag::lua::IgnoreReturn ());
	SAMBAG_LUA_FTAG(getExit, sambag::lua::IgnoreReturn ());
	SAMBAG_LUA_FTAG(getByName, sambag::lua::IgnoreReturn (std::string));
	SAMBAG_LUA_FTAG(getByType, sambag::lua::IgnoreReturn (std::string));
	SAMBAG_LUA_FTAG(getSelectedObjects, sambag::lua::IgnoreReturn ());
	SAMBAG_LUA_FTAG(addViewListener, void (std::string));
	SAMBAG_LUA_FTAG(removeViewListener, void (std::string));
	SAMBAG_LUA_FTAG(setMenu, void ());
	SAMBAG_LUA_FTAG(getContextObject, sambag::lua::IgnoreReturn ());
    typedef LOKI_TYPELIST_10(Frx_add_Tag, 
	Frx_remove_Tag, 
	Frx_getObjects_Tag, 
	Frx_connect_Tag, 
	Frx_addKnob_Tag, 
	Frx_addHostKnob_Tag, 
	Frx_getLocation_Tag, 
	Frx_setLocation_Tag, 
	Frx_getSize_Tag, 
	Frx_setSize_Tag) Functions1;

	typedef LOKI_TYPELIST_9(Frx_getEntry_Tag, 
	Frx_getExit_Tag, 
	Frx_getByName_Tag, 
	Frx_getByType_Tag, 
	Frx_getSelectedObjects_Tag, 
	Frx_addViewListener_Tag, 
	Frx_removeViewListener_Tag, 
	Frx_setMenu_Tag, 
	Frx_getContextObject_Tag) Functions2;

	
    ///////////////////////////////////////////////////////////////////////////
    /**
	* @brief TODO
	*/
	virtual sambag::lua::IgnoreReturn add(lua_State *lua) = 0;
	/**
	* @brief TODO
	*/
	virtual void remove(lua_State *lua) = 0;
	/**
	* @brief TODO
	*/
	virtual sambag::lua::IgnoreReturn getObjects(lua_State *lua) = 0;
	/**
	* @brief TODO
	*/
	virtual sambag::lua::IgnoreReturn connect(lua_State *lua) = 0;
	/**
	* @brief TODO
	*/
	virtual sambag::lua::IgnoreReturn addKnob(lua_State *lua) = 0;
	/**
	* @brief TODO
	*/
	virtual sambag::lua::IgnoreReturn addHostKnob(lua_State *lua, int index) = 0;
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
	virtual void setSize(lua_State *lua, float width, float height) = 0;
	/**
	* @brief TODO
	*/
	virtual sambag::lua::IgnoreReturn getEntry(lua_State *lua) = 0;
	/**
	* @brief TODO
	*/
	virtual sambag::lua::IgnoreReturn getExit(lua_State *lua) = 0;
	/**
	* @brief TODO
	*/
	virtual sambag::lua::IgnoreReturn getByName(lua_State *lua, const std::string & name) = 0;
	/**
	* @brief TODO
	*/
	virtual sambag::lua::IgnoreReturn getByType(lua_State *lua, const std::string & type) = 0;
	/**
	* @brief TODO
	*/
	virtual sambag::lua::IgnoreReturn getSelectedObjects(lua_State *lua) = 0;
	/**
	* @brief TODO
	*/
	virtual void addViewListener(lua_State *lua, const std::string & expr) = 0;
	/**
	* @brief TODO
	*/
	virtual void removeViewListener(lua_State *lua, const std::string & expr) = 0;
	/**
	* @brief TODO
	*/
	virtual void setMenu(lua_State *lua) = 0;
	/**
	* @brief TODO
	*/
	virtual sambag::lua::IgnoreReturn getContextObject(lua_State *lua) = 0;
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
    virtual ~LuaFrxViewBase() {}
private:
public:
}; // LuaFrxViewBase
}}

#endif /* SAMBAG_LuaFrxViewBase_H */

