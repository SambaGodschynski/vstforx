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
#include "LuaFrxConnectionBase.hpp"

namespace frx { namespace scripts {
//=============================================================================
/** 
  * @class LuaFrxConnection.
  */
class LuaFrxConnection : public LuaFrxConnectionBase {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<LuaFrxConnection> Ptr;
    //-------------------------------------------------------------------------
    typedef LuaFrxObject Super;
protected:
    //-------------------------------------------------------------------------
    LuaFrxConnection();
    ///////////////////////////////////////////////////////////////////////////
    slua::IgnoreReturn2 getObjects(lua_State *lua);
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
            boost::bind(&LuaFrxConnection::createAndPush, _1, _2, _3, "frx.lua.connection.IO"));
    }
    const bool LuaFrxConnections_Registered = registerAllConnections();
}


}} // namespace(s)
#endif  // FORX_LuaFrxConnection_H


