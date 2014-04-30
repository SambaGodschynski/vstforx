/*
 * THIS FILE IS AUTO CREATED BY THE LUACPP BUILD PROGRAM
 * EVERY CHANGES WILL BE OVERWRITTEN THE NEXT TIME 
 * THE THIS FILE IS GENERATED  
 *
 * LuaFrxParameterConnectionBase.cpp
 *
 *  Created on: Tue Apr 29 22:18:01 2014
 *      Author: Samba Godschysnki
 */

#include "LuaFrxParameterConnectionBase.hpp"

namespace frx { namespace scripts { 
//=============================================================================
//  Class LuaFrxParameterConnectionBase
//=============================================================================
//-----------------------------------------------------------------------------
void LuaFrxParameterConnectionBase::addLuaFields(lua_State *lua, int index) 
{
    using namespace sambag::lua;
    Super::addLuaFields(lua, index);
    // register functions
    registerClassFunctions<Functions1, TupleAccessor>(
	lua,
	boost::make_tuple(boost::bind(&LuaFrxParameterConnectionBase::getOperatorNames, this, lua),
		boost::bind(&LuaFrxParameterConnectionBase::removeOperatorAt, this, lua, _1),
		boost::bind(&LuaFrxParameterConnectionBase::addOperator, this, lua, _1),
		boost::bind(&LuaFrxParameterConnectionBase::getParameters, this, lua)),
	index, 
	getUId() 
	); 

	
    
}
//-----------------------------------------------------------------------------
void LuaFrxParameterConnectionBase::__lua_gc(lua_State *lua) {
    using namespace sambag::lua;
    unregisterClassFunctions<Functions1>(getUId());
	unregisterClassFunctions<MetaFunctions>(getUId());
	
    Super::__lua_gc(lua);
}

}} // namespace(s)

