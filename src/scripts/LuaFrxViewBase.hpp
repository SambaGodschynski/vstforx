/*
 * THIS FILE IS AUTO CREATED BY THE LUACPP BUILD PROGRAM
 * EVERY CHANGES WILL BE OVERWRITTEN THE NEXT TIME 
 * THE THIS FILE IS GENERATED  
 *
 * LuaFrxViewBase.hpp
 *
 *  Created on: Mon Jun 23 19:18:51 2014
 *      Author: Samba Godschysnki
 */

#ifndef SAMBAG_LuaFrxViewBase_H
#define SAMBAG_LuaFrxViewBase_H

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <sambag/lua/Lua.hpp>
#include <loki/Typelist.h>
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
	SAMBAG_LUA_FTAG(createListWindow, sambag::lua::IgnoreReturn ());
	SAMBAG_LUA_FTAG(addToSelection, void ());
	SAMBAG_LUA_FTAG(clearSelection, void ());
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

	typedef LOKI_TYPELIST_10(Frx_getEntry_Tag, 
	Frx_getExit_Tag, 
	Frx_getByName_Tag, 
	Frx_getByType_Tag, 
	Frx_getSelectedObjects_Tag, 
	Frx_addViewListener_Tag, 
	Frx_removeViewListener_Tag, 
	Frx_setMenu_Tag, 
	Frx_getContextObject_Tag, 
	Frx_createListWindow_Tag) Functions2;

	typedef LOKI_TYPELIST_2(Frx_addToSelection_Tag, 
	Frx_clearSelection_Tag) Functions3;

	
    ///////////////////////////////////////////////////////////////////////////
    /**
	* @brief Creates a new view object and adds it to the view.
	* @hiddenParam mixed whatToAdd
	* @returnType Object
	* @return a @see Object
	* @param can be either an object type id or a view 
	* object what isn't on the view already.
	* <pre>
	* frx.view:add("unknown-plugin.Plugin('/plugins/MyPlugin.vst')") --adding a plugin via location <br>
	* adsr=frx.view:add("internal.ADSRTrigger") -- adding internal plugin via object type id <br>
	* frx.view:add(adsr:getParameters()[1]) -- adding view object parameter <br>
	* </pre>
	* @version 1.0.5
	*/
	virtual sambag::lua::IgnoreReturn add(lua_State *lua) = 0;
	/**
	* @brief Removes an object from view.
	* @hiddenParam Object obj
	* @param the object to remove
	* @version 1.0.5
	*/
	virtual void remove(lua_State *lua) = 0;
	/**
	* @returnType Sequence
	* @return all objects on view
	* @version 1.0.5
	*/
	virtual sambag::lua::IgnoreReturn getObjects(lua_State *lua) = 0;
	/**
	* @brief Connects two view objects.
	* @hiddenParam Object from
	* @hiddenParam Object to
	* @param the "connect from" @see Object
	* @param the "connect to" @see Object
	* @returnType Object
	* @return a @see Connection object or nil if objects are unconnectable.
	* @version 1.0.5
	*/
	virtual sambag::lua::IgnoreReturn connect(lua_State *lua) = 0;
	/**
	* @brief Adds a "free parameter" knob to view.
	* @returnType Parameter
	* @return a @see Parameter object.
	* @version 1.0.5
	*/
	virtual sambag::lua::IgnoreReturn addKnob(lua_State *lua) = 0;
	/**
	* @brief Adds a "host parameter" knob to view.
	* @returnType Parameter
	* @return a @see Parameter object
	* @param the host parameter index
	* @version 1.0.5
	*/
	virtual sambag::lua::IgnoreReturn addHostKnob(lua_State *lua, int index) = 0;
	/**
	* @returnType Tuple
	* @return (x,y) the current view position
	* @version 1.0.5
	*/
	virtual Point getLocation(lua_State *lua) = 0;
	/**
	* @brief Set the view position
	* @version 1.0.5
	*/
	virtual void setLocation(lua_State *lua, float x, float y) = 0;
	/**
	* @returnType Tuple
	* @return (width, height) the current view size
	* @version 1.0.5
	*/
	virtual Point getSize(lua_State *lua) = 0;
	/**
	* @brief Set the view size
	* @version 1.0.5
	*/
	virtual void setSize(lua_State *lua, float width, float height) = 0;
	/**
	* @returnType Object
	* @return the "Entry" @see Object 
	* @version 1.0.5
	*/
	virtual sambag::lua::IgnoreReturn getEntry(lua_State *lua) = 0;
	/**
	* @returnType Object
	* @return the "Exit" @see Object 
	* @version 1.0.5
	*/
	virtual sambag::lua::IgnoreReturn getExit(lua_State *lua) = 0;
	/**
	* @returnType Object
	* @return a @see Object by view name or nil.
	* @version 1.0.5
	*/
	virtual sambag::lua::IgnoreReturn getByName(lua_State *lua, const std::string & name) = 0;
	/**
	* @returnType Sequence
	* @return a sequence of @see Object 
	* @param a object type id. Wildcards are supported.
	* <pre>
	* objs=frx.view:getByType("internal.Volume") --get all volumes <br>
	* objs=frx.view:getByType("internal.*") --get all internal plugins <br>
	* objs=frx.view:getByType("*.Plugin*") --get all (external) plugins <br>
	* </pre>
	* @version 1.0.5
	*/
	virtual sambag::lua::IgnoreReturn getByType(lua_State *lua, const std::string & type) = 0;
	/**
	* @returnType Sequence
	* @return all selected objects
	* @version 1.0.5
	*/
	virtual sambag::lua::IgnoreReturn getSelectedObjects(lua_State *lua) = 0;
	/**
	* @brief Adds a view event listener.
	* <pre>
	* function onViewEvent(evName, evObj)		  <br>
	* 		if evName=="requesting context menu" then <br>
	*		   -- handle event     	       	     	  <br>
	*		return					  <br>
	* end						  <br>
	* frx.view:addViewListener("onViewEvent") 		  
	* </pre>
	* @param a function name which will be called when
	* 	      event appears.
	* @version 1.0.5
	* @note implemented events: <br>
	* 	     &nbsp;&nbsp;<b>"requesting context menu"</b>, called when an objects context menu is opening <br>
	*	     &nbsp;&nbsp;<b>"on saving view state"</b>, called when the view state is going to be saved (there is no "on load" event because
	*	     	 	     	     the script dosen't run while loading) <br>
	*	     &nbsp;&nbsp;<b>"object added"</b>, called when an object was added to view (an "object removing" event is unfortunately missing because it is
	*	     to late at this point to create a lua representation before destroying the actual object)
	*/
	virtual void addViewListener(lua_State *lua, const std::string & callbackFuncName) = 0;
	/**
	* @brief Removes an view listener from view. ( see: @see addViewListener() )
	* @param the listener related to the callback function
	* @version 1.0.5
	*/
	virtual void removeViewListener(lua_State *lua, const std::string & callbackFuncName) = 0;
	/**
	* @brief Set the view context menu.
	* @hiddenParam Table menu
	* @param a menu structure table. A previous installed menu will be overwritten.
	* <pre>
	* menuStructureExample = {	<br>
	*				  {name="A Menu Label"}, <br>
	* 			    	  {name="A Singe Entry", action="aValidLuaExpression()"}, <br>
	* 			    	  {name="A SubMenu", {  <br>
	*     		    	         	        {name="A Single Submenu Entry", action="aValidLuaExpression()"}, <br>
	*     				        	{name="Another Single Submenu Entry", action="aValidLuaExpression()"},	<br>
	*			   		              }	 <br> 
	*			          } <br>
	*                        }	       											
	* </pre> 
	* @version 1.0.5
	*/
	virtual void setMenu(lua_State *lua) = 0;
	/**
	* @returnType Object
	* @return the @see Object of that object 
	* whose context menu is currently shown or nil
	* @version 1.0.5
	*/
	virtual sambag::lua::IgnoreReturn getContextObject(lua_State *lua) = 0;
	/**
	* @brief Creates a @see frx.view.ListWindow object.
	* @returnType ListWindow
	* @return the created @see frx.view.ListWindow object or nil when creation failed.
	* @version 1.0.52
	*/
	virtual sambag::lua::IgnoreReturn createListWindow(lua_State *lua) = 0;
	/**
	* @brief Add given object to selection
	* @hiddenParam Object obj
	* @param the @see Object to add 
	* @version 1.0.53
	*/
	virtual void addToSelection(lua_State *lua) = 0;
	/**
	* @brief Remove all objects from selection.
	* @version 1.0.53
	*/
	virtual void clearSelection(lua_State *lua) = 0;
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

