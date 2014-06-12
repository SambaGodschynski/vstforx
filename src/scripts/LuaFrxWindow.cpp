/*
 * ============================================================================
 * LuaFrxWindow.cpp
 *      Author: Johannes Unger
 * ============================================================================
 */

#include "LuaFrxWindow.hpp"
#include <sambag/com/Common.hpp>
#include <boost/foreach.hpp>
#include <exception>
#include <sambag/disco/components/WindowToolkit.hpp>
#include <sambag/disco/components/ui/UIManager.hpp>
#include <gui/components/ui/FrxLookAndFeel.hpp>
#include <sambag/disco/components/BorderLayout.hpp>
#include <sambag/disco/components/GridLayout.hpp>
#include <sambag/disco/components/BoxLayout.hpp>
#include <sambag/disco/components/FramedWindow.hpp>

namespace frx { namespace scripts {
//=============================================================================
// Class LuaFrxWindow
//=============================================================================
//-----------------------------------------------------------------------------
void LuaFrxWindow::open(lua_State *lua) {
    try {
        if (!window) {
            throw std::runtime_error("Window not accessable");
        }
        sd::Point2D p = window->getWindowLocation();
        if (p.x()==0 || p.y()==0) {
            window->positionWindow();   
        }
        sd::Dimension size = window->getWindowSize();
        if (size.width() < 10. || size.height() < 10. ) {
            window->pack();
        }
        window->open();
    } catch(const std::exception &ex) {
        sambag::lua::pushLuaError(lua, std::string("failed: ") + ex.what());
    } catch(...) {
        sambag::lua::pushLuaError(lua, "failed: unkown error");
    }
}
//-----------------------------------------------------------------------------
void LuaFrxWindow::setSize(lua_State *lua, int w, int h) {
    try {
        if (!window) {
            throw std::runtime_error("Window not accessable");
        }
        window->setWindowSize(sd::Dimension(w,h));
    } catch(const std::exception &ex) {
        sambag::lua::pushLuaError(lua, std::string("failed: ") + ex.what());
    } catch(...) {
        sambag::lua::pushLuaError(lua, "failed: unkown error");
    }
}
//-----------------------------------------------------------------------------
void LuaFrxWindow::setLocation(lua_State *lua, int x, int y) {
    try {
        if (!window) {
            throw std::runtime_error("Window not accessable");
        }
        window->setWindowLocation(sd::Point2D(x,y));
    } catch(const std::exception &ex) {
        sambag::lua::pushLuaError(lua, std::string("failed: ") + ex.what());
    } catch(...) {
        sambag::lua::pushLuaError(lua, "failed: unkown error");
    }
}
//-----------------------------------------------------------------------------
LuaFrxWindow::Point LuaFrxWindow::getSize(lua_State *lua) {
    try {
        if (!window) {
            throw std::runtime_error("Window not accessable");
        }
        sd::Dimension d = window->getWindowSize();
        return Point(d.width(), d.height());
    } catch(const std::exception &ex) {
        sambag::lua::pushLuaError(lua, std::string("failed: ") + ex.what());
    } catch(...) {
        sambag::lua::pushLuaError(lua, "failed: unkown error");
    }
    return Point(0,0);
}
//-----------------------------------------------------------------------------
LuaFrxWindow::Point LuaFrxWindow::getLocation(lua_State *lua) {
    try {
        if (!window) {
            throw std::runtime_error("Window not accessable");
        }
        sd::Point2D p = window->getWindowLocation();
        return Point(p.x(), p.y());
    } catch(const std::exception &ex) {
        sambag::lua::pushLuaError(lua, std::string("failed: ") + ex.what());
    } catch(...) {
        sambag::lua::pushLuaError(lua, "failed: unkown error");
    }
    return Point(0,0);
}
//-----------------------------------------------------------------------------
std::string LuaFrxWindow::getTitle(lua_State *lua) {
    try {
        if (!window) {
            throw std::runtime_error("Window not accessable");
        }
        return window->getTitle();
    } catch(const std::exception &ex) {
        sambag::lua::pushLuaError(lua, std::string("failed: ") + ex.what());
    } catch(...) {
        sambag::lua::pushLuaError(lua, "failed: unkown error");
    }
    return "";
}
//-----------------------------------------------------------------------------
void LuaFrxWindow::setTitle(lua_State *lua, const std::string &x) {
    try {
        if (!window) {
            throw std::runtime_error("Window not accessable");
        }
        return window->setTitle(x);
    } catch(const std::exception &ex) {
        sambag::lua::pushLuaError(lua, std::string("failed: ") + ex.what());
    } catch(...) {
        sambag::lua::pushLuaError(lua, "failed: unkown error");
    }
}
//-----------------------------------------------------------------------------
void LuaFrxWindow::createWindow(sdc::Window::Ptr parent)
{
    using namespace sambag::com;
	using namespace sambag::disco::components;
    window = sdc::FramedWindow::create(parent);
	sdc::ui::UIManager::instance().installLookAndFeel(window->getRootPane(),
		frx::gui::components::ui::FrxLookAndFeel::create()
	);
}
//-----------------------------------------------------------------------------
sdc::Window::Ptr LuaFrxWindow::getWindow() {
    if (!window) {
        throw std::runtime_error("Window not accessable");
    }
    return window;
}
//-----------------------------------------------------------------------------
void LuaFrxWindow::close(lua_State *lua) {
    try {
        if (!window) {
            throw std::runtime_error("Window not accessable");
        }
        window->close();
    } catch(const std::exception &ex) {
        sambag::lua::pushLuaError(lua, std::string("connecting failed: ") + ex.what());
    } catch(...) {
        sambag::lua::pushLuaError(lua, "connecting failed: unkown error");
    }
}
//-----------------------------------------------------------------------------
void onClose(lua_State *lua, const std::string &expr) {
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
void LuaFrxWindow::addCloseListener(lua_State *lua, const std::string &expr) {
    try {
        if (!window) {
            throw std::runtime_error("Window not accessable");
        }
        window->addTrackedOnCloseEventListener(
            boost::bind(&onClose, lua, expr),
            shared_from_this()
        );
    } catch(const std::exception &ex) {
        sambag::lua::pushLuaError(lua, std::string("connecting failed: ") + ex.what());
    } catch(...) {
        sambag::lua::pushLuaError(lua, "connecting failed: unkown error");
    }
}
//-----------------------------------------------------------------------------
void LuaFrxWindow::__lua_gc(lua_State *lua) {
    window->close();
    window.reset();
    Super::__lua_gc(lua);
}
}} // namespace(s)
