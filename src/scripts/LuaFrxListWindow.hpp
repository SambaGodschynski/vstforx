/*
 * ============================================================================
 * LuaFrxListWindow.h
 *      Author: Johannes Unger
 * ============================================================================
 */
#ifndef FORX_LuaFrxListWindow_H
#define FORX_LuaFrxListWindow_H

#include <boost/shared_ptr.hpp>
#include "LuaFrxListWindowBase.hpp"
#include <sambag/disco/components/Window.hpp>
#include <gui/HandyNamespaces.hpp>
#include <sambag/disco/components/List.hpp>
#include <sambag/disco/components/ScrollPane.hpp>

namespace frx { namespace scripts {
//=============================================================================
/** 
  * @class LuaFrxListWindow.
  */
class LuaFrxListWindow : public LuaFrxListWindowBase {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<LuaFrxListWindow> Ptr;
    //-------------------------------------------------------------------------
    typedef LuaFrxListWindowBase Super;
protected:
    //-------------------------------------------------------------------------
    sdc::AContainer::Ptr createListPane();
    //-------------------------------------------------------------------------
    virtual void createWindow(sdc::Window::Ptr parent);
    //-------------------------------------------------------------------------
    virtual sdc::AContainer::Ptr createBtnPane();
    //-------------------------------------------------------------------------
    LuaFrxListWindow() {}
    //-------------------------------------------------------------------------
    ///////////////////////////////////////////////////////////////////////////
    // lua2frx impl
    //-------------------------------------------------------------------------
    void add(lua_State *lua, const std::string &x);
    //-------------------------------------------------------------------------
    int getNumElements(lua_State *lua);
    //-------------------------------------------------------------------------
    std::string getElementAt(lua_State *lua, int i);
    //-------------------------------------------------------------------------
    void remove(lua_State *lua, const std::string &x);
    //-------------------------------------------------------------------------
    std::string getSelection(lua_State *lua);
    //-------------------------------------------------------------------------
    void addButton(lua_State *lua,
        const std::string &txt, const std::string &xpr);
    //-------------------------------------------------------------------------
    void addSelectionListener(lua_State *lua, const std::string &expr);
private:
    //-------------------------------------------------------------------------
    sdc::ScrollPane::Ptr scrollPane;
    //-------------------------------------------------------------------------
    sdc::Panel::Ptr buttons;
    //-------------------------------------------------------------------------
    sdc::StringList::Ptr list;
    //-------------------------------------------------------------------------
    void onButton(lua_State *lua, const std::string &expr);
    //-------------------------------------------------------------------------
    void _add(const std::string &str);
public:
    //-------------------------------------------------------------------------
    static Ptr createAndPush(lua_State * lua, sdc::Window::Ptr parent);
}; // LuaFrxListWindow


}} // namespace(s)
#endif  // FORX_LuaFrxListWindow_H


