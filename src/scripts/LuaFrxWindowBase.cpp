/*
 * THIS FILE IS AUTO CREATED BY THE LUACPP BUILD PROGRAM
 * EVERY CHANGES WILL BE OVERWRITTEN THE NEXT TIME 
 * THE THIS FILE IS GENERATED  
 *
 * LuaFrxWindowBase.cpp
 *
 *  Created on: Wed Jun 11 17:08:28 2014
 *      Author: Samba Godschysnki
 */

#include "LuaFrxWindowBase.hpp"

namespace frx { namespace scripts { 
//=============================================================================
//  Class LuaFrxWindowBase
//=============================================================================
//-----------------------------------------------------------------------------
void LuaFrxWindowBase::addLuaFields(lua_State *lua, int index) 
{
    using namespace sambag::lua;
    Super::addLuaFields(lua, index);
    // register functions
    registerClassFunctions<Functions1, TupleAccessor>(
	lua,
	boost::make_tuple(boost::bind(&LuaFrxWindowBase::open, this, lua, _1, _2),
		boost::bind(&LuaFrxWindowBase::close, this, lua),
		boost::bind(&LuaFrxWindowBase::addCloseListener, this, lua, _1)),
	index, 
	getUId() 
	); 

	
    
}
//-----------------------------------------------------------------------------
void LuaFrxWindowBase::__lua_gc(lua_State *lua) {
    using namespace sambag::lua;
    unregisterClassFunctions<Functions1>(getUId());
	unregisterClassFunctions<MetaFunctions>(getUId());
	
    Super::__lua_gc(lua);
}

}} // namespace(s)

