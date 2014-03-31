/*
 * LuaModelObject.cpp
 *
 *  Created on: Fri Mar 14 11:47:35 2014
 *      Author: Johannes Unger
 */

#include "LuaModelObject.hpp"
#include <sambag/com/exceptions/IllegalStateException.hpp>
#include <gui/components/FrxCircuidView.hpp>
#include <gui/IFrxControl.hpp>
#include <gui/HandyNamespaces.hpp>
#include <gui/components/FrxComponent.hpp>
#include <com/one4All.h>

namespace frx { namespace scripts {
//=============================================================================
//  Class LuaModelObject
//=============================================================================
//-----------------------------------------------------------------------------
LuaModelObject::Ptr LuaModelObject::getFromLuaStack(lua_State *lua, int index) {
    if (!lua_istable(lua, index)) {
        SAMBAG_THROW(
            sambag::com::exceptions::IllegalStateException,
            "argument is no valid object"
        );
    }
    lua_getfield(lua, index, FIELDNAME_UID.c_str());
    if (!lua_isstring(lua, -1)) {
         SAMBAG_THROW(
            sambag::com::exceptions::IllegalStateException,
            "missing uuid"
        );
    }
    boost::tuple<std::string> uid;
    slua::pop(lua, uid);
    LuaModelObject::Ptr res = getByUId(boost::get<0>(uid));
    if (!res) {
        SAMBAG_THROW(
            sambag::com::exceptions::IllegalStateException,
            "object isn't registered"
        );
    }
    return res;
}
//-----------------------------------------------------------------------------
LuaModelObject::UIdMap LuaModelObject::uidMap;
//-----------------------------------------------------------------------------
LuaModelObject::Ptr LuaModelObject::getByUId(const UId &uid) {
    UIdMap::iterator it = uidMap.find(uid);
    if (it==uidMap.end()) {
        return LuaModelObject::Ptr();
    }
    return it->second.lock();
}
//-----------------------------------------------------------------------------
LuaModelObject::LuaModelObject() {
}
//-----------------------------------------------------------------------------
void LuaModelObject::setModelObject(ModelObject::Ptr obj) {
    if (!obj) {
        SAMBAG_THROW(
            sambag::com::exceptions::IllegalStateException,
            "set: model object == NULL"
        );
    }
    this->obj = obj;
}
//-----------------------------------------------------------------------------
LuaModelObject::ModelObject::Ptr LuaModelObject::getModelObject() const {
    ModelObject::Ptr res = obj.lock();
    if (!res) {
        SAMBAG_THROW(
            sambag::com::exceptions::IllegalStateException,
            "get: model object == NULL"
        );
    }
    return res;
}
//-----------------------------------------------------------------------------
void LuaModelObject::setTypeId(const std::string &typeId) {
    this->typeId = typeId;
}
//-----------------------------------------------------------------------------
void LuaModelObject::addLuaFields(lua_State *lua, int index) {
    Super::addLuaFields(lua, index);
    uidMap[getUId()] = boost::dynamic_pointer_cast<LuaModelObject>(shared_from_this());
    
    if (getTypeId().empty()) {
        SAMBAG_THROW(
            sambag::com::exceptions::IllegalStateException,
            "internal error: missing typeid"
        );
    }
    slua::push(lua, getTypeId());
    lua_setfield(lua, index, "__frxtype");
}
//-----------------------------------------------------------------------------
bool LuaModelObject::isequal(lua_State *lua) const {
    try {
        LuaModelObject::Ptr a = getFromLuaStack(lua, -1);
        LuaModelObject::Ptr b = getFromLuaStack(lua, -2);
        return a->getModelObject() == b->getModelObject();
    } catch(...) {
        return false;
    }
}
//-----------------------------------------------------------------------------
void LuaModelObject::__lua_gc(lua_State *lua) {
    uidMap.erase(getUId());
    Super::__lua_gc(lua);
}
}} // namespace(s)
