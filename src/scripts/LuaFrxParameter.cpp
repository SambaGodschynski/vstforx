/*
 * ============================================================================
 * LuaFrxParameter.cpp
 *      Author: Johannes Unger
 * ============================================================================
 */

#include "LuaFrxParameter.hpp"
#include <sambag/com/Common.hpp>
#include <gui/components/FrxParameter.hpp>
#include <processing/IParameter.hpp>

namespace frx { namespace scripts {
//=============================================================================
//  Class LuaFrxParameter
//=============================================================================
//-----------------------------------------------------------------------------
std::string LuaFrxParameter::toString(lua_State * lua) const {
    std::stringstream ss;
    ss << Super::toString(lua) << " " << getValue();
    return ss.str();
}
//-----------------------------------------------------------------------------
void LuaFrxParameter::setValue(lua_State * lua, float v) {
    try {
        using frx::processing::IParameter;
        IParameter::Ptr p =
            boost::dynamic_pointer_cast<IParameter>(getModelObject());
        if (!p) {
            return;
        }
        p->setValue(v);
    } catch(const std::exception &ex) {
        slua::pushLuaError(lua, ex.what());
    } catch(...) {
        slua::pushLuaError(lua, "unknown error");
    }
}
//-----------------------------------------------------------------------------
float LuaFrxParameter::getValue() const {
    using frx::processing::IParameter;
    IParameter::Ptr p =
        boost::dynamic_pointer_cast<IParameter>(getModelObject());
    if (!p) {
        return 0.f;
    }
    return p->getValue();
}
//-----------------------------------------------------------------------------
float LuaFrxParameter::getValue(lua_State * lua) {
    try {
        return getValue();
    } catch(const std::exception &ex) {
        slua::pushLuaError(lua, ex.what());
    } catch(...) {
        slua::pushLuaError(lua, "unknown error");
    }
    return 0.f;
}
//-----------------------------------------------------------------------------
LuaFrxParameter::LuaFrxParameter() {
}
//-----------------------------------------------------------------------------
LuaFrxParameter::Ptr
LuaFrxParameter::createAndPush(lua_State *lua,
    ModelObject::Ptr obj, ViewModelMap::Ptr map, const std::string &typeId)
{
    Ptr res(new LuaFrxParameter());
    res->setModelObject(obj);
    res->setViewModelMap(map);
    res->setTypeId(typeId);
    res->createLuaObject(lua, "lua_frxparameter");
    return res;
}
}} // namespace(s)
