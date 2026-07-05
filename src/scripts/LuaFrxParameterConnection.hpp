/*
 * ============================================================================
 * LuaFrxParameterConnection.h
 *      Author: Johannes Unger
 * ============================================================================
 */
#ifndef FORX_LuaFrxParameterConnection_H
#define FORX_LuaFrxParameterConnection_H

#include <memory>
#include "LuaFrxConnection.hpp"
#include <gui/ViewFactory.hpp>
#include <com/one4All.h>
#include <sambag/lua/LuaSequence.hpp>
#include "LuaFrxParameterConnectionBase.hpp"
namespace frx { namespace scripts {
//=============================================================================
/** 
  * @class LuaFrxParameterConnection.
  */
class LuaFrxParameterConnection : public LuaFrxParameterConnectionBase {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef std::shared_ptr<LuaFrxParameterConnection> Ptr;
    //-------------------------------------------------------------------------
    typedef LuaFrxConnection Super;
protected:
    ///////////////////////////////////////////////////////////////////////////
    slua::LuaSequence<std::string> getOperatorNames(lua_State *lua);
    void removeOperatorAt(lua_State *lua, int index);
    void addOperator(lua_State *lua, const std::string &opName);
    slua::IgnoreReturn getParameters(lua_State *lua);
private:
public:
    //-------------------------------------------------------------------------
    static Ptr createAndPush(lua_State * lua,
        ModelObject::Ptr obj, ViewModelMap::Ptr map, const std::string &typeId);
}; // LuaFrxParameterConnection

namespace {
    inline bool registerAllParameterConnections() {
        LuaFrxObject::Factory &fac = LuaFrxObject::Factory::instance();
        return fac.registerCreator("frx.lua.connection.Parameter",
            boost::bind(&LuaFrxParameterConnection::createAndPush, _1, _2, _3, "frx.lua.connection.Parameter"));
    }
    const bool LuaFrxParameterConnections_Registered = registerAllParameterConnections();
}


}} // namespace(s)
#endif  // FORX_LuaFrxParameterConnection_H


