/*
 * ============================================================================
 * LuaFrxIO.cpp
 *      Author: Johannes Unger
 * ============================================================================
 */

#include "LuaFrxIO.hpp"
#include <sambag/com/Common.hpp>

namespace frx { namespace scripts {
//=============================================================================
//  Class LuaFrxIO
//=============================================================================
//-----------------------------------------------------------------------------
void LuaFrxIO::addLuaFields(lua_State *lua, int index) {
    Super::addLuaFields(lua, index);
}
//-----------------------------------------------------------------------------
LuaFrxIO::LuaFrxIO() {
}
//-----------------------------------------------------------------------------
LuaFrxIO::Ptr
LuaFrxIO::createAndPush(lua_State *lua,
    ModelObject::Ptr obj, ViewModelMap::Ptr map)
{
    Ptr res(new LuaFrxIO());
    res->setModelObject(obj);
    res->setViewModelMap(map);
    res->createLuaObject(lua, "lua_io");
    return res;
}
}} // namespace(s)
