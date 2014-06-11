/*
 * THIS FILE IS AUTO CREATED BY THE LUACPP BUILD PROGRAM
 * EVERY CHANGES WILL BE OVERWRITTEN THE NEXT TIME 
 * THE THIS FILE IS GENERATED  
 *
 * LuaFrxViewBase.cpp
 *
 *  Created on: Wed Jun 11 12:45:31 2014
 *      Author: Samba Godschysnki
 */

#include "LuaFrxViewBase.hpp"

namespace frx { namespace scripts { 
//=============================================================================
//  Class LuaFrxViewBase
//=============================================================================
//-----------------------------------------------------------------------------
void LuaFrxViewBase::addLuaFields(lua_State *lua, int index) 
{
    using namespace sambag::lua;
    Super::addLuaFields(lua, index);
    // register functions
    registerClassFunctions<Functions1, TupleAccessor>(
	lua,
	boost::make_tuple(boost::bind(&LuaFrxViewBase::add, this, lua),
		boost::bind(&LuaFrxViewBase::remove, this, lua),
		boost::bind(&LuaFrxViewBase::getObjects, this, lua),
		boost::bind(&LuaFrxViewBase::connect, this, lua),
		boost::bind(&LuaFrxViewBase::addKnob, this, lua),
		boost::bind(&LuaFrxViewBase::addHostKnob, this, lua, _1),
		boost::bind(&LuaFrxViewBase::getLocation, this, lua),
		boost::bind(&LuaFrxViewBase::setLocation, this, lua, _1, _2),
		boost::bind(&LuaFrxViewBase::getSize, this, lua),
		boost::bind(&LuaFrxViewBase::setSize, this, lua, _1, _2)),
	index, 
	getUId() 
	); 

	registerClassFunctions<Functions2, TupleAccessor>(
	lua,
	boost::make_tuple(boost::bind(&LuaFrxViewBase::getEntry, this, lua),
		boost::bind(&LuaFrxViewBase::getExit, this, lua),
		boost::bind(&LuaFrxViewBase::getByName, this, lua, _1),
		boost::bind(&LuaFrxViewBase::getByType, this, lua, _1),
		boost::bind(&LuaFrxViewBase::getSelectedObjects, this, lua),
		boost::bind(&LuaFrxViewBase::addViewListener, this, lua, _1),
		boost::bind(&LuaFrxViewBase::removeViewListener, this, lua, _1),
		boost::bind(&LuaFrxViewBase::setMenu, this, lua),
		boost::bind(&LuaFrxViewBase::getContextObject, this, lua),
		boost::bind(&LuaFrxViewBase::createListWindow, this, lua)),
	index, 
	getUId() 
	); 

	
    
}
//-----------------------------------------------------------------------------
void LuaFrxViewBase::__lua_gc(lua_State *lua) {
    using namespace sambag::lua;
    unregisterClassFunctions<Functions1>(getUId());
	unregisterClassFunctions<Functions2>(getUId());
	unregisterClassFunctions<MetaFunctions>(getUId());
	
    Super::__lua_gc(lua);
}

}} // namespace(s)

