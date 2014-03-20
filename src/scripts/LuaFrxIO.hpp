/*
 * ============================================================================
 * LuaFrxIO.h
 *      Author: Johannes Unger
 * ============================================================================
 */
#ifndef FORX_LuaFrxIO_H
#define FORX_LuaFrxIO_H

#include <boost/shared_ptr.hpp>
#include "LuaFrxObject.hpp"
#include <gui/ViewFactory.hpp>
#include <com/one4All.h>

namespace frx { namespace scripts {
//=============================================================================
/** 
  * @class LuaFrxIO.
  */
class LuaFrxIO : public LuaFrxObject {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<LuaFrxIO> Ptr;
    //-------------------------------------------------------------------------
    typedef LuaFrxObject Super;
protected:
    //-------------------------------------------------------------------------
    virtual void addLuaFields(lua_State * lua, int index);
    //-------------------------------------------------------------------------
    LuaFrxIO();
private:
public:
    //-------------------------------------------------------------------------
    static Ptr createAndPush(lua_State * lua,
        ModelObject::Ptr obj, ViewModelMap::Ptr map, const std::string &typeId);
}; // LuaFrxIO

namespace {
    inline bool registerAllIos() {
        LuaFrxObject::Factory &fac = LuaFrxObject::Factory::instance();
        return fac.registerCreator("frx.lua.io.Input",
            boost::bind(&LuaFrxIO::createAndPush, _1, _2, _3, "frx.lua.io.Input"))
        && fac.registerCreator("frx.lua.io.Output",
            boost::bind(&LuaFrxIO::createAndPush, _1, _2, _3, "frx.lua.io.Output"))
        && fac.registerCreator("frx.lua.io.Entry",
            boost::bind(&LuaFrxIO::createAndPush, _1, _2, _3, "frx.lua.io.Entry"))
        && fac.registerCreator("frx.lua.io.Exit",
            boost::bind(&LuaFrxIO::createAndPush, _1, _2, _3, "frx.lua.io.Exit"));
    }
    const bool LuaFrxIO_Registered = registerAllIos();

}


}} // namespace(s)
#endif  // FORX_LuaFrxIO_H


