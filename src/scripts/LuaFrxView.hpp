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
#include "LuaFrxViewBase.hpp"

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
class LuaFrxView : public LuaFrxViewBase {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<LuaFrxView> Ptr;
    //-------------------------------------------------------------------------
    typedef LuaFrxViewBase Super;
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
    LuaFrxView();
    //-------------------------------------------------------------------------
    fgc::FrxCircuidViewPtr getView(lua_State *lua) const;
    //-------------------------------------------------------------------------
    ///////////////////////////////////////////////////////////////////////////
    // Lua impl.
    slua::IgnoreReturn add(lua_State *lua);
    slua::IgnoreReturn addObject(lua_State *lua);
    slua::IgnoreReturn addProcessor(lua_State *lua, const std::string &id);
    slua::IgnoreReturn addRelatedParameter(lua_State *lua);
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
    slua::IgnoreReturn getContextObject(lua_State *lua);
    void addMenuEntry(sdc::PopupMenuPtr res, lua_State *lua, int index);
    void onMenu(lua_State *lua, const std::string &cmd);
    void setMenu(lua_State *lua);
    void addViewListener(lua_State *lua, const std::string &listener);
    void removeViewListener(lua_State *lua, const std::string &listener);
    boost::tuple<float,float> getLocation(lua_State *lua);
    void setLocation(lua_State *lua, float x, float y);
    boost::tuple<float,float> getSize(lua_State *lua);
    void setSize(lua_State *lua, float x, float y);
    slua::IgnoreReturn createListWindow(lua_State *lua);
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


