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
        ModelObject::Ptr obj, ViewModelMap::Ptr map);
}; // LuaFrxIO

namespace {
    inline bool registerAllIos() {
        LuaFrxObject::Factory &fac = LuaFrxObject::Factory::instance();
        return fac.registerCreator("frx.lua.io.Input", &LuaFrxIO::createAndPush)
        && fac.registerCreator("frx.lua.io.Output", &LuaFrxIO::createAndPush)
        && fac.registerCreator("frx.lua.io.Entry", &LuaFrxIO::createAndPush)
        && fac.registerCreator("frx.lua.io.Exit", &LuaFrxIO::createAndPush);
    }
    const bool LuaFrxIO_Registered = registerAllIos();

}


}} // namespace(s)
#endif  // FORX_LuaFrxIO_H


