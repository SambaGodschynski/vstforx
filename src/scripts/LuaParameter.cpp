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
float LuaParameter::getValue(lua_State * lua) {
    using ::processing::parameter::Parameter;
    try {
        return getValue();
    } catch(const std::exception &ex) {
        slua::pushLuaError(lua, ex.what());
    } catch(...) {
        slua::pushLuaError(lua, "unknown error");
    }
    return 0;
}
//-----------------------------------------------------------------------------
float LuaParameter::getValue() const {
    using ::processing::parameter::Parameter;
    Parameter::Ptr x =
        boost::dynamic_pointer_cast<Parameter>(getModelObject());
    return x->getValue();
}
//-----------------------------------------------------------------------------
std::string LuaParameter::getName() const {
    using ::processing::parameter::Parameter;
    Parameter::Ptr x =
        boost::dynamic_pointer_cast<Parameter>(getModelObject());
    return x->getName();
}
//-----------------------------------------------------------------------------
std::string LuaParameter::getName(lua_State * lua) {
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
    ss<<getName()<<" "<<getValue();
    return ss.str();
}
}} // namespace(s)
