/*
 * THIS FILE IS AUTO CREATED BY THE LUACPP BUILD PROGRAM
 * EVERY CHANGES WILL BE OVERWRITTEN THE NEXT TIME 
 * THE THIS FILE IS GENERATED  
 *
 * LuaParameterBase.cpp
 *
 *  Created on: Tue Apr 29 22:18:02 2014
 *      Author: Samba Godschysnki
 */

#include "LuaParameterBase.hpp"

namespace frx { namespace scripts { 
//=============================================================================
//  Class LuaParameterBase
//=============================================================================
//-----------------------------------------------------------------------------
void LuaParameterBase::addLuaFields(lua_State *lua, int index) 
{
    using namespace sambag::lua;
    Super::addLuaFields(lua, index);
    // register functions
    registerClassFunctions<Functions1, TupleAccessor>(
	lua,
	boost::make_tuple(boost::bind(&LuaParameterBase::setValue, this, lua, _1),
		boost::bind(&LuaParameterBase::getValue, this, lua),
		boost::bind(&LuaParameterBase::getName, this, lua)),
	index, 
	getUId() 
	); 

	
    
}
//-----------------------------------------------------------------------------
void LuaParameterBase::__lua_gc(lua_State *lua) {
    using namespace sambag::lua;
    unregisterClassFunctions<Functions1>(getUId());
	unregisterClassFunctions<MetaFunctions>(getUId());
	
    Super::__lua_gc(lua);
}

}} // namespace(s)

