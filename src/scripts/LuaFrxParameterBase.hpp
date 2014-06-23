/*
 * THIS FILE IS AUTO CREATED BY THE LUACPP BUILD PROGRAM
 * EVERY CHANGES WILL BE OVERWRITTEN THE NEXT TIME 
 * THE THIS FILE IS GENERATED  
 *
 * LuaFrxParameterBase.hpp
 *
 *  Created on: Mon Jun 23 21:27:44 2014
 *      Author: Samba Godschysnki
 */

#ifndef SAMBAG_LuaFrxParameterBase_H
#define SAMBAG_LuaFrxParameterBase_H

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <sambag/lua/Lua.hpp>
#include <loki/Typelist.h>
#include <sambag/lua/ALuaObject.hpp>

#include <scripts/LuaFrxObject.hpp>

namespace frx { namespace scripts { 
//=============================================================================
class LuaFrxParameterBase : public LuaFrxObject {
//=============================================================================
public:
    //-------------------------------------------------------------------------
    typedef LuaFrxObject Super;
    //-------------------------------------------------------------------------
    typedef boost::shared_ptr<LuaFrxParameterBase> Ptr;
    //-------------------------------------------------------------------------
    typedef boost::weak_ptr<LuaFrxParameterBase> WPtr;
    //-------------------------------------------------------------------------
    
private:
protected:
    //-------------------------------------------------------------------------
    LuaFrxParameterBase() {}
    //-------------------------------------------------------------------------
    SAMBAG_LUA_FTAG(setValue, void (float));
	SAMBAG_LUA_FTAG(getValue, float ());
	SAMBAG_LUA_FTAG(addListener, void (std::string));
    typedef LOKI_TYPELIST_3(Frx_setValue_Tag, 
	Frx_getValue_Tag, 
	Frx_addListener_Tag) Functions1;

	
    ///////////////////////////////////////////////////////////////////////////
    /**
	* @brief Set the parameter value.
	* @param the value (0..1)
	* @version 1.0.5
	*/
	virtual void setValue(lua_State *lua, float value) = 0;
	/**
	* @return the parameter value.
	* @version 1.0.5
	*/
	virtual float getValue(lua_State *lua) = 0;
	/**
	* @brief Adds a listener which will be called when 
	*        the parameter value was changed.
	* @param a valid lua expression
	* @version 1.0.52
	*/
	virtual void addListener(lua_State *lua, const std::string & expr) = 0;
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
    virtual ~LuaFrxParameterBase() {}
private:
public:
}; // LuaFrxParameterBase
}}

#endif /* SAMBAG_LuaFrxParameterBase_H */

