/*
 * LuaUserData.cpp
 *
 *  Created on: Fri Aug 29 10:43:36 2014
 *      Author: Johannes Unger
 */

#include "LuaUserData.hpp"
#include <sambag/lua/LuaHelper.hpp>

namespace frx { namespace scripts {
//=============================================================================
//  Class LuaUserData
//=============================================================================
//-----------------------------------------------------------------------------
void LuaUserData::add(const std::string &key, const std::string &value) {
    container.add(key+".data", value);
}
//-----------------------------------------------------------------------------
void LuaUserData::remove(const std::string &key) {
    try {
        container.get_child(key).clear();
    } catch (const ContainerEx &) {
    }
}
//-----------------------------------------------------------------------------
void LuaUserData::clear() {
    try {
        container.clear();
    } catch (const ContainerEx &) {
    }
}
//-----------------------------------------------------------------------------
void LuaUserData::add(lua_State *lua) {
    namespace slua = sambag::lua;
    if(!lua_isstring(lua,  -2)) {
        throw std::runtime_error("arguments mismatch");
    }
    std::string key( lua_tostring(lua,  -2) );
    // first remove old values
    remove(key);
    
    if(!lua_istable(lua,  -1)) {
        throw std::runtime_error("arguments mismatch");
    }
    int index = -1;
    lua_pushnil(lua); /* first key */
    --index;
    while (lua_next(lua,  index) != 0) {
        boost::tuple<std::string> value;
        slua::pop(lua,  value);
        add(key, boost::get<0>(value));
    }

}
}} // namespace(s)
