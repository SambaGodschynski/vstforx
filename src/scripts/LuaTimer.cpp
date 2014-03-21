/*
 * LuaTimer.cpp
 *
 *  Created on: Fri Mar 14 11:47:35 2014
 *      Author: Johannes Unger
 */

#include "LuaTimer.hpp"
#include <sambag/com/exceptions/IllegalStateException.hpp>
#include <gui/components/FrxCircuidView.hpp>
#include <gui/IFrxControl.hpp>
#include <gui/HandyNamespaces.hpp>
#include <gui/components/FrxComponent.hpp>
#include <com/one4All.h>

namespace frx { namespace scripts {
//=============================================================================
//  Class LuaTimer
//=============================================================================
//-----------------------------------------------------------------------------
void LuaTimer::__onTimer(lua_State *lua, const std::string &luaCallback) {
    try {
        SAMBAG_TRY_TO_LOCK_RECURSIVE(mutex)
        slua::executeString(lua, luaCallback.c_str());
    } catch(...) {
        SAMBAG_LOG_ERR<<"script ctrl. on timer failed.";
    }
}
//-----------------------------------------------------------------------------
LuaTimer::LuaTimer(Mutex &mutex) : mutex(mutex) {
}
//-----------------------------------------------------------------------------
void LuaTimer::start(lua_State *lua) {
    timer->start();
}
//-----------------------------------------------------------------------------
void LuaTimer::stop(lua_State *lua) {
    timer->stop();
}
//-----------------------------------------------------------------------------
void LuaTimer::addLuaFields(lua_State *lua, int index) {
    Super::addLuaFields(lua, index);
    using boost::bind;
    
    sambag::lua::registerClassFunctions<Functions,
        sambag::lua::TupleAccessor>
    (
        lua,
        boost::make_tuple(
            bind(&LuaTimer::start, this, lua),
            bind(&LuaTimer::stop, this, lua)
        ),
        index,
        getUId()
    );
} 
//-----------------------------------------------------------------------------
void LuaTimer::__gc(lua_State *lua) {
    timer->stop();
    Super::__gc(lua);
}
//-----------------------------------------------------------------------------
LuaTimer::Ptr LuaTimer::createAndPush(lua_State *lua, Mutex &mutex,
    const std::string &callback, int ms, int numRep)
{
    Ptr res(new LuaTimer(mutex));
    res->timer = Timer::create(ms);
    res->timer->sce::EventSender<Timer::Event>::addTrackedEventListener(
        boost::bind(&LuaTimer::__onTimer, res.get(), lua, callback),
        res
    );
    res->timer->setNumRepetitions(numRep);
    res->createLuaObject(lua, "lua_timer");
    return res;
}
}} // namespace(s)
