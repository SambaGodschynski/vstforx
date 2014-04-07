/*
 * ============================================================================
 * LuaFrxProcessor.h
 *      Author: Johannes Unger
 * ============================================================================
 */
#ifndef FORX_LuaFrxProcessor_H
#define FORX_LuaFrxProcessor_H

#include <boost/shared_ptr.hpp>
#include "LuaFrxObject.hpp"
#include <gui/ViewFactory.hpp>
#include <com/one4All.h>
#include <gui/IFrxControl.hpp>

namespace frx { namespace scripts {
//=============================================================================
/** 
  * @class LuaFrxProcessor.
  */
class LuaFrxProcessor : public LuaFrxObject {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<LuaFrxProcessor> Ptr;
    //-------------------------------------------------------------------------
    typedef LuaFrxObject Super;
protected:
    //-------------------------------------------------------------------------
    virtual void addLuaFields(lua_State * lua, int index);
    //-------------------------------------------------------------------------
    LuaFrxProcessor();
    //-------------------------------------------------------------------------
    SAMBAG_LUA_FTAG(getInputs, slua::IgnoreReturn());
    SAMBAG_LUA_FTAG(getOutputs, slua::IgnoreReturn());
    SAMBAG_LUA_FTAG(getParameters, slua::IgnoreReturn());
    SAMBAG_LUA_FTAG(addInput, slua::IgnoreReturn());
    SAMBAG_LUA_FTAG(addOutput, slua::IgnoreReturn());
    SAMBAG_LUA_FTAG(openCloseEditor, void());
    typedef LOKI_TYPELIST_6(Frx_getInputs_Tag,
        Frx_getOutputs_Tag,
        Frx_getParameters_Tag,
        Frx_addInput_Tag,
        Frx_addOutput_Tag,
        Frx_openCloseEditor_Tag
    ) Functions;
    ///////////////////////////////////////////////////////////////////////////
    // lua2frx impl
    slua::IgnoreReturn getInputs(lua_State *lua) const;
    slua::IgnoreReturn getOutputs(lua_State *lua) const;
    slua::IgnoreReturn getParameters(lua_State *lua) const;
    slua::IgnoreReturn addInput(lua_State *lua);
    slua::IgnoreReturn addOutput(lua_State *lua);
    void openCloseEditor(lua_State *lua);
    //-------------------------------------------------------------------------
    virtual void __lua_gc(lua_State *lua);
private:
public:
    //-------------------------------------------------------------------------
    static Ptr createAndPush(lua_State * lua,
        ModelObject::Ptr obj, ViewModelMap::Ptr map, const std::string &typeId);
}; // LuaFrxProcessor

namespace {
    /**
     * @brief adopts registered VieFactory ids
     */
    inline bool registerAllProcessors() {
        std::vector<std::string> ids;
        using frx::gui::components::ViewFactory;
        ViewFactory::instance().getRegisteredIds(ids);
        bool res = true;
        std::string ns="frx.gui.";
        LuaFrxObject::Factory &fac = LuaFrxObject::Factory::instance();
        BOOST_FOREACH(const std::string &id, ids) {
            std::string new_id = com::IdParser(ns+id).namespace_("lua").toString();
            res &= fac.registerCreator(
                new_id,
                boost::bind(&LuaFrxProcessor::createAndPush, _1, _2, _3, new_id)
            );
        }
        return res;
    }
    const bool LuaFrxProcessor_Registered = registerAllProcessors();
}


}} // namespace(s)
#endif  // FORX_LuaFrxProcessor_H


