/*
 * THIS FILE IS AUTO CREATED BY THE LUACPP BUILD PROGRAM
 * EVERY CHANGES WILL BE OVERWRITTEN THE NEXT TIME 
 * THE THIS FILE IS GENERATED  
 *
 * LuaFrxListWindowBase.cpp
 *
 *  Created on: Thu Jun 12 16:38:43 2014
 *      Author: Samba Godschysnki
 */

#include "LuaFrxListWindowBase.hpp"

namespace frx { namespace scripts { 
//=============================================================================
//  Class LuaFrxListWindowBase
//=============================================================================
//-----------------------------------------------------------------------------
void LuaFrxListWindowBase::addLuaFields(lua_State *lua, int index) 
{
    using namespace sambag::lua;
    Super::addLuaFields(lua, index);
    // register functions
    registerClassFunctions<Functions1, TupleAccessor>(
	lua,
	boost::make_tuple(boost::bind(&LuaFrxListWindowBase::add, this, lua, _1),
		boost::bind(&LuaFrxListWindowBase::remove, this, lua, _1),
		boost::bind(&LuaFrxListWindowBase::getSelection, this, lua),
		boost::bind(&LuaFrxListWindowBase::getNumElements, this, lua),
		boost::bind(&LuaFrxListWindowBase::getElementAt, this, lua, _1),
		boost::bind(&LuaFrxListWindowBase::addButton, this, lua, _1, _2),
		boost::bind(&LuaFrxListWindowBase::addSelectionListener, this, lua, _1),
		boost::bind(&LuaFrxListWindowBase::getSelectedIndex, this, lua),
		boost::bind(&LuaFrxListWindowBase::removeElementAt, this, lua, _1)),
	index, 
	getUId() 
	); 

	
    
}
//-----------------------------------------------------------------------------
void LuaFrxListWindowBase::__lua_gc(lua_State *lua) {
    using namespace sambag::lua;
    unregisterClassFunctions<Functions1>(getUId());
	unregisterClassFunctions<MetaFunctions>(getUId());
	
    Super::__lua_gc(lua);
}

}} // namespace(s)

