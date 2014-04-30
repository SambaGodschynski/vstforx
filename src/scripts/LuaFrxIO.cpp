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
LuaFrxIO::LuaFrxIO() {
}
//-----------------------------------------------------------------------------
LuaFrxIO::Ptr
LuaFrxIO::createAndPush(lua_State *lua,
    ModelObject::Ptr obj, ViewModelMap::Ptr map, const std::string &typeId)
{
    Ptr res(new LuaFrxIO());
    res->setModelObject(obj);
    res->setViewModelMap(map);
    res->setTypeId(typeId);
    res->createLuaObject(lua, "lua_io");
    return res;
}
}} // namespace(s)
