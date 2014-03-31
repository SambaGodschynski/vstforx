/*
 * ============================================================================
 * LuaFrxParameter.h
 *      Author: Johannes Unger
 * ============================================================================
 */
#ifndef FORX_LuaFrxParameter_H
#define FORX_LuaFrxParameter_H

#include <boost/shared_ptr.hpp>
#include "LuaFrxObject.hpp"
#include <gui/ViewFactory.hpp>
#include <com/one4All.h>

namespace frx { namespace scripts {
//=============================================================================
/** 
  * @class LuaFrxParameter.
  */
class LuaFrxParameter : public LuaFrxObject {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<LuaFrxParameter> Ptr;
    //-------------------------------------------------------------------------
    typedef LuaFrxObject Super;
protected:
    //-------------------------------------------------------------------------
    virtual void addLuaFields(lua_State * lua, int index);
    //-------------------------------------------------------------------------
    LuaFrxParameter();
    //-------------------------------------------------------------------------
    SAMBAG_LUA_FTAG(setValue, void(float));
    SAMBAG_LUA_FTAG(getValue, float());
    typedef LOKI_TYPELIST_2(Frx_setValue_Tag,
        Frx_getValue_Tag
    ) Functions;
    ///////////////////////////////////////////////////////////////////////////
    // Lua impl.
    //-------------------------------------------------------------------------
    virtual std::string toString(lua_State * lua) const;
    void setValue(lua_State * lua, float v);
    float getValue(lua_State * lua) const;
    //-------------------------------------------------------------------------
    virtual void __lua_gc(lua_State *lua);
private:
public:
    //-------------------------------------------------------------------------
    static Ptr createAndPush(lua_State * lua,
        ModelObject::Ptr obj, ViewModelMap::Ptr map, const std::string &typeId);
}; // LuaFrxParameter

namespace {
    const bool LuaFrxParameter_Registered =
        LuaFrxObject::Factory::instance().
            registerCreator("frx.lua.parameter.StdKnob",
                boost::bind(&LuaFrxParameter::createAndPush, _1, _2, _3, "frx.lua.parameter.StdKnob")
            );

}


}} // namespace(s)
#endif  // FORX_LuaFrxParameter_H


