/*
 * THIS FILE IS AUTO CREATED BY THE LUACPP BUILD PROGRAM
 * EVERY CHANGES WILL BE OVERWRITTEN THE NEXT TIME 
 * THE THIS FILE IS GENERATED  
 *
 * LuaImplBase.cpp
 *
 *  Created on: Wed Jun  4 14:35:18 2014
 *      Author: Samba Godschysnki
 */

#include "LuaImplBase.hpp"

namespace frx { namespace processing { 
//=============================================================================
//  Class LuaImplBase
//=============================================================================
//-----------------------------------------------------------------------------
void LuaImplBase::addLuaFields(lua_State *lua, int index) 
{
    using namespace sambag::lua;
    Super::addLuaFields(lua, index);
    // register functions
    registerClassFunctions<Functions1, TupleAccessor>(
	lua,
	boost::make_tuple(boost::bind(&LuaImplBase::log, this, lua),
		boost::bind(&LuaImplBase::logWarn, this, lua),
		boost::bind(&LuaImplBase::logErr, this, lua),
		boost::bind(&LuaImplBase::logTrace, this, lua),
		boost::bind(&LuaImplBase::sendMidi, this, lua),
		boost::bind(&LuaImplBase::getChannel, this, lua, _1),
		boost::bind(&LuaImplBase::fft, this, lua),
		boost::bind(&LuaImplBase::setChannel, this, lua),
		boost::bind(&LuaImplBase::getSamplePos, this, lua),
		boost::bind(&LuaImplBase::getBarStartPos, this, lua)),
	index, 
	getUId() 
	); 

	registerClassFunctions<Functions2, TupleAccessor>(
	lua,
	boost::make_tuple(boost::bind(&LuaImplBase::getPpqPos, this, lua),
		boost::bind(&LuaImplBase::getTimeSigNumerator, this, lua),
		boost::bind(&LuaImplBase::getTimeSigDenominator, this, lua),
		boost::bind(&LuaImplBase::getTempo, this, lua),
		boost::bind(&LuaImplBase::transportIsPlaying, this, lua),
		boost::bind(&LuaImplBase::setParameterValue, this, lua, _1, _2),
		boost::bind(&LuaImplBase::setParameterDisplay, this, lua, _1, _2),
		boost::bind(&LuaImplBase::getParameterValue, this, lua, _1),
		boost::bind(&LuaImplBase::getParameterDisplay, this, lua, _1),
		boost::bind(&LuaImplBase::addParameterListener, this, lua, _1, _2)),
	index, 
	getUId() 
	); 

	registerClassFunctions<Functions3, TupleAccessor>(
	lua,
	boost::make_tuple(boost::bind(&LuaImplBase::removeParameterListener, this, lua, _1, _2),
		boost::bind(&LuaImplBase::getPersistUserData, this, lua, _1),
		boost::bind(&LuaImplBase::setPersistUserData, this, lua)),
	index, 
	getUId() 
	); 

	
    
}
//-----------------------------------------------------------------------------
void LuaImplBase::__lua_gc(lua_State *lua) {
    using namespace sambag::lua;
    unregisterClassFunctions<Functions1>(getUId());
	unregisterClassFunctions<Functions2>(getUId());
	unregisterClassFunctions<Functions3>(getUId());
	unregisterClassFunctions<MetaFunctions>(getUId());
	
    Super::__lua_gc(lua);
}

}} // namespace(s)

