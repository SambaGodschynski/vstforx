/*
 * THIS FILE IS AUTO CREATED BY THE LUACPP BUILD PROGRAM
 * EVERY CHANGES WILL BE OVERWRITTEN THE NEXT TIME 
 * THE THIS FILE IS GENERATED  
 *
 * LuaFrxIOBase.cpp
 *
 *  Created on: Tue Apr 29 22:18:01 2014
 *      Author: Samba Godschysnki
 */

#include "LuaFrxIOBase.hpp"

namespace frx { namespace scripts { 
//=============================================================================
//  Class LuaFrxIOBase
//=============================================================================
//-----------------------------------------------------------------------------
void LuaFrxIOBase::addLuaFields(lua_State *lua, int index) 
{
    using namespace sambag::lua;
    Super::addLuaFields(lua, index);
    // register functions
    registerClassFunctions<Functions1, TupleAccessor>(
	lua,
	std::make_tuple(),
	index, 
	getUId() 
	); 

	
    
}
//-----------------------------------------------------------------------------
void LuaFrxIOBase::__lua_gc(lua_State *lua) {
    using namespace sambag::lua;
    unregisterClassFunctions<Functions1>(getUId());
	unregisterClassFunctions<MetaFunctions>(getUId());
	
    Super::__lua_gc(lua);
}

}} // namespace(s)

