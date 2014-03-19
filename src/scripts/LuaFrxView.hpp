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
    virtual void addLuaFields(lua_State *lua, int index);
    //-------------------------------------------------------------------------
    LuaFrxView();
    //-------------------------------------------------------------------------
    fgc::FrxCircuidViewPtr getView(lua_State *lua) const;
    //-------------------------------------------------------------------------
    typedef boost::tuple<float,float> Point;
    SAMBAG_LUA_FTAG(addProcessor, slua::IgnoreReturn(std::string));
    SAMBAG_LUA_FTAG(remove, void());
    SAMBAG_LUA_FTAG(getObjects, slua::IgnoreReturn());
    SAMBAG_LUA_FTAG(connect, slua::IgnoreReturn());
    SAMBAG_LUA_FTAG(addKnob, slua::IgnoreReturn());
    SAMBAG_LUA_FTAG(addHostKnob, slua::IgnoreReturn(int));
    SAMBAG_LUA_FTAG(getLocation, Point());
    SAMBAG_LUA_FTAG(setLocation, void(float, float));
    SAMBAG_LUA_FTAG(getSize, Point());
    SAMBAG_LUA_FTAG(setSize, void(float, float));
    typedef LOKI_TYPELIST_10(Frx_addProcessor_Tag,
        Frx_remove_Tag,
        Frx_getObjects_Tag,
        Frx_connect_Tag,
        Frx_addKnob_Tag,
        Frx_addHostKnob_Tag,
        Frx_getLocation_Tag,
        Frx_setLocation_Tag,
        Frx_getSize_Tag,
/*10*/  Frx_setSize_Tag
    ) Functions;
    ///////////////////////////////////////////////////////////////////////////
    // Lua impl.
    slua::IgnoreReturn addProcessor(lua_State *lua, const std::string &id);
    void remove(lua_State *lua);
    slua::IgnoreReturn getObjects(lua_State *lua);
    slua::IgnoreReturn connect(lua_State *lua);
    slua::IgnoreReturn addKnob(lua_State *lua);
    slua::IgnoreReturn addHostKnob(lua_State *lua, int index);
    boost::tuple<float,float> getLocation(lua_State *lua) const;
    void setLocation(lua_State *lua, float x, float y);
    boost::tuple<float,float> getSize(lua_State *lua) const;
    void setSize(lua_State *lua, float x, float y);
private:
    //-------------------------------------------------------------------------
    fgc::VstForxEditor *editor;
public:
    //-------------------------------------------------------------------------
    fgc::FrxCircuidViewPtr getView() const;
    //-------------------------------------------------------------------------
    static Ptr createAndPush(lua_State *lua,
        fgc::VstForxEditor *editor);
}; // LuaFrxView
}} // namespace(s)
#endif  // FORX_LuaFrxView_H


