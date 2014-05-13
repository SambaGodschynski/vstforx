/*
 * LuaTimer.hpp
 *
 *  Created on: Fri Mar 14 11:47:35 2014
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_LuaTimer_H
#define SAMBAG_LuaTimer_H

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <sambag/lua/ALuaObject.hpp>
#include <boost/function.hpp>
#include <processing/FrxAsyncDSPTimer.hpp>
#include <sambag/com/Thread.hpp>
#include <sambag/com/events/Events.hpp>
#include "LuaTimerBase.hpp"

namespace frx { namespace scripts {
namespace slua = sambag::lua;
struct TimerExecFailed {
    std::string msg;
    TimerExecFailed(const std::string &msg) : msg(msg) {}
};
//=============================================================================
/** 
  * @class LuaTimer.
  */
class LuaTimer : public LuaTimerBase,
    public sambag::com::events::EventSender<TimerExecFailed>
{
//=============================================================================
public:
    //-------------------------------------------------------------------------
    typedef sambag::com::events::EventSender<TimerExecFailed> ExecFailedSender;
    //-------------------------------------------------------------------------
    typedef TimerExecFailed ExecFailedEvent;
    //-------------------------------------------------------------------------
    typedef slua::ALuaObject Super;
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<LuaTimer> Ptr;
private:
    //-------------------------------------------------------------------------
    typedef frx::processing::FrxAsyncDSPTimer Timer;
    Timer::Ptr timer;
    //-------------------------------------------------------------------------
    typedef sambag::com::RecursiveMutex Mutex;
    Mutex &mutex;
    //-------------------------------------------------------------------------
    void __onTimer(sambag::lua::LuaStateWRef _lua, const std::string &luaCallback);
protected:
    //-------------------------------------------------------------------------
    LuaTimer(Mutex &mutex);
    ///////////////////////////////////////////////////////////////////////////
    // lua2frx impl
    void start(lua_State *lua);
    void stop(lua_State *lua);
public:
    //-------------------------------------------------------------------------
    virtual ~LuaTimer() {}
    //-------------------------------------------------------------------------
    /**
     * @brief creates a lua timer
     * @param the lua state
     * @param to keep the lua state in sync we need a mutex object
     * @param the lua callback command
     * @param timer time in milliseconds
     * @param timer number of repetitions
     */
    typedef boost::weak_ptr<void> Tracker;
    static Ptr createAndPush(sambag::lua::LuaStateWRef lua,
        Tracker tracker,
        Mutex &mutex,
        const std::string &callback,
        int ms, int numRep);
}; // LuaTimer
}} // namespace(s)

#endif /* SAMBAG_LuaTimer_H */
