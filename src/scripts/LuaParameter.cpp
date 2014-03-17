/*
 * LuaParameter.cpp
 *
 *  Created on: Fri Mar 14 11:47:45 2014
 *      Author: Johannes Unger
 */

#include "LuaParameter.hpp"
#include <sambag/com/Common.hpp>

namespace frx { namespace scripts {
//=============================================================================
//  Class LuaParameter
//=============================================================================
//-----------------------------------------------------------------------------
void LuaParameter::setValue(float v) {
    SAMBAG_LOG_TRACE<<v;
}
//-----------------------------------------------------------------------------
void LuaParameter::addLuaFields(lua_State * lua, int index) {
}
//-----------------------------------------------------------------------------
LuaParameter::LuaParameter() {
}
//-----------------------------------------------------------------------------
LuaParameter::Ptr
LuaParameter::create(lua_State * lua, const std::string &name) {
    Ptr res(new LuaParameter());
    res->createLuaObject(lua, name);
    return res;
}
}} // namespace(s)
