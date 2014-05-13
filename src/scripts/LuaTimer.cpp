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
void LuaTimer::__onTimer(sambag::lua::LuaStateWRef _lua, const std::string &luaCallback) {
    sambag::lua::LuaStateRef lua = _lua.lock();
    if (!lua) {
        return;
    }
    try {
        SAMBAG_TRY_TO_LOCK_RECURSIVE(mutex)
        slua::executeString(lua.get(), luaCallback.c_str());
    } catch(const std::exception &ex) {
        SAMBAG_LOG_ERR<<"timer callback failed: "<<ex.what();
        ExecFailedSender::notifyListeners(this,
            ExecFailedEvent(std::string("timer execution failed: ") + ex.what())
        );
    } catch(...) {
        SAMBAG_LOG_ERR<<"timer callback failed.";
        ExecFailedSender::notifyListeners(this, ExecFailedEvent("unknown error"));
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
LuaTimer::Ptr LuaTimer::createAndPush(sambag::lua::LuaStateWRef _lua,
    Tracker tracker,
    Mutex &mutex,
    const std::string &callback,
    int ms, int numRep)
{
    sambag::lua::LuaStateRef lua = _lua.lock();
    if (!lua) {
        throw std::runtime_error("luastate == NULL");
    }
    Ptr res(new LuaTimer(mutex));
    res->timer = Timer::create(ms);
    res->timer->sce::EventSender<Timer::Event>::addTrackedEventListener(
        boost::bind(&LuaTimer::__onTimer, res.get(), _lua, callback),
        tracker
    );
    res->timer->setNumRepetitions(numRep);
    res->createLuaObject(lua.get(), "lua_timer");
    return res;
}
}} // namespace(s)
