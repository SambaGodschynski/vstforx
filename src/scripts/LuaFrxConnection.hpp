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
private:
public:
    //-------------------------------------------------------------------------
    static Ptr createAndPush(lua_State * lua,
        ModelObject::Ptr obj, ViewModelMap::Ptr map);
}; // LuaFrxConnection

namespace {
    inline bool registerAllConnections() {
        LuaFrxObject::Factory &fac = LuaFrxObject::Factory::instance();
        return fac.registerCreator("frx.lua.connection.IO", &LuaFrxConnection::createAndPush)
        && fac.registerCreator("frx.lua.connection.Parameter", &LuaFrxConnection::createAndPush);
    }
    const bool LuaFrxConnections_Registered = registerAllConnections();
}


}} // namespace(s)
#endif  // FORX_LuaFrxConnection_H


