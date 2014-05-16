/*
 * ============================================================================
 * LuaFrxConnection.cpp
 *      Author: Johannes Unger
 * ============================================================================
 */

#include "LuaFrxConnection.hpp"
#include <gui/components/FrxConnection.hpp>
#include <sambag/com/Common.hpp>

namespace frx { namespace scripts {
//=============================================================================
//  Class LuaFrxConnection
//=============================================================================
//-----------------------------------------------------------------------------
void LuaFrxConnection::
pushComponent(lua_State *lua, fgc::FrxComponentPtr vObj) const
{
    ViewModelMap::Ptr map = getViewModelMap();
    std::string type = vObj->getTypeId();
    if (type.empty()) {
        throw std::runtime_error("empty not supported");
    }
    std::string id = com::IdParser(type).namespace_("lua").toString();
    LuaFrxObject::Factory &fac = LuaFrxObject::Factory::instance();
    if (!fac.isRegistered(id)) {
        throw std::runtime_error(id + " not supported");
    }
    LuaFrxObject::Ptr lobj = fac.createAndPush(
        id,
        lua,
        map->getModelObject(vObj),
        map
    );

}
//-----------------------------------------------------------------------------
slua::IgnoreReturn2 LuaFrxConnection::getObjects(lua_State *lua) {
    using namespace frx::gui;
    using namespace frx::gui::components;
    try {
        FrxConnection::Ptr cn =
            boost::dynamic_pointer_cast<FrxConnection>(getViewObject());
        pushComponent(lua, cn->getSrcComponent());
        pushComponent(lua, cn->getDstComponent());
    } catch(const std::exception &ex) {
        slua::pushLuaError(lua, ex.what());
    } catch(...) {
        slua::pushLuaError(lua, "unknown error");
    }
    return slua::IgnoreReturn();
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
