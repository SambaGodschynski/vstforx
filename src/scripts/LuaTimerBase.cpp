/*
 * THIS FILE IS AUTO CREATED BY THE LUACPP BUILD PROGRAM
 * EVERY CHANGES WILL BE OVERWRITTEN THE NEXT TIME 
 * THE THIS FILE IS GENERATED  
 *
 * LuaTimerBase.cpp
 *
 *  Created on: Tue Apr 29 22:18:03 2014
 *      Author: Samba Godschysnki
 */

#include "LuaTimerBase.hpp"

namespace frx { namespace scripts { 
//=============================================================================
//  Class LuaTimerBase
//=============================================================================
//-----------------------------------------------------------------------------
void LuaTimerBase::addLuaFields(lua_State *lua, int index) 
{
    using namespace sambag::lua;
    Super::addLuaFields(lua, index);
    // register functions
    registerClassFunctions<Functions1, TupleAccessor>(
	lua,
	boost::make_tuple(boost::bind(&LuaTimerBase::start, this, lua),
		boost::bind(&LuaTimerBase::stop, this, lua)),
	index, 
	getUId() 
	); 

	
    
}
//-----------------------------------------------------------------------------
void LuaTimerBase::__lua_gc(lua_State *lua) {
    using namespace sambag::lua;
    unregisterClassFunctions<Functions1>(getUId());
	unregisterClassFunctions<MetaFunctions>(getUId());
	
    Super::__lua_gc(lua);
}

}} // namespace(s)

