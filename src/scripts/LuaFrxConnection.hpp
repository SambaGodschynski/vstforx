/*
 * ============================================================================
 * LuaFrxConnection.h
 *      Author: Johannes Unger
 * ============================================================================
 */
#ifndef FORX_LuaFrxConnection_H
#define FORX_LuaFrxConnection_H

#include <boost/shared_ptr.hpp>
#include "LuaFrxObject.hpp"
#include <gui/ViewFactory.hpp>
#include <com/one4All.h>

namespace frx { namespace scripts {
//=============================================================================
/** 
  * @class LuaFrxConnection.
  */
class LuaFrxConnection : public LuaFrxObject {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<LuaFrxConnection> Ptr;
    //-------------------------------------------------------------------------
    typedef LuaFrxObject Super;
protected:
    //-------------------------------------------------------------------------
    virtual void addLuaFields(lua_State * lua, int index);
    //-------------------------------------------------------------------------
    LuaFrxConnection();
    SAMBAG_LUA_FTAG(getObjects, slua::IgnoreReturn2());
    ///////////////////////////////////////////////////////////////////////////
    typedef LOKI_TYPELIST_1(Frx_getObjects_Tag
    ) Functions1;
    slua::IgnoreReturn2 getObjects(lua_State *lua) const;
private:
    //-------------------------------------------------------------------------
    void pushComponent(lua_State *lua, fgc::FrxComponentPtr c) const;
public:
    //-------------------------------------------------------------------------
    static Ptr createAndPush(lua_State * lua,
        ModelObject::Ptr obj, ViewModelMap::Ptr map, const std::string &typeId);
}; // LuaFrxConnection

namespace {
    inline bool registerAllConnections() {
        LuaFrxObject::Factory &fac = LuaFrxObject::Factory::instance();
        return fac.registerCreator("frx.lua.connection.IO",
            boost::bind(&LuaFrxConnection::createAndPush, _1, _2, _3, "frx.lua.connection.IO"))
        && fac.registerCreator("frx.lua.connection.Parameter",
            boost::bind(&LuaFrxConnection::createAndPush, _1, _2, _3, "frx.lua.connection.Parameter"));
    }
    const bool LuaFrxConnections_Registered = registerAllConnections();
}


}} // namespace(s)
#endif  // FORX_LuaFrxConnection_H


