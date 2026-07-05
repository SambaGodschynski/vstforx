/*
 * LuaFrxObject.cpp
 *
 *  Created on: Fri Mar 14 11:47:35 2014
 *      Author: Johannes Unger
 */

#include <tuple>
#include "LuaFrxObject.hpp"
#include <sambag/com/exceptions/IllegalStateException.hpp>
#include <gui/components/FrxCircuidView.hpp>
#include <gui/IFrxControl.hpp>
#include <gui/HandyNamespaces.hpp>
#include <gui/components/FrxComponent.hpp>
#include <com/one4All.h>
#include <gui/components/FrxMenuLabel.hpp>

namespace frx { namespace scripts {
//=============================================================================
//  Class LuaFrxObject
//=============================================================================
namespace {
    void __onMenu(lua_State *lua, const std::string &cmd,
        fgc::FrxCircuidView::WPtr _view)
    {
        fgc::FrxCircuidView::Ptr view = _view.lock();
        if (!view) {
            return;
        }
        SAMBAG_BEGIN_SYNCHRONIZED(sambag::lua::ALuaObject::getLock(lua))
            try {
                slua::executeString(lua, cmd);
            } catch(const sambag::lua::ExecutionFailed &ex) {
                view->errorMessage(cmd + " failed: " + ex.errMsg);
            } catch(...) {
                view->errorMessage(cmd + " failed for unkown reason");
            }
        SAMBAG_END_SYNCHRONIZED
    }
} // namespace
//-----------------------------------------------------------------------------
void LuaFrxObject::addMenuEntry(sambag::disco::components::PopupMenuPtr res,
    lua_State *lua, int index)
{
    using namespace sambag::disco::components;
    lua_pushnil(lua); /* first key */
    --index;
    std::string name, action;
    Menu::Ptr smenu;
    while (lua_next(lua, index) != 0) { // -1 == key index, -2 == value index
        std::string key, value;
        if (lua_isstring(lua, -1) == 1) { // value
            slua::get(value, lua, -1);
        }
        if (lua_istable(lua, -1)==1) { // is table
            // create sub menu
            smenu = Menu::create();
            lua_pushnil(lua); /* first key */
            while (lua_next(lua, -2) != 0) {
                if (lua_istable(lua, -1)==1) {
                    addMenuEntry(smenu->getPopupMenu(), lua, -1);
                }
                lua_pop(lua, 1);
            }
        }
        if (lua_type(lua, -2) != LUA_TSTRING) { // ignore non string keys
            lua_pop(lua, 1);
            continue;
        }
        slua::get(key, lua, -2);
        lua_pop(lua, 1);
        if (key=="name") {
            name = value;
        }
        if (key=="action") {
            action = value;
        }
    }
    if (name.empty()) {
        return;
    }
    if (smenu) {
        smenu->setText(name);
        res->add(smenu);
        return;
    }
    // add menu item
    if (action.length()==0) {
        fgc::FrxMenuLabel::Ptr label = fgc::FrxMenuLabel::create();
        label->setText(name);
        res->add(label);
        return;
    }
    MenuItem::Ptr item = MenuItem::create();
    fgc::FrxCircuidViewWPtr _view =
        getViewObject()->getFirstContainer<fgc::FrxCircuidView>();
    item->EventSender<sdc::events::ActionEvent>::addEventListener(
        boost::bind(&__onMenu, lua, action, _view)
    );
    item->setText(name);
    res->add(item);
}
//-----------------------------------------------------------------------------
std::string LuaFrxObject::getViewId(lua_State *lua) {
    try {
        return sambag::com::toString(getViewObject().get());
    } catch(const std::exception &ex) {
        slua::pushLuaError(lua, ex.what());
    } catch(...) {
        slua::pushLuaError(lua, "unknown error");
    }
    return "";
}
//-----------------------------------------------------------------------------
void LuaFrxObject::setMenu(lua_State *lua) {
    using namespace sambag::disco::components;
    try {
        using namespace sambag::disco::components;
        PopupMenuPtr res = PopupMenu::create();
        // iterate through menu table
        int index = -1;
        if (lua_istable(lua, index)!=1) {
            throw std::runtime_error("invalid value");
        }
        lua_pushnil(lua); /* first key */
        --index;
        while (lua_next(lua, index) != 0) {
            if (lua_istable(lua, -1)==1) {
                addMenuEntry(res, lua, -1);
            }
            lua_pop(lua, 1);
        }
        getViewObject()->setComponentPopupMenu(res);
    } catch(const std::exception &ex) {
        slua::pushLuaError(lua, ex.what());
    } catch(...) {
        slua::pushLuaError(lua, "unknown error");
    }
}

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
    lua_getfield(lua, index, FIELDNAME_UID.c_str());
    if (!lua_isstring(lua, -1)) {
         SAMBAG_THROW(
            sambag::com::exceptions::IllegalStateException,
            "missing uuid"
        );
    }
    std::tuple<std::string> uid;
    slua::get(lua, uid);
    lua_pop(lua, 1);
    LuaFrxObject::Ptr res = getByUId(std::get<0>(uid));
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
LuaFrxObject::ViewModelMap::Ptr LuaFrxObject::getViewModelMap() const {
    ViewModelMap::Ptr res = modelMap.lock();
    if (!res) {
        SAMBAG_THROW(
            sambag::com::exceptions::IllegalStateException,
            "set: modelMap == NULL"
        );
    }
    return res;
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
    fgc::FrxComponent::Ptr res = std::dynamic_pointer_cast<fgc::FrxComponent>
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
void LuaFrxObject::setName(lua_State *lua, const std::string &name) {
    try {
        getViewObject()->setName(name);
        getModelObject()->setName(name);
    } catch(const std::exception &ex) {
        slua::pushLuaError(lua, std::string("view object is not available: ") + ex.what());
    } catch(...) {
        slua::pushLuaError(lua, "view object is not available");
    }

}
//-----------------------------------------------------------------------------
std::string LuaFrxObject::getName(lua_State *lua) {
    fgc::FrxComponent::Ptr obj = getViewObject(lua);
    return obj->getName();
}
//-----------------------------------------------------------------------------
std::string LuaFrxObject::getTypeId(lua_State *lua) {
   com::IdParser id(getTypeId());
   return id.type() + "." + id.name();
}

//-----------------------------------------------------------------------------
void LuaFrxObject::addLuaFields(lua_State *lua, int index) {
    Super::addLuaFields(lua, index);
    uidMap[getUId()] = std::dynamic_pointer_cast<LuaFrxObject>(shared_from_this());

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
void LuaFrxObject::setTypeId(const std::string &typeId) {
    this->typeId = typeId;
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
void LuaFrxObject::__lua_gc(lua_State *lua) {
    uidMap.erase(getUId());
    Super::__lua_gc(lua);
}
//-----------------------------------------------------------------------------
std::tuple<float,float>  LuaFrxObject::getLocation(lua_State *lua) {
    fgc::FrxComponent::Ptr c = getViewObject(lua);
    if (!c) {
        return std::make_tuple(0.f, 0.f);
    }
    sd::Point2D p = c->getLocation();
    return std::make_tuple(p.x(), p.y());
}
//-----------------------------------------------------------------------------
void LuaFrxObject::setLocation(lua_State *lua, float x, float y) {
    fgc::FrxComponent::Ptr c = getViewObject(lua);
    if (!c) {
        return;
    }
    SAMBAG_BEGIN_SYNCHRONIZED(c->getTreeLock())
        c->setLocation(sd::Point2D(x,y));
    SAMBAG_END_SYNCHRONIZED
}
//-----------------------------------------------------------------------------
std::tuple<float,float>  LuaFrxObject::getSize(lua_State *lua) {
    fgc::FrxComponent::Ptr c = getViewObject(lua);
    if (!c) {
        return std::make_tuple(0.f, 0.f);
    }
    sd::Dimension p = c->getSize();
    return std::make_tuple(p.width(), p.height());
}
//-----------------------------------------------------------------------------
void LuaFrxObject::setSize(lua_State *lua, float w, float h) {
    fgc::FrxComponent::Ptr c = getViewObject(lua);
    if (!c) {
        return;
    }
    c->setSize(sd::Dimension(w,h));
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
