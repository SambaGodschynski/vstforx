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
    SAMBAG_LUA_FTAG(addProcessor, slua::IgnoreReturn(std::string));
    SAMBAG_LUA_FTAG(remove, void());
    SAMBAG_LUA_FTAG(getObjects, slua::IgnoreReturn());
    typedef LOKI_TYPELIST_3(Frx_addProcessor_Tag,
        Frx_remove_Tag,
        Frx_getObjects_Tag
    ) Functions;
    ///////////////////////////////////////////////////////////////////////////
    // Lua impl.
    slua::IgnoreReturn addProcessor(lua_State *lua, const std::string &id);
    void remove(lua_State *lua);
    slua::IgnoreReturn getObjects(lua_State *lua);
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


