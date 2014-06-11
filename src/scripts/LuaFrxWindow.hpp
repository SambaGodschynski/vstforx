/*
 * ============================================================================
 * LuaFrxWindow.h
 *      Author: Johannes Unger
 * ============================================================================
 */
#ifndef FORX_LuaFrxWindow_H
#define FORX_LuaFrxWindow_H

#include <boost/shared_ptr.hpp>
#include "LuaFrxWindowBase.hpp"
#include <sambag/disco/components/Window.hpp>
#include <gui/HandyNamespaces.hpp>

namespace frx { namespace scripts {
//=============================================================================
/** 
  * @class LuaFrxWindow.
  */
class LuaFrxWindow : public LuaFrxWindowBase {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<LuaFrxWindow> Ptr;
    //-------------------------------------------------------------------------
    typedef LuaFrxWindowBase Super;
protected:
    //-------------------------------------------------------------------------
    sdc::AContainer::Ptr createListPane();
    //-------------------------------------------------------------------------
    virtual void createWindow(sdc::Window::Ptr parent);
    //-------------------------------------------------------------------------
    virtual sdc::Window::Ptr getWindow();
    //-------------------------------------------------------------------------
    LuaFrxWindow() {}
    //-------------------------------------------------------------------------
    void __lua_gc(lua_State *lua);
    //-------------------------------------------------------------------------
    ///////////////////////////////////////////////////////////////////////////
    // lua2frx impl
    void open(lua_State *lua, float w, float h);
    void close(lua_State *lua);
    void addCloseListener(lua_State *lua, const std::string &expr);
private:
    //-------------------------------------------------------------------------
    sdc::Window::Ptr window;
}; // LuaFrxWindow


}} // namespace(s)
#endif  // FORX_LuaFrxWindow_H


