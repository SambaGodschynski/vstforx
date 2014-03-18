/*
 * LuaFrxObject.cpp
 *
 *  Created on: Fri Mar 14 11:47:35 2014
 *      Author: Johannes Unger
 */

#include "LuaFrxObject.hpp"
#include <sambag/com/exceptions/IllegalStateException.hpp>
#include <gui/components/FrxCircuidView.hpp>
#include <gui/IFrxControl.hpp>
#include <gui/HandyNamespaces.hpp>
#include <gui/components/FrxComponent.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <com/one4All.h>

namespace frx { namespace scripts {
//=============================================================================
//  Class LuaFrxObject
//=============================================================================
//-----------------------------------------------------------------------------
std::string LuaFrxObject::toString(lua_State *lua) const {
    fgc::FrxComponent::Ptr c = getViewObject(lua);
    if (!c) {
        return "";
    }
    return c->getName();
}
//-----------------------------------------------------------------------------
LuaFrxObject::Ptr LuaFrxObject::getFromLuaStack(lua_State *lua, int index) {
    if (!lua_istable(lua, index)) {
        SAMBAG_THROW(
            sambag::com::exceptions::IllegalStateException,
            "argument is no valid object"
        );
    }
    lua_getfield(lua, index, "__frxUID");
    if (!lua_isstring(lua, -1)) {
         SAMBAG_THROW(
            sambag::com::exceptions::IllegalStateException,
            "missing uuid"
        );
    }
    boost::tuple<std::string> uid;
    slua::get(lua, uid);
    lua_pop(lua, 1);
    LuaFrxObject::Ptr res = getByUId(boost::get<0>(uid));
    if (!res) {
        SAMBAG_THROW(
            sambag::com::exceptions::IllegalStateException,
            "object isn't registered in view"
        );
    }
    return res;
}
//-----------------------------------------------------------------------------
LuaFrxObject::UIdMap LuaFrxObject::uidMap;
//-----------------------------------------------------------------------------
LuaFrxObject::Ptr LuaFrxObject::getByUId(const UId &uid) {
    UIdMap::iterator it = uidMap.find(uid);
    if (it==uidMap.end()) {
        return LuaFrxObject::Ptr();
    }
    return it->second.lock();
}
//-----------------------------------------------------------------------------
LuaFrxObject::LuaFrxObject() {
}
//-----------------------------------------------------------------------------
void LuaFrxObject::setModelObject(ModelObject::Ptr obj) {
    if (!obj) {
        SAMBAG_THROW(
            sambag::com::exceptions::IllegalStateException,
            "set: model object == NULL"
        );
    }
    this->obj = obj;
}
//-----------------------------------------------------------------------------
void LuaFrxObject::setViewModelMap(ViewModelMap::Ptr map) {
    if (!map) {
        SAMBAG_THROW(
            sambag::com::exceptions::IllegalStateException,
            "set: model2view == NULL"
        );
    }
    this->modelMap = map;
}
//-----------------------------------------------------------------------------
LuaFrxObject::ModelObject::Ptr LuaFrxObject::getModelObject() const {
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
fgc::FrxComponent::Ptr LuaFrxObject::getViewObject() const {
    ModelObject::Ptr mObj = getModelObject();
    ViewModelMap::Ptr map = modelMap.lock();
    if (!map) {
        SAMBAG_THROW(
            sambag::com::exceptions::IllegalStateException,
            "get: model2view map == NULL"
        );
    }
    fgc::FrxComponent::Ptr res = boost::dynamic_pointer_cast<fgc::FrxComponent>
        (map->getViewObject(mObj));
    if (!res) {
        SAMBAG_THROW(
            sambag::com::exceptions::IllegalStateException,
            "get: view object == NULL"
        );
    }
    return res;
}
//-----------------------------------------------------------------------------
fgc::FrxComponent::Ptr LuaFrxObject::getViewObject(lua_State *lua) const {
    fgc::FrxComponent::Ptr c;
    try {
        c = getViewObject();
    } catch(const std::exception &ex) {
        slua::pushLuaError(lua, std::string("view object is not available: ") + ex.what());
        return fgc::FrxComponent::Ptr();
    } catch(...) {
        slua::pushLuaError(lua, "view object is not available");
        return fgc::FrxComponent::Ptr();
    }
    return c;
}
//-----------------------------------------------------------------------------
void LuaFrxObject::addLuaFields(lua_State *lua, int index) {
    Super::addLuaFields(lua, index);
    using namespace boost::uuids;
    uuid uuid = random_generator()();
    uid = boost::uuids::to_string(uuid);
    slua::push(lua, uid);
    lua_setfield(lua, index, "__frxUID");
    uidMap[uid] = boost::dynamic_pointer_cast<LuaFrxObject>(shared_from_this());
}
//-----------------------------------------------------------------------------
bool LuaFrxObject::isequal(lua_State *lua) const {
    try {
        LuaFrxObject::Ptr a = getFromLuaStack(lua, -1);
        LuaFrxObject::Ptr b = getFromLuaStack(lua, -2);
        return a->getModelObject() == b->getModelObject();
    } catch(...) {
        return false;
    }
}
//-----------------------------------------------------------------------------
void LuaFrxObject::__gc(lua_State *lua) {
    uidMap.erase(uid);
    Super::__gc(lua);
}
//=============================================================================
// LuaFrxObject::Factory
//=============================================================================
//-----------------------------------------------------------------------------
bool LuaFrxObject::Factory::registerCreator(const std::string &id, const Creator &f) {
    if (com::IdParser(id).namespace_() != "lua") {
        SAMBAG_LOG_WARN<<"tried to register: " << id << " as lua object creator";
    }
    return creatorMap.insert(
        CreatorMap::value_type(id, f)
    ).second;
}
//-----------------------------------------------------------------------------
LuaFrxObject::Ptr LuaFrxObject::Factory::createAndPush(const std::string &id,
        lua_State * lua, ModelObject::Ptr obj, ViewModelMap::Ptr map)
{
    CreatorMap::const_iterator it = creatorMap.find(id);
    if (it==creatorMap.end()) {
        return LuaFrxObject::Ptr();
    }
    return it->second(lua, obj, map);
}
//-----------------------------------------------------------------------------
bool LuaFrxObject::Factory::isRegistered(const std::string &id) {
    return creatorMap.find(id)!=creatorMap.end();
}
//-----------------------------------------------------------------------------
LuaFrxObject::Factory & LuaFrxObject::Factory::instance() {
    typedef Loki::SingletonHolder<Factory> FactoryHolder;
    return FactoryHolder::Instance();
}
///////////////////////////////////////////////////////////////////////////////
}} // namespace(s)
