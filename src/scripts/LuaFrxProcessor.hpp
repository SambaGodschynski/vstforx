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
#include "LuaFrxProcessorBase.hpp"
namespace frx { namespace scripts {
//=============================================================================
/** 
  * @class LuaFrxProcessor.
  */
class LuaFrxProcessor : public LuaFrxProcessorBase {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<LuaFrxProcessor> Ptr;
    //-------------------------------------------------------------------------
    typedef LuaFrxObject Super;
protected:
    //-------------------------------------------------------------------------
    LuaFrxProcessor();
    //-------------------------------------------------------------------------
    ///////////////////////////////////////////////////////////////////////////
    // lua2frx impl
    slua::IgnoreReturn getInputs(lua_State *lua);
    slua::IgnoreReturn getOutputs(lua_State *lua);
    slua::IgnoreReturn getParameters(lua_State *lua);
    slua::IgnoreReturn addInput(lua_State *lua, bool folow);
    slua::IgnoreReturn addOutput(lua_State *lua, bool follow);
    void openCloseEditor(lua_State *lua);
    int getNumInputs(lua_State *lua);
    int getNumOutputs(lua_State *lua);
    int getNumParameters(lua_State *lua);
    std::string getPluginLocation(lua_State *lua);
    std::string sendMessage(lua_State *lua, const std::string &msg);
	sambag::lua::IgnoreReturn serialize(lua_State *lua);
	void deserialize(lua_State *lua);
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


