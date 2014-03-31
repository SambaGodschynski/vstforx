/*
 * LuaParameter.cpp
 *
 *  Created on: Fri Mar 14 11:47:45 2014
 *      Author: Johannes Unger
 */

#include "LuaParameter.hpp"
#include <sambag/com/Common.hpp>
#include <processing/parameter/parameter.h>
#include <sambag/com/exceptions/IllegalStateException.hpp>

namespace frx { namespace scripts {
//=============================================================================
//  Class LuaParameter
//=============================================================================
//-----------------------------------------------------------------------------
void LuaParameter::__lua_gc(lua_State *lua) {
    slua::unregisterClassFunctions<Functions1>(getUId());
    Super::__lua_gc(lua);
}
//-----------------------------------------------------------------------------
void LuaParameter::addLuaFields(lua_State * lua, int index) {
    Super::addLuaFields(lua, index);
    using boost::bind;
    sambag::lua::registerClassFunctions<Functions1,
        sambag::lua::TupleAccessor>
    (
        lua,
        boost::make_tuple(
            bind(&LuaParameter::setValue, this, lua, _1),
            bind(&LuaParameter::getValue, this, lua),
            bind(&LuaParameter::getName, this, lua)
        ),
        index,
        getUId()
    );
}
//-----------------------------------------------------------------------------
LuaParameter::LuaParameter() {
}
//-----------------------------------------------------------------------------
LuaParameter::Ptr
LuaParameter::createAndPush(lua_State * lua, ModelObject::Ptr obj) {
    Ptr res(new LuaParameter());
    res->setTypeId("frx.lua.parameter.Parameter");
    res->createLuaObject(lua, "lua_mparameter");
    res->setModelObject(obj);
    return res;
}
//-----------------------------------------------------------------------------
void LuaParameter::setValue(lua_State * lua, float v) {
    using ::processing::parameter::Parameter;
    try {
        Parameter::Ptr x =
            boost::dynamic_pointer_cast<Parameter>(getModelObject());
        return x->setValue(v);
    } catch(const std::exception &ex) {
        slua::pushLuaError(lua, ex.what());
    } catch(...) {
        slua::pushLuaError(lua, "unknown error");
    }
}
//-----------------------------------------------------------------------------
float LuaParameter::getValue(lua_State * lua) const {
    using ::processing::parameter::Parameter;
    try {
        Parameter::Ptr x =
            boost::dynamic_pointer_cast<Parameter>(getModelObject());
        return x->getValue();
    } catch(const std::exception &ex) {
        slua::pushLuaError(lua, ex.what());
    } catch(...) {
        slua::pushLuaError(lua, "unknown error");
    }
    return 0;
}
//-----------------------------------------------------------------------------
std::string LuaParameter::getName(lua_State * lua) const {
    using ::processing::parameter::Parameter;
    try {
        Parameter::Ptr x =
            boost::dynamic_pointer_cast<Parameter>(getModelObject());
        return x->getName();
    } catch(const std::exception &ex) {
        slua::pushLuaError(lua, ex.what());
    } catch(...) {
        slua::pushLuaError(lua, "unknown error");
    }
    return "";
}
//-----------------------------------------------------------------------------
std::string LuaParameter::toString(lua_State * lua) const {
    std::stringstream ss;
    ss<<getName(lua)<<" "<<getValue(lua);
    return ss.str();
}
}} // namespace(s)
