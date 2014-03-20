/*
 * ============================================================================
 * LuaFrxConnection.cpp
 *      Author: Johannes Unger
 * ============================================================================
 */

#include "LuaFrxConnection.hpp"
#include <sambag/com/Common.hpp>

namespace frx { namespace scripts {
//=============================================================================
//  Class LuaFrxConnection
//=============================================================================
//-----------------------------------------------------------------------------
void LuaFrxConnection::addLuaFields(lua_State *lua, int index) {
    Super::addLuaFields(lua, index);
}
//-----------------------------------------------------------------------------
LuaFrxConnection::LuaFrxConnection() {
}
//-----------------------------------------------------------------------------
LuaFrxConnection::Ptr
LuaFrxConnection::createAndPush(lua_State *lua,
    ModelObject::Ptr obj, ViewModelMap::Ptr map, const std::string &typeId)
{
    Ptr res(new LuaFrxConnection());
    res->setModelObject(obj);
    res->setViewModelMap(map);
    res->setTypeId(typeId);
    res->createLuaObject(lua, "lua_connection");
    return res;
}
}} // namespace(s)
