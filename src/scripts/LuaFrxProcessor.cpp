/*
 * ============================================================================
 * LuaFrxProcessor.cpp
 *      Author: Johannes Unger
 * ============================================================================
 */

#include "LuaFrxProcessor.hpp"
#include <sambag/com/Common.hpp>

namespace frx { namespace scripts {
//=============================================================================
//  Class LuaFrxProcessor
//=============================================================================
//-----------------------------------------------------------------------------
void LuaFrxProcessor::addLuaFields(lua_State *lua, int index) {
    Super::addLuaFields(lua, index);
}
//-----------------------------------------------------------------------------
LuaFrxProcessor::LuaFrxProcessor() {
}
//-----------------------------------------------------------------------------
LuaFrxProcessor::Ptr
LuaFrxProcessor::createAndPush(lua_State *lua,
    ModelObject::Ptr obj, ViewModelMap::Ptr map)
{
    Ptr res(new LuaFrxProcessor());
    res->setModelObject(obj);
    res->setViewModelMap(map);
    res->createLuaObject(lua, "lua_processor");
    return res;
}
}} // namespace(s)
