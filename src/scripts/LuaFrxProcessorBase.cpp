/*
 * THIS FILE IS AUTO CREATED BY THE LUACPP BUILD PROGRAM
 * EVERY CHANGES WILL BE OVERWRITTEN THE NEXT TIME 
 * THE THIS FILE IS GENERATED  
 *
 * LuaFrxProcessorBase.cpp
 *
 *  Created on: Wed Dec  7 21:59:39 2016
 *      Author: Samba Godschysnki
 */

#include "LuaFrxProcessorBase.hpp"

namespace frx { namespace scripts { 
//=============================================================================
//  Class LuaFrxProcessorBase
//=============================================================================
//-----------------------------------------------------------------------------
void LuaFrxProcessorBase::addLuaFields(lua_State *lua, int index) 
{
    using namespace sambag::lua;
    Super::addLuaFields(lua, index);
    // register functions
    registerClassFunctions<Functions1, TupleAccessor>(
	lua,
	boost::make_tuple(boost::bind(&LuaFrxProcessorBase::getInputs, this, lua),
		boost::bind(&LuaFrxProcessorBase::getOutputs, this, lua),
		boost::bind(&LuaFrxProcessorBase::getParameters, this, lua),
		boost::bind(&LuaFrxProcessorBase::getNumParameters, this, lua),
		boost::bind(&LuaFrxProcessorBase::addInput, this, lua, _1),
		boost::bind(&LuaFrxProcessorBase::addOutput, this, lua, _1),
		boost::bind(&LuaFrxProcessorBase::openCloseEditor, this, lua),
		boost::bind(&LuaFrxProcessorBase::getNumInputs, this, lua),
		boost::bind(&LuaFrxProcessorBase::getNumOutputs, this, lua),
		boost::bind(&LuaFrxProcessorBase::getPluginLocation, this, lua)),
	index, 
	getUId() 
	); 

	registerClassFunctions<Functions2, TupleAccessor>(
	lua,
	boost::make_tuple(boost::bind(&LuaFrxProcessorBase::sendMessage, this, lua, _1),
		boost::bind(&LuaFrxProcessorBase::serialize, this, lua),
		boost::bind(&LuaFrxProcessorBase::deserialize, this, lua, _1)),
	index, 
	getUId() 
	); 

	
    
}
//-----------------------------------------------------------------------------
void LuaFrxProcessorBase::__lua_gc(lua_State *lua) {
    using namespace sambag::lua;
    unregisterClassFunctions<Functions1>(getUId());
	unregisterClassFunctions<Functions2>(getUId());
	unregisterClassFunctions<MetaFunctions>(getUId());
	
    Super::__lua_gc(lua);
}

}} // namespace(s)

