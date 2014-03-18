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
private:
public:
    //-------------------------------------------------------------------------
    static Ptr createAndPush(lua_State * lua,
        ModelObject::Ptr obj, ViewModelMap::Ptr map);
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
        BOOST_FOREACH(const std::string &id, ids) {
            res &= LuaFrxObject::registerCreator(
                com::IdParser(ns+id).namespace_("lua").toString(),
                &LuaFrxProcessor::createAndPush
            );
        }
        return res;
    }
    const bool registered = registerAllProcessors();
}


}} // namespace(s)
#endif  // FORX_LuaFrxProcessor_H


