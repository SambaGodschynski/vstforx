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
    ss << Super::toString(lua) << " " << getValue(lua);
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
float LuaFrxParameter::getValue(lua_State * lua) const {
    try {
        using frx::processing::IParameter;
        IParameter::Ptr p =
            boost::dynamic_pointer_cast<IParameter>(getModelObject());
        if (!p) {
            return 0.f;
        }
        return p->getValue();
    } catch(const std::exception &ex) {
        slua::pushLuaError(lua, ex.what());
    } catch(...) {
        slua::pushLuaError(lua, "unknown error");
    }
    return 0.f;
}
//-----------------------------------------------------------------------------
void LuaFrxParameter::addLuaFields(lua_State *lua, int index) {
    Super::addLuaFields(lua, index);
    using boost::bind;
    sambag::lua::registerClassFunctions<Functions,
        sambag::lua::TupleAccessor>
    (
        lua,
        boost::make_tuple(
            bind(&LuaFrxParameter::setValue, this, lua, _1),
            bind(&LuaFrxParameter::getValue, this, lua)
        ),
        index,
        getUId()
    );

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
