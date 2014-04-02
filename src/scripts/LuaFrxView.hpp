/*
 * ===========================================================================================================
 * LuaFrxView.h
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#ifndef FORX_LuaFrxView_H
#define FORX_LuaFrxView_H

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <sambag/lua/ALuaObject.hpp>
#include "LuaFrxObject.hpp"
#include <gui/HandyNamespaces.hpp>
#include <sambag/disco/components/Forward.hpp>
#include <gui/components/FrxCircuidView.hpp>
#include <boost/unordered_set.hpp>
namespace frx {
namespace gui { namespace components {
    class FrxCircuidView;
    typedef boost::shared_ptr<FrxCircuidView> FrxCircuidViewPtr;
    typedef boost::weak_ptr<FrxCircuidView> FrxCircuidViewWPtr;
    class VstForxEditor;
}}

namespace scripts {
//=============================================================================
/** 
  * @class LuaFrxView.
  */
class LuaFrxView : public sambag::lua::ALuaObject {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<LuaFrxView> Ptr;
    //-------------------------------------------------------------------------
    typedef sambag::lua::ALuaObject Super;
protected:
    //-------------------------------------------------------------------------
    void initListenersIfNeccessary(lua_State *lua);
    //-------------------------------------------------------------------------
    void onViewEvent(lua_State *lua, const fgc::FrxCircuidViewEvent &ev);
    //-------------------------------------------------------------------------
    /**
     * @brief push components representations into lua stack 
     */
    template <class Container>
    void pushComponents(lua_State *lua, const Container &components);
    //-------------------------------------------------------------------------
    virtual void addLuaFields(lua_State *lua, int index);
    //-------------------------------------------------------------------------
    LuaFrxView();
    //-------------------------------------------------------------------------
    fgc::FrxCircuidViewPtr getView(lua_State *lua) const;
    //-------------------------------------------------------------------------
    typedef boost::tuple<float,float> Point;
    SAMBAG_LUA_FTAG(add, slua::IgnoreReturn());
    SAMBAG_LUA_FTAG(remove, void());
    SAMBAG_LUA_FTAG(getObjects, slua::IgnoreReturn());
    SAMBAG_LUA_FTAG(connect, slua::IgnoreReturn());
    SAMBAG_LUA_FTAG(addKnob, slua::IgnoreReturn());
    SAMBAG_LUA_FTAG(addHostKnob, slua::IgnoreReturn(int));
    SAMBAG_LUA_FTAG(getLocation, Point());
    SAMBAG_LUA_FTAG(setLocation, void(float, float));
    SAMBAG_LUA_FTAG(getSize, Point());
    SAMBAG_LUA_FTAG(setSize, void(float, float));
    SAMBAG_LUA_FTAG(getEntry, slua::IgnoreReturn());
    SAMBAG_LUA_FTAG(getExit, slua::IgnoreReturn());
    SAMBAG_LUA_FTAG(getByName, slua::IgnoreReturn(std::string));
    SAMBAG_LUA_FTAG(getByType, slua::IgnoreReturn(std::string));
    SAMBAG_LUA_FTAG(getSelectedObjects, slua::IgnoreReturn());
    SAMBAG_LUA_FTAG(addViewListener, void(std::string));
    SAMBAG_LUA_FTAG(removeViewListener, void(std::string));
    typedef LOKI_TYPELIST_10(Frx_add_Tag,
        Frx_remove_Tag,
        Frx_getObjects_Tag,
        Frx_connect_Tag,
        Frx_addKnob_Tag,
        Frx_addHostKnob_Tag,
        Frx_getLocation_Tag,
        Frx_setLocation_Tag,
        Frx_getSize_Tag,
        Frx_setSize_Tag) Functions1;
    
    typedef LOKI_TYPELIST_7(Frx_getEntry_Tag,
        Frx_getExit_Tag,
        Frx_getByName_Tag,
        Frx_getByType_Tag,
        Frx_getSelectedObjects_Tag,
        Frx_addViewListener_Tag,
        Frx_removeViewListener_Tag
    ) Functions2;
    ///////////////////////////////////////////////////////////////////////////
    // Lua impl.
    slua::IgnoreReturn add(lua_State *lua);
    slua::IgnoreReturn addObject(lua_State *lua);
    slua::IgnoreReturn addProcessor(lua_State *lua, const std::string &id);
    slua::IgnoreReturn addProcessorParameter(lua_State *lua);
    void remove(lua_State *lua);
    slua::IgnoreReturn getObjects(lua_State *lua);
    slua::IgnoreReturn getSelectedObjects(lua_State *lua);
    slua::IgnoreReturn connect(lua_State *lua);
    slua::IgnoreReturn addKnob(lua_State *lua);
    slua::IgnoreReturn addHostKnob(lua_State *lua, int index);
    slua::IgnoreReturn getEntry(lua_State *lua);
    slua::IgnoreReturn getExit(lua_State *lua);
    slua::IgnoreReturn getByName(lua_State *lua, const std::string &name);
    slua::IgnoreReturn getByType(lua_State *lua, const std::string &type);
    void addViewListener(lua_State *lua, const std::string &listener);
    void removeViewListener(lua_State *lua, const std::string &listener);
    boost::tuple<float,float> getLocation(lua_State *lua) const;
    void setLocation(lua_State *lua, float x, float y);
    boost::tuple<float,float> getSize(lua_State *lua) const;
    void setSize(lua_State *lua, float x, float y);
    //-------------------------------------------------------------------------
    virtual void __lua_gc(lua_State *lua);
private:
    //-------------------------------------------------------------------------
    fgc::VstForxEditor *editor;
    //-------------------------------------------------------------------------
    boost::unordered_set<std::string> luaViewListener;
    //-------------------------------------------------------------------------
    fgc::FrxCircuidView::WPtr listenerInstalled;
public:
    //-------------------------------------------------------------------------
    fgc::FrxCircuidViewPtr getView() const;
    //-------------------------------------------------------------------------
    static Ptr createAndPush(lua_State *lua,
        fgc::VstForxEditor *editor);
}; // LuaFrxView
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
template <class Container>
void LuaFrxView::pushComponents(lua_State *lua, const Container &components) {
    using namespace frx::gui;
	using namespace frx::gui::components;
    using namespace sambag::disco::components;
    
    IViewModelMap::Ptr map = getViewModelMap(getView());
    
    lua_createtable(lua, components.size(), 0);
    int top = lua_gettop(lua);
    
    int lua_index = 1;
    BOOST_FOREACH(const typename Container::value_type &_c, components) {
        AComponentPtr c = _c;
        FrxComponent::Ptr vObj = boost::dynamic_pointer_cast<FrxComponent>(c);
        if (!vObj) {
            continue;
        }
        processing::ModelObject::Ptr mObj = map->getModelObject(vObj);
        std::string type = vObj->getTypeId();
        
        if (type.empty()) {
            continue;
        }
        std::string id = com::IdParser(type).namespace_("lua").toString();
        LuaFrxObject::Factory &fac = LuaFrxObject::Factory::instance();
        if (!fac.isRegistered(id)) {
            continue;
        }
        lua_pushnumber(lua, lua_index++);
        LuaFrxObject::Ptr lobj = fac.createAndPush(
            id,
            lua,
            mObj,
            map
        );
        if (!lobj) {
            lua_pushnil(lua);
        }
        lua_settable(lua, top);
    }
}


}} // namespace(s)
#endif  // FORX_LuaFrxView_H


