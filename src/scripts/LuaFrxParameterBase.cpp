/*
 * THIS FILE IS AUTO CREATED BY THE LUACPP BUILD PROGRAM
 * EVERY CHANGES WILL BE OVERWRITTEN THE NEXT TIME 
 * THE THIS FILE IS GENERATED  
 *
 * LuaFrxParameterBase.cpp
 *
 *  Created on: Mon Jun 23 21:27:51 2014
 *      Author: Samba Godschysnki
 */

#include "LuaFrxParameterBase.hpp"

namespace frx { namespace scripts { 
//=============================================================================
//  Class LuaFrxParameterBase
//=============================================================================
//-----------------------------------------------------------------------------
void LuaFrxParameterBase::addLuaFields(lua_State *lua, int index) 
{
    using namespace sambag::lua;
    Super::addLuaFields(lua, index);
    // register functions
    registerClassFunctions<Functions1, TupleAccessor>(
	lua,
	std::make_tuple(boost::bind(&LuaFrxParameterBase::setValue, this, lua, _1),
		boost::bind(&LuaFrxParameterBase::getValue, this, lua),
		boost::bind(&LuaFrxParameterBase::addListener, this, lua, _1)),
	index, 
	getUId() 
	); 

	
    
}
//-----------------------------------------------------------------------------
void LuaFrxParameterBase::__lua_gc(lua_State *lua) {
    using namespace sambag::lua;
    unregisterClassFunctions<Functions1>(getUId());
	unregisterClassFunctions<MetaFunctions>(getUId());
	
    Super::__lua_gc(lua);
}

}} // namespace(s)

