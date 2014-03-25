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

namespace frx { namespace scripts {
namespace slua = sambag::lua;
//=============================================================================
/** 
  * @class LuaTimer.
  */
class LuaTimer : public slua::ALuaObject {
//=============================================================================
public:
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
    //-------------------------------------------------------------------------
    /**
     * @brief called when lua object will be removed.
     */
    virtual void __gc(lua_State *lua);
    ///////////////////////////////////////////////////////////////////////////
    SAMBAG_LUA_FTAG(start, void());
    SAMBAG_LUA_FTAG(stop, void());
    typedef LOKI_TYPELIST_2(Frx_start_Tag, Frx_stop_Tag) Functions;
    // lua2frx impl
    void start(lua_State *lua);
    void stop(lua_State *lua);
public:
    //-------------------------------------------------------------------------
    virtual void addLuaFields(lua_State *lua, int index);
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
    static Ptr createAndPush(sambag::lua::LuaStateWRef lua, Mutex &mutex,
        const std::string &callback, int ms, int numRep);
}; // LuaTimer
}} // namespace(s)

#endif /* SAMBAG_LuaTimer_H */
