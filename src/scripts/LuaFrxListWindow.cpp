/*
 * ============================================================================
 * LuaFrxListWindow.cpp
 *      Author: Johannes Unger
 * ============================================================================
 */

#include "LuaFrxListWindow.hpp"
#include <sambag/com/Common.hpp>
#include <boost/foreach.hpp>
#include <exception>
#include <sambag/disco/components/WindowToolkit.hpp>
#include <sambag/disco/components/ui/UIManager.hpp>
#include <gui/components/ui/FrxLookAndFeel.hpp>
#include <sambag/disco/components/BorderLayout.hpp>
#include <sambag/disco/components/GridLayout.hpp>
#include <sambag/disco/components/BoxLayout.hpp>
#include <sambag/disco/components/TitledBorder.hpp>
#include <sambag/disco/components/SolidBorder.hpp>
#include <sambag/disco/components/FramedWindow.hpp>
#include <boost/algorithm/string.hpp>
#include <sambag/disco/components/Button.hpp>

namespace frx { namespace scripts {
//=============================================================================
//  Class LuaFrxListWindow
//=============================================================================
//-----------------------------------------------------------------------------
void LuaFrxListWindow::createWindow(sdc::Window::Ptr parent)
{
    using namespace sambag::com;
	using namespace sambag::disco::components;
    Super::createWindow(parent);
    
    sdc::Window::Ptr window = getWindow();
    sdc::AContainer::Ptr cnt = window->getContentPane();
    cnt->add(createListPane(), sdc::BorderLayout::CENTER, sdc::AContainer::APPEND);
    cnt->add(createBtnPane(), sdc::BorderLayout::SOUTH, sdc::AContainer::APPEND);
}
//-----------------------------------------------------------------------------
sdc::AContainer::Ptr LuaFrxListWindow::createBtnPane() {
	buttons = sdc::Panel::create();
    return buttons;
}
//-----------------------------------------------------------------------------
sdc::AContainer::Ptr LuaFrxListWindow::createListPane() {
	list = sdc::StringList::create();
	scrollPane = sdc::ScrollPane::create(list);
    scrollPane->setBorder(sdc::SolidBorder::create());
	return scrollPane;
}
//-----------------------------------------------------------------------------
LuaFrxListWindow::Ptr
LuaFrxListWindow::createAndPush(lua_State *lua, sdc::Window::Ptr parent)
{
    Ptr res(new LuaFrxListWindow());
    res->createLuaObject(lua, "list_window");
    res->createWindow(parent);
    return res;
}
//-----------------------------------------------------------------------------
void LuaFrxListWindow::_add(const std::string &x) {
    try {
        if (!list || !scrollPane) {
            throw std::runtime_error("list not accessable");
        }
        std::vector<std::string> res;
        boost::algorithm::split(res, x, boost::algorithm::is_any_of("\n"));
    
        BOOST_FOREACH(const std::string &x, res) {
            list->addElement(x);
        }
        scrollPane->revalidate();
        int i = list->ListModel::getSize()-1;
        list->ensureIndexIsVisible(i);
        list->revalidate();
        list->redraw();
    } catch(const std::exception &ex) {
    } catch(...) {
    }

}
//-----------------------------------------------------------------------------
void LuaFrxListWindow::add(lua_State *lua, const std::string &x) {
    sdc::getWindowToolkit()->invokeLater(
        boost::bind(&LuaFrxListWindow::_add, this, x),
        50,
        shared_from_this()
    ); // keep in sync with ui
}
//-----------------------------------------------------------------------------
int LuaFrxListWindow::getNumElements(lua_State *lua) {
    try {
        if (!list) {
            throw std::runtime_error("list not accessable");
        }
        return list->ListModel::getSize();
    } catch(const std::exception &ex) {
        sambag::lua::pushLuaError(lua, std::string("connecting failed: ") + ex.what());
    } catch(...) {
        sambag::lua::pushLuaError(lua, "connecting failed: unkown error");
    }
    return 0;
}
//-----------------------------------------------------------------------------
std::string LuaFrxListWindow::getElementAt(lua_State *lua, int i) {
    try {
        if (!list) {
            throw std::runtime_error("list not accessable");
        }
        if (i<1 || i>(int)list->ListModel::getSize()) {
            throw std::runtime_error("out of bounds");
        }
        return list->ListModel::get(i-1);
    } catch(const std::exception &ex) {
        sambag::lua::pushLuaError(lua, std::string("connecting failed: ") + ex.what());
    } catch(...) {
        sambag::lua::pushLuaError(lua, "connecting failed: unkown error");
    }
    return "";
}
//-----------------------------------------------------------------------------
void LuaFrxListWindow::remove(lua_State *lua, const std::string &x) {
    try {
        if (!list) {
            throw std::runtime_error("list not accessable");
        }
        list->ListModel::removeElement(x);
    } catch(const std::exception &ex) {
        sambag::lua::pushLuaError(lua, std::string("connecting failed: ") + ex.what());
    } catch(...) {
        sambag::lua::pushLuaError(lua, "connecting failed: unkown error");
    }

}
//-----------------------------------------------------------------------------
std::string LuaFrxListWindow::getSelection(lua_State *lua) {
    try {
        if (!list) {
            throw std::runtime_error("list not accessable");
        }
        const std::string *res=list->getSelectedValue();
        if (!res) {
            return "";
        }
        return *res;
    } catch(const std::exception &ex) {
        sambag::lua::pushLuaError(lua, std::string("connecting failed: ") + ex.what());
    } catch(...) {
        sambag::lua::pushLuaError(lua, "connecting failed: unkown error");
    }
    return "";
}
//-----------------------------------------------------------------------------
void LuaFrxListWindow::onButton(lua_State *lua, const std::string &expr) {
    SAMBAG_BEGIN_SYNCHRONIZED(getLock(lua))
        try {
            sambag::lua::executeString(lua, expr);
        } catch(const sambag::lua::ExecutionFailed &ex) {
            SAMBAG_LOG_ERR << expr << " failed: " << ex.errMsg;
        } catch(...) {
            SAMBAG_LOG_ERR << expr << " failed: unkown reason";
        }
    SAMBAG_END_SYNCHRONIZED
}
//-----------------------------------------------------------------------------
void LuaFrxListWindow::addButton(lua_State *lua,
        const std::string &txt, const std::string &xpr)
{
    try {
        if (!buttons) {
            throw std::runtime_error("buttons not accessable");
        }
        sdc::Button::Ptr btn = sdc::Button::create();
        btn->setText(txt);
        btn->sce::EventSender<sdc::events::ActionEvent>::addTrackedEventListener(
            boost::bind(&LuaFrxListWindow::onButton, this, lua, xpr),
            shared_from_this()
        );
        buttons->add(btn);
        getWindow()->getContentPane()->revalidate();
    } catch(const std::exception &ex) {
        sambag::lua::pushLuaError(lua, std::string("connecting failed: ") + ex.what());
    } catch(...) {
        sambag::lua::pushLuaError(lua, "connecting failed: unkown error");
    }

}
//------------------------------------------------------------------------------
void _onSelectionChanged(lua_State *lua, const std::string &expr) {
    SAMBAG_BEGIN_SYNCHRONIZED(sambag::lua::ALuaObject::getLock(lua))
        try {
            sambag::lua::executeString(lua, expr);
        } catch(const sambag::lua::ExecutionFailed &ex) {
            SAMBAG_LOG_ERR << expr << " failed: " << ex.errMsg;
        } catch(...) {
            SAMBAG_LOG_ERR << expr << " failed: unkown reason";
        }
    SAMBAG_END_SYNCHRONIZED
}
void LuaFrxListWindow::addSelectionListener(lua_State *lua, const std::string &expr)
{
    try {
        if (!list) {
            throw std::runtime_error("list not accessable");
        }
        list->sce::EventSender<sdc::events::ListSelectionEvent>::addTrackedEventListener(
            boost::bind(&_onSelectionChanged, lua, expr),
            shared_from_this()
        );
    } catch(const std::exception &ex) {
        sambag::lua::pushLuaError(lua, std::string("connecting failed: ") + ex.what());
    } catch(...) {
        sambag::lua::pushLuaError(lua, "connecting failed: unkown error");
    }
}
}} // namespace(s)
