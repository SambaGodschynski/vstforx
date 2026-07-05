/*
 * THIS FILE IS AUTO CREATED BY THE LUACPP BUILD PROGRAM
 * EVERY CHANGES WILL BE OVERWRITTEN THE NEXT TIME 
 * THE THIS FILE IS GENERATED  
 *
 * LuaFrxListWindowBase.hpp
 *
 *  Created on: Thu Jun 12 16:38:43 2014
 *      Author: Samba Godschysnki
 */

#ifndef SAMBAG_LuaFrxListWindowBase_H
#define SAMBAG_LuaFrxListWindowBase_H

#include <memory>
#include <sambag/lua/Lua.hpp>
#include <loki/Typelist.h>
#include <sambag/lua/ALuaObject.hpp>

#include <scripts/LuaFrxWindow.hpp>

namespace frx { namespace scripts { 
//=============================================================================
class LuaFrxListWindowBase : public frx::scripts::LuaFrxWindow {
//=============================================================================
public:
    //-------------------------------------------------------------------------
    typedef frx::scripts::LuaFrxWindow Super;
    //-------------------------------------------------------------------------
    typedef std::shared_ptr<LuaFrxListWindowBase> Ptr;
    //-------------------------------------------------------------------------
    typedef std::weak_ptr<LuaFrxListWindowBase> WPtr;
    //-------------------------------------------------------------------------
    
private:
protected:
    //-------------------------------------------------------------------------
    LuaFrxListWindowBase() {}
    //-------------------------------------------------------------------------
    SAMBAG_LUA_FTAG(add, void (std::string));
	SAMBAG_LUA_FTAG(remove, void (std::string));
	SAMBAG_LUA_FTAG(getSelection, std::string ());
	SAMBAG_LUA_FTAG(getNumElements, int ());
	SAMBAG_LUA_FTAG(getElementAt, std::string (int));
	SAMBAG_LUA_FTAG(addButton, void (std::string, std::string));
	SAMBAG_LUA_FTAG(addSelectionListener, void (std::string));
	SAMBAG_LUA_FTAG(getSelectedIndex, int ());
	SAMBAG_LUA_FTAG(removeElementAt, void (int));
    typedef LOKI_TYPELIST_9(Frx_add_Tag, 
	Frx_remove_Tag, 
	Frx_getSelection_Tag, 
	Frx_getNumElements_Tag, 
	Frx_getElementAt_Tag, 
	Frx_addButton_Tag, 
	Frx_addSelectionListener_Tag, 
	Frx_getSelectedIndex_Tag, 
	Frx_removeElementAt_Tag) Functions1;

	
    ///////////////////////////////////////////////////////////////////////////
    /**
	* @brief Adds a string to the list.
	* @param the string
	* @version 1.0.52
	*/
	virtual void add(lua_State *lua, const std::string & x) = 0;
	/**
	* @brief Removes a string from the list.
	* @param the string
	* @version 1.0.52
	*/
	virtual void remove(lua_State *lua, const std::string & x) = 0;
	/**
	* @return the selected string
	* @version 1.0.52
	*/
	virtual std::string getSelection(lua_State *lua) = 0;
	/**
	* @return the number of elements in the list
	* @version 1.0.52
	*/
	virtual int getNumElements(lua_State *lua) = 0;
	/**
	* @return the element at i
	* @note according to lua the first entry is at index 1
	* @param the index
	* @version 1.0.52
	*/
	virtual std::string getElementAt(lua_State *lua, int i) = 0;
	/**
	* @brief adds a button to window
	* @param the button text
	* @param the lua expression which will be executed when 
	* 	button was pressed
	* @version 1.0.52
	*/
	virtual void addButton(lua_State *lua, const std::string & text, const std::string & luaExpr) = 0;
	/**
	* @brief Adds a listener which is called
	*        when selection was changed
	* @param a valid lua expression
	* @version 1.0.52
	*/
	virtual void addSelectionListener(lua_State *lua, const std::string & expr) = 0;
	/**
	* @return the selected index starting with 1. Returns 0 when nothing selected.
	* @version 1.0.52
	*/
	virtual int getSelectedIndex(lua_State *lua) = 0;
	/**
	* @brief Removes the element at index i.
	* @param the index
	* @version 1.0.52
	*/
	virtual void removeElementAt(lua_State *lua, int i) = 0;
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
    virtual ~LuaFrxListWindowBase() {}
private:
public:
}; // LuaFrxListWindowBase
}}

#endif /* SAMBAG_LuaFrxListWindowBase_H */

