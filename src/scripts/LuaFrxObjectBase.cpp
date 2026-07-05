/*
 * THIS FILE IS AUTO CREATED BY THE LUACPP BUILD PROGRAM
 * EVERY CHANGES WILL BE OVERWRITTEN THE NEXT TIME 
 * THE THIS FILE IS GENERATED  
 *
 * LuaFrxObjectBase.cpp
 *
 *  Created on: Tue Apr 29 22:18:01 2014
 *      Author: Samba Godschysnki
 */

#include "LuaFrxObjectBase.hpp"

namespace frx { namespace scripts { 
//=============================================================================
//  Class LuaFrxObjectBase
//=============================================================================
//-----------------------------------------------------------------------------
void LuaFrxObjectBase::addLuaFields(lua_State *lua, int index) 
{
    using namespace sambag::lua;
    Super::addLuaFields(lua, index);
    // register functions
    registerClassFunctions<Functions1, TupleAccessor>(
	lua,
	std::make_tuple(boost::bind(&LuaFrxObjectBase::getLocation, this, lua),
		boost::bind(&LuaFrxObjectBase::setLocation, this, lua, _1, _2),
		boost::bind(&LuaFrxObjectBase::getSize, this, lua),
		boost::bind(&LuaFrxObjectBase::setSize, this, lua, _1, _2),
		boost::bind(&LuaFrxObjectBase::setName, this, lua, _1),
		boost::bind(&LuaFrxObjectBase::getName, this, lua),
		boost::bind(&LuaFrxObjectBase::getTypeId, this, lua),
		boost::bind(&LuaFrxObjectBase::setMenu, this, lua),
		boost::bind(&LuaFrxObjectBase::getViewId, this, lua)),
	index, 
	getUId() 
	); 

	
    
}
//-----------------------------------------------------------------------------
void LuaFrxObjectBase::__lua_gc(lua_State *lua) {
    using namespace sambag::lua;
    unregisterClassFunctions<Functions1>(getUId());
	unregisterClassFunctions<MetaFunctions>(getUId());
	
    Super::__lua_gc(lua);
}

}} // namespace(s)

