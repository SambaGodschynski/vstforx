/*
 * ============================================================================
 * LuaFrxParameter.h
 *      Author: Johannes Unger
 * ============================================================================
 */
#ifndef FORX_LuaFrxParameter_H
#define FORX_LuaFrxParameter_H

#include <memory>
#include "LuaFrxObject.hpp"
#include <gui/ViewFactory.hpp>
#include <com/one4All.h>
#include "LuaFrxParameterBase.hpp"

namespace frx { namespace scripts {
//=============================================================================
/** 
  * @class LuaFrxParameter.
  */
class LuaFrxParameter : public LuaFrxParameterBase {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef std::shared_ptr<LuaFrxParameter> Ptr;
    //-------------------------------------------------------------------------
    typedef LuaFrxObject Super;
protected:
    //-------------------------------------------------------------------------
    LuaFrxParameter();
    //-------------------------------------------------------------------------
    ///////////////////////////////////////////////////////////////////////////
    // Lua impl.
    //-------------------------------------------------------------------------
    virtual std::string toString(lua_State * lua) const;
    void setValue(lua_State * lua, float v);
    float getValue(lua_State * lua);
    float getValue() const;
    void addListener(lua_State * lua, const std::string &callbk);
    //-------------------------------------------------------------------------
    virtual std::string getName(lua_State *lua);
private:
    //-------------------------------------------------------------------------
    void onParameterChanged(lua_State * lua, const std::string &callbk);
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


