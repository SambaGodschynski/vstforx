/*
 * ============================================================================
 * LuaFrxWindow.h
 *      Author: Johannes Unger
 * ============================================================================
 */
#ifndef FORX_LuaFrxWindow_H
#define FORX_LuaFrxWindow_H

#include <memory>
#include "LuaFrxWindowBase.hpp"
#include <sambag/disco/components/FramedWindow.hpp>
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
	typedef std::shared_ptr<LuaFrxWindow> Ptr;
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
    void open(lua_State *lua);
    void close(lua_State *lua);
    void addCloseListener(lua_State *lua, const std::string &expr);
    void setSize(lua_State *lua, int w, int h);
    void setLocation(lua_State *lua, int x, int y);
    Point getSize(lua_State *lua);
    Point getLocation(lua_State *lua);
    void setTitle(lua_State *lua, const std::string &x);
    std::string getTitle(lua_State *lua);
private:
    //-------------------------------------------------------------------------
    sdc::FramedWindow::Ptr window;
}; // LuaFrxWindow


}} // namespace(s)
#endif  // FORX_LuaFrxWindow_H


