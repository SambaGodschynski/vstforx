/*
 * PluginScriptCtrl.cpp
 *
 *  Created on: Mon Dec 17 10:01:25 2012
 *      Author: Johannes Unger
 */

#include "PluginScriptCtrl.hpp"
#include <sambag/com/exceptions/IllegalStateException.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/tuple/tuple.hpp>
#include <sambag/com/Common.hpp>
#include <boost/foreach.hpp>
#include <sambag/disco/components/WindowToolkit.hpp>
#include <loki/Typelist.h>
#include <gui/components/IFrxComponentFactory.hpp>
#include <gui/components/FrxCircuidView.hpp>
#include <gui/components/FrxComponent.hpp>
#include <gui/components/FrxNode.hpp>
#include <gui/components/FrxProcessorNode.hpp>
#include <gui/components/FrxParameter.hpp>
#include <gui/IFrxControl.hpp>
#include <sambag/lua/LuaSequence.hpp>
#include <gui/IViewModelMap.hpp>
#include <gui/components/FrxConcreteIO.hpp>
#include <gui/HandyNamespaces.hpp>
#include <processing/VstForxPlug.hpp>
#include <OS_Specific/OS_com.h>
#include <sambag/disco/components/Viewport.hpp>
#include <com/FrxConfig.h>
#include <sambag/com/Config.h>
#include <sambag/disco/components/Window.hpp>
#include "LuaParameter.hpp"
#include "LuaFrxView.hpp"
#include <com/PluginCollection.h>
#include <sambag/com/Common.hpp>
#include "LuaTimer.hpp"
#include <boost/regex.hpp>

namespace frx {

namespace gui {
    extern std::string __lastBrowserSelection;
}

namespace scripts {
    namespace slua = sambag::lua;
namespace {
	/**
	 * open/close sequences needs to wait before call the next. 
	 */
	const int FRX_OPENCLOSE_WORKAROUND_WAIT=700;
	///////////////////////////////////////////////////////////////////////////
	// FrxFunction impl.
	#define FRX_START_SCRIPTCALL ctrl->__startScriptCall(std::string(name())); 
    
    /*\
    boost::this_thread::sleep(boost::posix_time::milliseconds(FRX_OPENCLOSE_WORKAROUND_WAIT));*/
    
	#define FRX_GET_PLUG frx::processing::VstForxPlug * plug = ctrl->getPlugin();
	#define FRX_GET_EDITOR frx::gui::components::VstForxEditor * editor = ctrl->getEditor();
	///////////////////////////////////////////////////////////////////////////
	// Access
	typedef PluginScriptCtrl Ctrl;
	typedef Ctrl::LuaPtr LuaPtr;
	typedef bool Bool; // #244 workaround
	const int True = true;
	const int False = false;
	const LuaPtr NULL_LUAPTR = "";
	std::list<std::string> registeredFs;
	//-------------------------------------------------------------------------
	struct FrxOpenPlugin {
		typedef boost::function<void()> Function;
		static const char * name() { return "openPlugin"; }
		static void process(Ctrl *ctrl, const Ctrl::LuaProcessor &lp);
	};
	//-------------------------------------------------------------------------
	struct FrxClosePlugin {
		typedef boost::function<void()> Function;
		static const char * name() { return "closePlugin"; }
		static void process(Ctrl *ctrl, const Ctrl::LuaProcessor &lp);
	};
	//-------------------------------------------------------------------------
	struct FrxOpenEditor {
		typedef boost::function<void()> Function;
		static const char * name() { return "openEditor"; }
		static void process(Ctrl *ctrl, const Ctrl::LuaProcessor &lp);
	};
	//-------------------------------------------------------------------------
	struct FrxCloseEditor {
		typedef boost::function<void()> Function;
		static const char * name() { return "closeEditor"; }
		static void process(Ctrl *ctrl, const Ctrl::LuaProcessor &lp);
	};
	//-------------------------------------------------------------------------
	struct FrxWait {
		typedef boost::function<void(int)> Function;
		static const char * name() { return "wait"; }
		static void process(int sec, Ctrl *ctrl, const Ctrl::LuaProcessor &lp);
	};
	//-------------------------------------------------------------------------
	struct FrxSerializePlugin {
		typedef boost::function<std::string()> Function;
		static const char * name() { return "serializePlugin"; }
		static std::string process(Ctrl *ctrl, const Ctrl::LuaProcessor &lp);
	};
	//-------------------------------------------------------------------------
	struct FrxDeserializePlugin {
		typedef boost::function<void(std::string)> Function;
		static const char * name() { return "deserializePlugin"; }
		static void process(const std::string &, Ctrl *ctrl, const Ctrl::LuaProcessor &lp);
	};
	//-------------------------------------------------------------------------
	struct FrxIsEditorOpen {
		typedef boost::function<Bool()> Function;
		static const char * name() { return "isEditorOpen"; }
		static Bool process(Ctrl *ctrl, const Ctrl::LuaProcessor &lp);
	};
	//-------------------------------------------------------------------------
	struct FrxVerbose {
		typedef boost::function<void(Bool val)> Function;
		static const char * name() { return "verbose"; }
		static void process(Bool val, Ctrl *ctrl, const Ctrl::LuaProcessor &lp);
	};
	//-------------------------------------------------------------------------
	struct FrxGetGraphDelay {
        typedef boost::function<int()> Function;
		static const char * name() { return "getGraphDelay"; }
		static int process(Ctrl *ctrl, const Ctrl::LuaProcessor &lp);
	};
	//-------------------------------------------------------------------------
	struct FrxMessageBox {
        typedef boost::function<void(std::string)> Function;
		static const char * name() { return "messageBox"; }
		static void process(const std::string&, Ctrl *ctrl, const Ctrl::LuaProcessor &lp);
	};
	//-------------------------------------------------------------------------
	struct FrxSelectFile {
        typedef boost::function<std::string(std::string)> Function;
		static const char * name() { return "showSelectFileDlg"; }
		static std::string process(const std::string&, Ctrl *ctrl, const Ctrl::LuaProcessor &lp);
	};
	//-------------------------------------------------------------------------
	struct FrxSelectDirectory {
        typedef boost::function<std::string(std::string)> Function;
		static const char * name() { return "showSelectDirectory"; }
		static std::string process(const std::string&, Ctrl *ctrl, const Ctrl::LuaProcessor &lp);
	};
	//-------------------------------------------------------------------------
	struct FrxSaveFile {
        typedef boost::function<std::string(std::string)> Function;
		static const char * name() { return "showSaveFileDlg"; }
		static std::string process(const std::string&, Ctrl *ctrl, const Ctrl::LuaProcessor &lp);
	};
	//-------------------------------------------------------------------------
	struct FrxOpenSceneBrowser {
        typedef boost::function<void(std::string)> Function;
		static const char * name() { return "openSceneBrowser"; }
		static void process(const std::string&, Ctrl *ctrl, const Ctrl::LuaProcessor &lp);
	};
	//-------------------------------------------------------------------------
	struct FrxOpenUrl {
        typedef boost::function<void(std::string)> Function;
		static const char * name() { return "openUrl"; }
		static void process(const std::string&, Ctrl *ctrl, const Ctrl::LuaProcessor &lp);
	};
	//-------------------------------------------------------------------------
	struct FrxOpenSetup {
        typedef boost::function<void()> Function;
		static const char * name() { return "openSetup"; }
		static void process(Ctrl *ctrl, const Ctrl::LuaProcessor &lp);
	};
	//-------------------------------------------------------------------------
	struct FrxOpenAbout {
        typedef boost::function<void()> Function;
		static const char * name() { return "openAbout"; }
		static void process(Ctrl *ctrl, const Ctrl::LuaProcessor &lp);
	};
	//-------------------------------------------------------------------------
	struct FrxGetLastBrowserSelection {
        typedef boost::function<std::string()> Function;
		static const char * name() { return "getLastSceneBrowserSelection"; }
		static std::string process(Ctrl *ctrl, const Ctrl::LuaProcessor &lp);
	};
	//-------------------------------------------------------------------------
	struct FrxGetVersionInteger {
        typedef boost::function<int()> Function;
		static const char * name() { return "getVersionInteger"; }
		static int process(Ctrl *ctrl, const Ctrl::LuaProcessor &lp);
	};
	//-------------------------------------------------------------------------
	struct FrxGetVersionString {
        typedef boost::function<std::string()> Function;
		static const char * name() { return "getVersionString"; }
		static std::string process(Ctrl *ctrl, const Ctrl::LuaProcessor &lp);
	};
	//-------------------------------------------------------------------------
	struct FrxSetEditorExitOnClose {
		typedef boost::function<void(Bool)> Function;
		static const char * name() { return "setEditorExitOnClose"; }
		static void process(Bool val, Ctrl *ctrl, const Ctrl::LuaProcessor &lp);
	};
	//-------------------------------------------------------------------------
	struct FrxQueryDB {
		typedef boost::function<slua::IgnoreReturn(std::string)> Function;
		static const char * name() { return "queryDB"; }
		static slua::IgnoreReturn process(const std::string &query, Ctrl *ctrl, const Ctrl::LuaProcessor &lp);
	};
	//-------------------------------------------------------------------------
	struct FrxAddTimer {
		typedef boost::function<slua::IgnoreReturn(std::string, int, int)> Function;
		static const char * name() { return "addTimer"; }
		static slua::IgnoreReturn process(const std::string &, int, int, Ctrl *ctrl, const Ctrl::LuaProcessor &lp);
	};
	//-------------------------------------------------------------------------
	struct FrxSetPersistData {
		typedef boost::function<void()> Function;
		static const char * name() { return "setPersistData"; }
		static void process(Ctrl *ctrl, const Ctrl::LuaProcessor &lp);
	};
	//-------------------------------------------------------------------------
	struct FrxGetPersistData {
		typedef boost::function<slua::IgnoreReturn(std::string)> Function;
		static const char * name() { return "getPersistData"; }
		static slua::IgnoreReturn process(const std::string &, Ctrl *ctrl, const Ctrl::LuaProcessor &lp);
	};
    //-------------------------------------------------------------------------
	struct FrxShowInputTextDlg {
		typedef boost::function<std::string(std::string, std::string)> Function;
		static const char * name() { return "showInputTextDlg"; }
		static std::string process(const std::string &, const std::string &, Ctrl *ctrl, const Ctrl::LuaProcessor &lp);
	};
    //-------------------------------------------------------------------------
	struct FrxShowYesNoDlg {
		typedef boost::function<bool(std::string)> Function;
		static const char * name() { return "showYesNoDlg"; }
		static bool process(const std::string &, Ctrl *ctrl, const Ctrl::LuaProcessor &lp);
	};
    //-------------------------------------------------------------------------
	struct FrxRunOnUIThread {
		typedef boost::function<void(std::string)> Function;
		static const char * name() { return "runOnUIThread"; }
		static void process(const std::string &, Ctrl *ctrl, const Ctrl::LuaProcessor &lp);
	};
    //-------------------------------------------------------------------------
	struct FrxExec {
		typedef boost::function<void(std::string)> Function;
		static const char * name() { return "exec"; }
		static void process(const std::string &, Ctrl *ctrl, const Ctrl::LuaProcessor &lp);
	};
    //-------------------------------------------------------------------------
	struct FrxGetBuildHash {
		typedef boost::function<std::string()> Function;
		static const char * name() { return "getBuildHash"; }
		static std::string process(Ctrl *ctrl, const Ctrl::LuaProcessor &lp) {
            std::stringstream ss;
            ss<<FRX_GITHASH<<":"<<SAMBAG_VERSION_BUILD;
            return ss.str();
        }
	};
	//-------------------------------------------------------------------------
	typedef LOKI_TYPELIST_7(FrxOpenPlugin,
		FrxClosePlugin,
		FrxOpenEditor,
		FrxCloseEditor,
		FrxIsEditorOpen,
		FrxVerbose,
		FrxSetEditorExitOnClose
    ) FrxPrivateFunctionList;
	//-------------------------------------------------------------------------
	typedef LOKI_TYPELIST_24(
		FrxWait,
		FrxGetLastBrowserSelection,
	    FrxSerializePlugin,
		FrxDeserializePlugin,
        FrxMessageBox,
        FrxSelectDirectory,
        FrxSelectFile,
        FrxGetGraphDelay,
        FrxOpenSceneBrowser,
/*10*/  FrxOpenSetup,
        FrxOpenAbout,
        FrxOpenUrl,
        FrxGetVersionString,
        FrxGetVersionInteger,
        FrxQueryDB,
        FrxAddTimer,
        FrxSetPersistData,
        FrxGetPersistData,
        FrxShowInputTextDlg,
/*20*/  FrxRunOnUIThread,
        FrxExec,
        FrxSaveFile,
		FrxShowYesNoDlg,
        FrxGetBuildHash
	) FrxPublicFunctionList;
//-----------------------------------------------------------------------------
void FrxExec::process(const std::string &cmd,
    Ctrl *ctrl, const Ctrl::LuaProcessor &lp)
{
    sambag::lua::LuaStateRef lua = boost::get<0>(lp).lock();
    if(!lua) {
        return;
    }
    try {
        sambag::lua::executeString(lua.get(), cmd);
    } catch(const std::exception &ex) {
        slua::pushLuaError(lua.get(),  ex.what());
    } catch (...) {
        slua::pushLuaError(lua.get(),  "unkown error");
    }

}
//-----------------------------------------------------------------------------
namespace {
    void __runuiimpl(const std::string &cmd, const Ctrl::LuaProcessor &lp) {
        sambag::lua::LuaStateRef lua = boost::get<0>(lp).lock();
        if(!lua) {
            return;
        }
        try {
            SAMBAG_TRY_TO_LOCK_RECURSIVE(*boost::get<1>(lp))
            sambag::lua::executeString(lua.get(), cmd);
        } catch(const std::exception &ex) {
            SAMBAG_LOG_ERR<<ex.what();
        } catch (...) {
            SAMBAG_LOG_ERR<<"unkown error";
        }
    }
}
void FrxRunOnUIThread::process(const std::string &cmd,
    Ctrl *ctrl, const Ctrl::LuaProcessor &lp)
{
    sdc::getWindowToolkit()->invokeLater(
        boost::bind(&__runuiimpl, cmd, lp),
        10
    );
}
//-----------------------------------------------------------------------------
std::string FrxShowInputTextDlg::process(const std::string &title,
    const std::string &txt, Ctrl *ctrl, const Ctrl::LuaProcessor &lp)
{
    using namespace frx::gui;
	using namespace frx::gui::components;
	FRX_START_SCRIPTCALL
	FRX_GET_PLUG
	FRX_GET_EDITOR
    sambag::lua::LuaStateRef lua = boost::get<0>(lp).lock();
    if(!lua) {
        return "";
    }
    try {
        std::string res(txt);
        FrxCircuidViewPtr view = editor->getCircuidView();
        if (!view) {
            return "";
        }
        sdc::Window::Ptr win = view->getFirstContainer<sdc::Window>();
        if (!win) {
            return "";
        }
        if (win->getThreadId() != sambag::com::getThreadId()) {
            throw std::runtime_error("this function need to be called from main thread. Use runOnUIThread for this purpose.");
        }
        ::com::osShowInputTextDlg(title, res, win->getWindowImpl()->getSystemHandle());
        return res;
    } catch(const std::exception &ex) {
        slua::pushLuaError(lua.get(),  ex.what());
    } catch (...) {
        slua::pushLuaError(lua.get(),  "unkown error");
    }
    return "";
}
//-----------------------------------------------------------------------------
void FrxSetPersistData::process(Ctrl *ctrl, const Ctrl::LuaProcessor &lp) {
    sambag::lua::LuaStateRef lua = boost::get<0>(lp).lock();
    if(!lua) {
        return;
    }
    try {
        if(!lua_isstring(lua.get(),  -2)) {
            throw std::runtime_error("arguments mismatch");
        }
        std::string key( lua_tostring(lua.get(),  -2) );
        // first remove old values
        ctrl->getPersistUserData().erase(key);
        
        if(!lua_istable(lua.get(),  -1)) {
            throw std::runtime_error("arguments mismatch");
        }
        int index = -1;
        lua_pushnil(lua.get()); /* first key */
        --index;
        while (lua_next(lua.get(),  index) != 0) {
            boost::tuple<std::string> value;
            slua::pop(lua.get(),  value);
            ctrl->getPersistUserData().insert(std::make_pair(
                key,
                boost::get<0>(value)));
        }
    } catch(const std::exception &ex) {
        slua::pushLuaError(lua.get(),  ex.what());
    } catch (...) {
        slua::pushLuaError(lua.get(),  "unkown error");
    }
}
//-----------------------------------------------------------------------------
slua::IgnoreReturn FrxGetPersistData::process(const std::string &key, Ctrl *ctrl, const Ctrl::LuaProcessor &lp){
    sambag::lua::LuaStateRef lua = boost::get<0>(lp).lock();
    if(!lua) {
        return slua::IgnoreReturn();
    }
    Ctrl::PersistUserData::iterator it, end;
    boost::tie(it, end) = ctrl->getPersistUserData().equal_range(key);
    lua_createtable(lua.get(), 0, 0);
    int tbl = lua_gettop(lua.get());
    int index=0;
    for(; it!=end; ++it) {
        lua_pushinteger(lua.get(), ++index);
        lua_pushstring(lua.get(), it->second.c_str());
        lua_settable(lua.get(), tbl);
    }
    return slua::IgnoreReturn();
}
//-----------------------------------------------------------------------------
namespace {
    void onTimerFailure(const LuaTimer::ExecFailedEvent &ev,
        boost::function<void(std::string)> &handler)
    {
        if (handler) {
            handler(ev.msg);
        }
    }
}
slua::IgnoreReturn FrxAddTimer::
process(const std::string &luaCallback, int ms, int numRepetitions, Ctrl *ctrl, const Ctrl::LuaProcessor &lp) {
    FRX_START_SCRIPTCALL
    sambag::lua::LuaStateRef lua = boost::get<0>(lp).lock();
    if(!lua) {
        return slua::IgnoreReturn();
    }
    LuaTimer::Ptr tm = LuaTimer::createAndPush(
        boost::get<0>(lp), // luastateref
        boost::get<3>(lp), // tracker
        boost::get<1>(lp), // mutex
        luaCallback,
        ms, numRepetitions);
    Ctrl::OnExecErrorF execFHandler = boost::get<2>(lp);
    if (execFHandler) {
        tm->ExecFailedSender::addTrackedEventListener(boost::bind(&onTimerFailure, _2, execFHandler), lua);
    }
    return slua::IgnoreReturn();
}
//-----------------------------------------------------------------------------
slua::IgnoreReturn FrxQueryDB::process(const std::string &query, Ctrl *ctrl, const Ctrl::LuaProcessor &lp) {
    sambag::lua::LuaStateRef lua = boost::get<0>(lp).lock();
    if(!lua) {
        return slua::IgnoreReturn();
    }
	FRX_START_SCRIPTCALL
    try {
        sambag::cpsqlite::DataBase::Ptr db = com::getPluginCollection()->getDataBase();
        if (!db) {
            throw std::runtime_error("accessing datbase failed");
        }
        using namespace sambag::cpsqlite;
        DataBase::Executer::Ptr exec = db->getExecuter();
        DataBase::Results res;
        exec->execute(query, res);
        typedef DataBase::Results::value_type ResultPtr;
        typedef DataBase::Result::Column2Entry Columns;
        lua_createtable(lua.get(), res.size(), 0);
        int tbl = lua_gettop(lua.get()), lindex=0;
        BOOST_FOREACH(ResultPtr x, res) {
            lua_pushinteger(lua.get(), ++lindex);
            lua_createtable(lua.get(), 0, 0);
            int entry = lua_gettop(lua.get());
            BOOST_FOREACH(const Columns::value_type &col, x->col2Entry) { // iterate colums
                lua_pushstring(lua.get(), col.first.c_str()); // first is key
                lua_pushstring(lua.get(), col.second.c_str()); // second is value
                lua_settable(lua.get(), entry);
            }
            lua_settable(lua.get(), tbl);
        }
    } catch(const std::exception &ex) {
        slua::pushLuaError(lua.get(), ex.what());
    } catch (...) {
        slua::pushLuaError(lua.get(), "unkown error");
    }
    
    return sambag::lua::IgnoreReturn();
}
//-----------------------------------------------------------------------------
std::string FrxGetVersionString::process(Ctrl *ctrl, const Ctrl::LuaProcessor &lp) {
    sambag::lua::LuaStateRef lua = boost::get<0>(lp).lock();
    if(!lua) {
        return std::string();
    }
	FRX_START_SCRIPTCALL
	std::stringstream ss;
    ss<<FRX_VERSION_MAJOR<<"."<<FRX_VERSION_MINOR<<"."<<FRX_VERSION_MICRO;
    return ss.str();
}
//-----------------------------------------------------------------------------
int FrxGetVersionInteger::process(Ctrl *ctrl, const Ctrl::LuaProcessor &lp) {
    sambag::lua::LuaStateRef lua = boost::get<0>(lp).lock();
    if(!lua) {
        return 0;
    }
	FRX_START_SCRIPTCALL
    return FRX_VERSION_MAJOR * 100 + FRX_VERSION_MINOR * 10 + FRX_VERSION_MICRO;
}
//-----------------------------------------------------------------------------
std::string FrxGetLastBrowserSelection::process(Ctrl *ctrl, const Ctrl::LuaProcessor &lp) {
    sambag::lua::LuaStateRef lua = boost::get<0>(lp).lock();
    if(!lua) {
        return std::string();
    }
	FRX_START_SCRIPTCALL
	FRX_GET_PLUG
	FRX_GET_EDITOR
    return frx::gui::__lastBrowserSelection;
}
//-----------------------------------------------------------------------------
void FrxOpenSceneBrowser::process(const std::string &path, Ctrl *ctrl, const Ctrl::LuaProcessor &lp) {
    sambag::lua::LuaStateRef lua = boost::get<0>(lp).lock();
    if(!lua) {
        return;
    }
    try {
        FRX_START_SCRIPTCALL
        FRX_GET_PLUG
        FRX_GET_EDITOR
        using namespace frx::gui;
        using namespace frx::gui::components;
        FrxCircuidViewPtr view = editor->getCircuidView();
    
        sdc::Window::Ptr win = view->getFirstContainer<sdc::Window>();
        if (!win) {
            return;
        }
        if (win->getThreadId() != sambag::com::getThreadId()) {
            throw std::runtime_error("this function need to be called from main thread. Use runOnUIThread for this purpose.");
        }
    
        IFrxControl &frxctrl = getFrxControl(view);
        frxctrl.openSceneBrowser(view, path);
    } catch(const std::exception &ex) {
        slua::pushLuaError(lua.get(), ex.what());
    } catch (...) {
        slua::pushLuaError(lua.get(), "unkown error");
    }
}
//-----------------------------------------------------------------------------
void FrxOpenSetup::process(Ctrl *ctrl, const Ctrl::LuaProcessor &lp) {
    sambag::lua::LuaStateRef lua = boost::get<0>(lp).lock();
    if(!lua) {
        return;
    }
    try {
        FRX_START_SCRIPTCALL
        FRX_GET_PLUG
        FRX_GET_EDITOR
        using namespace frx::gui;
        using namespace frx::gui::components;
        FrxCircuidViewPtr view = editor->getCircuidView();
        
        sdc::Window::Ptr win = view->getFirstContainer<sdc::Window>();
        if (!win) {
            return;
        }
        if (win->getThreadId() != sambag::com::getThreadId()) {
            throw std::runtime_error("this function need to be called from main thread. Use runOnUIThread for this purpose.");
        }
        
        IFrxControl &frxctrl = getFrxControl(view);
        frxctrl.openSetup(view);
    } catch(const std::exception &ex) {
        slua::pushLuaError(lua.get(), ex.what());
    } catch (...) {
        slua::pushLuaError(lua.get(), "unkown error");
    }
}
//-----------------------------------------------------------------------------
void FrxOpenAbout::process(Ctrl *ctrl, const Ctrl::LuaProcessor &lp) {
    sambag::lua::LuaStateRef lua = boost::get<0>(lp).lock();
    if(!lua) {
        return;
    }
    try {
        FRX_START_SCRIPTCALL
        FRX_GET_PLUG
        FRX_GET_EDITOR
        FRX_START_SCRIPTCALL
        using namespace frx::gui;
        using namespace frx::gui::components;
        FrxCircuidViewPtr view = editor->getCircuidView();
        
        sdc::Window::Ptr win = view->getFirstContainer<sdc::Window>();
        if (!win) {
            return;
        }
        if (win->getThreadId() != sambag::com::getThreadId()) {
            throw std::runtime_error("this function need to be called from main thread. Use runOnUIThread for this purpose.");
        }
        
        IFrxControl &frxctrl = getFrxControl(view);
        frxctrl.openAbout(view);
    } catch(const std::exception &ex) {
        slua::pushLuaError(lua.get(), ex.what());
    } catch (...) {
        slua::pushLuaError(lua.get(), "unkown error");
    }
}
//-----------------------------------------------------------------------------
void FrxMessageBox::process(const std::string &msg, Ctrl *ctrl, const Ctrl::LuaProcessor &lp) {
    sambag::lua::LuaStateRef lua = boost::get<0>(lp).lock();
    if(!lua) {
        return;
    }
    try {
        FRX_START_SCRIPTCALL
        FRX_GET_PLUG
        FRX_GET_EDITOR
        using namespace frx::gui;
        using namespace frx::gui::components;
        FrxCircuidViewPtr view = editor->getCircuidView();
        
        sdc::Window::Ptr win = view->getFirstContainer<sdc::Window>();
        if (!win) {
            return;
        }
        if (win->getThreadId() != sambag::com::getThreadId()) {
            throw std::runtime_error("this function need to be called from main thread. Use runOnUIThread for this purpose.");
        }
        ::com::osMessageBox("VSTForx", msg, ::com::MSG_ALERT);
    } catch(const std::exception &ex) {
        slua::pushLuaError(lua.get(), ex.what());
    } catch (...) {
        slua::pushLuaError(lua.get(), "unkown error");
    }
}
//-----------------------------------------------------------------------------
bool FrxShowYesNoDlg::process(const std::string &msg, Ctrl *ctrl, const Ctrl::LuaProcessor &lp) {
    sambag::lua::LuaStateRef lua = boost::get<0>(lp).lock();
    if(!lua) {
        return false;
    }
    try {
        FRX_START_SCRIPTCALL
        FRX_GET_PLUG
        FRX_GET_EDITOR
        using namespace frx::gui;
        using namespace frx::gui::components;
        FrxCircuidViewPtr view = editor->getCircuidView();
        
        sdc::Window::Ptr win = view->getFirstContainer<sdc::Window>();
        if (!win) {
            return false;
        }
        if (win->getThreadId() != sambag::com::getThreadId()) {
            throw std::runtime_error("this function need to be called from main thread. Use runOnUIThread for this purpose.");
        }
		return ::com::osMessageBox("VSTForx", msg, 
			::com::MSG_QUESTION) == ::com::MSG_RET_YES;
    } catch(const std::exception &ex) {
        slua::pushLuaError(lua.get(), ex.what());
    } catch (...) {
        slua::pushLuaError(lua.get(), "unkown error");
    }
	return false;
}
//-----------------------------------------------------------------------------
void FrxOpenUrl::process(const std::string &url, Ctrl *ctrl, const Ctrl::LuaProcessor &lp) {
    using namespace frx::gui;
    using namespace frx::gui::components;
    sambag::lua::LuaStateRef lua = boost::get<0>(lp).lock();
    if(!lua) {
        return;
    }
    try {
        FRX_START_SCRIPTCALL
        FRX_GET_PLUG
        FRX_GET_EDITOR
        FrxCircuidViewPtr view = editor->getCircuidView();
        sdc::Window::Ptr win = view->getFirstContainer<sdc::Window>();
        if (!win) {
            return;
        }
        if (win->getThreadId() != sambag::com::getThreadId()) {
            throw std::runtime_error("this function need to be called from main thread. Use runOnUIThread for this purpose.");
        }
        
        // check whether url is trusted
        boost::regex trusted("http://[a-zA-Z0-9]*\\.vstforx\\.de/.*$");
        if (boost::regex_match(url, trusted)) {
            ::com::osOpenLink(url);
            return;
        }
        
        std::stringstream msg;
        msg<<"You are going to visit the url '"<<url<<"'.";
        msg<<" Which is not an official VSTForx site. Do you want to proceed?";
        
		bool res = ::com::osMessageBox("VSTForx", msg.str(),
			::com::MSG_QUESTION) == ::com::MSG_RET_YES;
        if (res) {
            ::com::osOpenLink(url);
        }
    
    
    } catch(const std::exception &ex) {
        slua::pushLuaError(lua.get(), ex.what());
    } catch (...) {
        slua::pushLuaError(lua.get(), "unkown error");
    }
}
//-----------------------------------------------------------------------------
std::string FrxSelectFile::process(const std::string &startPath, Ctrl *ctrl, const Ctrl::LuaProcessor &lp) {
    sambag::lua::LuaStateRef lua = boost::get<0>(lp).lock();
    if(!lua) {
        return std::string();
    }
    try {
        FRX_START_SCRIPTCALL
        FRX_GET_PLUG
        FRX_GET_EDITOR
        using namespace frx::gui;
        using namespace frx::gui::components;
        FrxCircuidViewPtr view = editor->getCircuidView();
        
        sdc::Window::Ptr win = view->getFirstContainer<sdc::Window>();
        if (!win) {
            return "";
        }
        if (win->getThreadId() != sambag::com::getThreadId()) {
            throw std::runtime_error("this function need to be called from main thread. Use runOnUIThread for this purpose.");
        }
        return ::com::osSelectFile("select file", startPath, win->getWindowImpl()->getSystemHandle());
    } catch(const std::exception &ex) {
        slua::pushLuaError(lua.get(), ex.what());
    } catch (...) {
        slua::pushLuaError(lua.get(), "unkown error");
    }
    return "";
}
//-----------------------------------------------------------------------------
std::string FrxSaveFile::process(const std::string &startPath, Ctrl *ctrl, const Ctrl::LuaProcessor &lp) {
    sambag::lua::LuaStateRef lua = boost::get<0>(lp).lock();
    if(!lua) {
        return std::string();
    }
    try {
        FRX_START_SCRIPTCALL
        FRX_GET_PLUG
        FRX_GET_EDITOR
        using namespace frx::gui;
        using namespace frx::gui::components;
        FrxCircuidViewPtr view = editor->getCircuidView();
        
        sdc::Window::Ptr win = view->getFirstContainer<sdc::Window>();
        if (!win) {
            return "";
        }
        if (win->getThreadId() != sambag::com::getThreadId()) {
            throw std::runtime_error("this function need to be called from main thread. Use runOnUIThread for this purpose.");
        }
        return ::com::osSaveFile("select file", startPath, win->getWindowImpl()->getSystemHandle());
    } catch(const std::exception &ex) {
        slua::pushLuaError(lua.get(), ex.what());
    } catch (...) {
        slua::pushLuaError(lua.get(), "unkown error");
    }
    return "";
}

//-----------------------------------------------------------------------------
std::string FrxSelectDirectory::process(const std::string &startPath, Ctrl *ctrl, const Ctrl::LuaProcessor &lp) {
    sambag::lua::LuaStateRef lua = boost::get<0>(lp).lock();
    if(!lua) {
        return std::string();
    }
    try {
        FRX_START_SCRIPTCALL
        FRX_GET_PLUG
        FRX_GET_EDITOR
        using namespace frx::gui;
        using namespace frx::gui::components;
        FrxCircuidViewPtr view = editor->getCircuidView();
        
        sdc::Window::Ptr win = view->getFirstContainer<sdc::Window>();
        if (!win) {
            return "";
        }
        if (win->getThreadId() != sambag::com::getThreadId()) {
            throw std::runtime_error("this function need to be called from main thread. Use runOnUIThread for this purpose.");
        }
        return ::com::osSelectFile("select directory", startPath, win->getWindowImpl()->getSystemHandle());
    } catch(const std::exception &ex) {
        slua::pushLuaError(lua.get(), ex.what());
    } catch (...) {
        slua::pushLuaError(lua.get(), "unkown error");
    }
    return "";
}
//-----------------------------------------------------------------------------
int FrxGetGraphDelay::process(Ctrl *ctrl, const Ctrl::LuaProcessor &lp) {
    sambag::lua::LuaStateRef lua = boost::get<0>(lp).lock();
    if(!lua) {
        return 0;
    }
	FRX_START_SCRIPTCALL
	FRX_GET_PLUG
    FRX_GET_EDITOR
	using namespace frx::gui;
	using namespace frx::gui::components;
	using namespace frx::processing;
    FrxCircuidViewPtr view = editor->getCircuidView();
	IFrxControl &frxctrl = getFrxControl(view);
    frx::processing::IModelController::Ptr mctrl;
	IViewModelMap::Ptr map;
	boost::tie(mctrl, map) = getControllerAndMap(view);
    return mctrl->getGraphDelay();
}
//-----------------------------------------------------------------------------
void FrxSetEditorExitOnClose::process(Bool val, Ctrl *ctrl, const Ctrl::LuaProcessor &lp) {
    sambag::lua::LuaStateRef lua = boost::get<0>(lp).lock();
    if(!lua) {
        return;
    }
	FRX_START_SCRIPTCALL
	FRX_GET_EDITOR
	using sambag::disco::components::Window;
	editor->getHostWindow()->setDefaultCloseOperation( 
		val == True ? Window::EXIT_ON_CLOSE :
		Window::DISPOSE_ON_CLOSE
	);
}
//-----------------------------------------------------------------------------
void FrxVerbose::process(Bool val, Ctrl *ctrl, const Ctrl::LuaProcessor &lp) {
    sambag::lua::LuaStateRef lua = boost::get<0>(lp).lock();
    if(!lua) {
        return;
    }
	FRX_START_SCRIPTCALL
	ctrl->setVerbose( val == 1 ? true : false );
}
//-----------------------------------------------------------------------------
Bool FrxIsEditorOpen::process(Ctrl *ctrl, const Ctrl::LuaProcessor &lp) {
    sambag::lua::LuaStateRef lua = boost::get<0>(lp).lock();
    if(!lua) {
        return false;
    }
	FRX_START_SCRIPTCALL
	FRX_GET_EDITOR
	return editor->isOpen() ? True : False;
}
//-----------------------------------------------------------------------------
std::string FrxSerializePlugin::process(Ctrl *ctrl, const Ctrl::LuaProcessor &lp) {
    sambag::lua::LuaStateRef lua = boost::get<0>(lp).lock();
    if(!lua) {
        return std::string();
    }
	FRX_START_SCRIPTCALL
	FRX_GET_PLUG
	char *chunk[1];
	int bytes = plug->getChunk((void**)&chunk[0]);
	if (bytes == 0)
		return "";
	char * res = chunk[0];
	return std::string(res);
}
//-----------------------------------------------------------------------------
void FrxDeserializePlugin::process(const std::string &bytes, Ctrl *ctrl, const Ctrl::LuaProcessor &lp) {
    sambag::lua::LuaStateRef lua = boost::get<0>(lp).lock();
    if(!lua) {
        return;
    }
	FRX_START_SCRIPTCALL
	FRX_GET_PLUG
	plug->setChunk((void*)bytes.c_str(), bytes.size());
}
//-----------------------------------------------------------------------------
void FrxOpenPlugin::process(Ctrl *ctrl, const Ctrl::LuaProcessor &lp) {
    sambag::lua::LuaStateRef lua = boost::get<0>(lp).lock();
    if(!lua) {
        return;
    }
	FRX_START_SCRIPTCALL
	FRX_GET_PLUG
	FRX_GET_EDITOR
	plug->open();
	boost::this_thread::sleep(boost::posix_time::milliseconds(FRX_OPENCLOSE_WORKAROUND_WAIT));
}
//-----------------------------------------------------------------------------
void FrxClosePlugin::process(Ctrl *ctrl, const Ctrl::LuaProcessor &lp) {
    sambag::lua::LuaStateRef lua = boost::get<0>(lp).lock();
    if(!lua) {
        return;
    }
	FRX_START_SCRIPTCALL
	FRX_GET_PLUG
	FRX_GET_EDITOR
	plug->close();
	boost::this_thread::sleep(boost::posix_time::milliseconds(FRX_OPENCLOSE_WORKAROUND_WAIT));
}
//-----------------------------------------------------------------------------
void onEditorOpen(void *src, const frx::gui::components::ViewIsReadyEvent &ev, bool *isOpen)  {
	*isOpen = true;
}
//-----------------------------------------------------------------------------
void FrxOpenEditor::process(Ctrl *ctrl, const Ctrl::LuaProcessor &lp) {
    sambag::lua::LuaStateRef lua = boost::get<0>(lp).lock();
    if(!lua) {
        return;
    }
	FRX_START_SCRIPTCALL
	FRX_GET_PLUG
	FRX_GET_EDITOR
	editor->open();
	if (editor->getHostWindow()->isOpen()) {
		return;
	}
	bool isOpen = false;
    using frx::gui::components::ViewIsReadyEvent;
	sdc::Window::OnOpenEventSender::Connection evcn =
        editor->sce::EventSender<ViewIsReadyEvent>::addEventListener(
			boost::bind(&onEditorOpen, _1, _2, &isOpen)
		);
	while (!isOpen) {
		boost::this_thread::sleep(boost::posix_time::milliseconds(100));
	}
	evcn.disconnect();
    boost::this_thread::sleep(boost::posix_time::milliseconds(FRX_OPENCLOSE_WORKAROUND_WAIT));
}
//-----------------------------------------------------------------------------
void onEditorClose(void *src, const sambag::disco::components::OnCloseEvent &ev, bool *isClose)  {
	*isClose = true;
}
//-----------------------------------------------------------------------------
void FrxCloseEditor::process(Ctrl *ctrl, const Ctrl::LuaProcessor &lp) {
    sambag::lua::LuaStateRef lua = boost::get<0>(lp).lock();
    if(!lua) {
        return;
    }
	FRX_START_SCRIPTCALL
	FRX_GET_PLUG
	FRX_GET_EDITOR
	editor->close();
	if (!editor->getHostWindow()->isOpen()) {
		return;
	}
	bool isClose = false;
	sambag::disco::components::Window::OnCloseEventSender::Connection evcn = 
		editor->getHostWindow()->addOnCloseEventListener(
			boost::bind(&onEditorClose, _1, _2, &isClose)
		);
	while (!isClose) {
		boost::this_thread::sleep(boost::posix_time::milliseconds(100));
	}
	evcn.disconnect();
    boost::this_thread::sleep(boost::posix_time::milliseconds(FRX_OPENCLOSE_WORKAROUND_WAIT));
}
//-----------------------------------------------------------------------------
void FrxWait::process(int millis, Ctrl *ctrl, const Ctrl::LuaProcessor &lp) {
    sambag::lua::LuaStateRef lua = boost::get<0>(lp).lock();
    if(!lua) {
        return;
    }
	FRX_START_SCRIPTCALL
	FRX_GET_PLUG
	FRX_GET_EDITOR
	boost::this_thread::sleep(boost::posix_time::milliseconds(millis));
}
} // namespace
//=============================================================================
//  Class PluginScriptCtrl
//=============================================================================
//-----------------------------------------------------------------------------
PluginScriptCtrl::PluginScriptCtrl(bool isPublic) :
    isPublic(isPublic), plug(NULL), editor(NULL)
{
}
//-----------------------------------------------------------------------------
PluginScriptCtrl::LuaPtr 
PluginScriptCtrl::getLuaPtr(FrxComponentPtr c)
{
	std::string key(sambag::com::toString(c.get()));
	componentMap[key] = c;
	return key;
}
//-----------------------------------------------------------------------------
PluginScriptCtrl::FrxComponentPtr 
PluginScriptCtrl::getFrxComponent(const LuaPtr &c)
{
	ComponentMap::const_iterator it = componentMap.find(c);
	if (it==componentMap.end()) {
		return FrxComponentPtr();
	}
	return it->second;
}
//-----------------------------------------------------------------------------
void PluginScriptCtrl::remove(const LuaPtr &c)
{
	componentMap.erase(c);
	modelObjectMap.erase(c);
}
//-----------------------------------------------------------------------------
LuaPtr PluginScriptCtrl::getLuaPtr(ModelObjectPtr c) {
	std::string key(sambag::com::toString(c.get()));
	modelObjectMap[key] = c;
	return key;
}
//-----------------------------------------------------------------------------
PluginScriptCtrl::ModelObjectPtr 
PluginScriptCtrl::getModelObject(const LuaPtr &c) 
{
	ModelObjectMap::const_iterator it = modelObjectMap.find(c);
	if (it==modelObjectMap.end()) {
		return ModelObjectPtr();
	}
	return it->second;
}
//-----------------------------------------------------------------------------
sambag::disco::components::WindowPtr PluginScriptCtrl::getEditorWindow() const {
	if (!editor) {
		return sambag::disco::components::WindowPtr();
	}
	return editor->getHostWindow();
}
//-----------------------------------------------------------------------------
PluginScriptCtrl::MutexPtr PluginScriptCtrl::getMutex() const {
    if (!__scriptCallMutex) {
        __scriptCallMutex = MutexPtr( new Mutex() );
    }
    return __scriptCallMutex;
}
//-----------------------------------------------------------------------------
void PluginScriptCtrl::setPlugin(frx::processing::VstForxPlug *plug) {
	if (!plug) {
		return;
	}
	this->plug = plug;
	using namespace sambag::disco::components;
	editor =
		static_cast<frx::gui::components::VstForxEditor*>(plug->getEditor());
	if (!editor) {
		SAMBAG_THROW(
			sambag::com::exceptions::IllegalStateException,
			"VstForxEditor = NULL"
		);
	}
    
    using namespace sambag::lua;
	__luaState = createLuaStateRef();
	registerFunctions(
        LuaProcessor(__luaState,
                     getMutex(),
                     OnExecErrorF(),
                     __luaState
        ), isPublic, true);
}
//-----------------------------------------------------------------------------
void PluginScriptCtrl::appendJob(const std::string &str) {
	scripts.push_back(str);
}
//-----------------------------------------------------------------------------
void PluginScriptCtrl::start() {
	using namespace sambag::lua;
	thread = boost::thread(
		boost::bind(&PluginScriptCtrl::runThread, this)
	);
}
//-----------------------------------------------------------------------------
void PluginScriptCtrl::join() {
	thread.join();
}
//-----------------------------------------------------------------------------
void PluginScriptCtrl::execute(const std::string &str) {
	using namespace sambag::lua;
	SAMBAG_TRY_TO_LOCK_RECURSIVE(getMutexRef())
	executeString(__luaState.get(), str);
}
//-----------------------------------------------------------------------------
void PluginScriptCtrl::executeFile(const std::string &path) {
	SAMBAG_TRY_TO_LOCK_RECURSIVE(getMutexRef())
	sambag::lua::executeFile(__luaState.get(), path);
}
//-----------------------------------------------------------------------------
void PluginScriptCtrl::runThread() {
	using namespace sambag::lua;
	BOOST_FOREACH(const std::string &script, scripts) {
		try {
			SAMBAG_TRY_TO_LOCK_RECURSIVE(getMutexRef())
			executeString(__luaState.get(), script);
		} catch(const ExecutionFailed &ex) {
			SAMBAG_LOG_ERR<<"executation failed: "<<ex.errMsg;
			sce::EventSender<ScriptExeFailedEvent>::notifyListeners(
				this,
				ScriptExeFailedEvent(ex.errMsg)
			);
			break;
		}
	}
	sce::EventSender<ScriptEnded>::notifyListeners(
		this,
		ScriptEnded()
	);
}
//-----------------------------------------------------------------------------
void PluginScriptCtrl::__startScriptCall(const std::string &fname) {
	if (fname!="" && verbose) {
        if (fname == "frxOpenEditor" ||
            fname == "frxOpenPlugin" ||
            fname == "frxCloseEditor" ||
            fname == "frxClosePlugin")
        {
            sambag::com::log("call " + fname);
        }
	}
	lastCall = fname;
}
//-----------------------------------------------------------------------------
void PluginScriptCtrl::__endScriptCall() {
	//boost::this_thread::sleep(boost::posix_time::milliseconds(100));
}

//-----------------------------------------------------------------------------
PluginScriptCtrl::LuaState PluginScriptCtrl::getLuaState()  {
    LockPtr lock( new Lock(getMutexRef(), boost::try_to_lock));
	if (!lock->owns_lock()) {
        lock->timed_lock(boost::get_system_time() +
        boost::posix_time::seconds(SAMBAG_LOCK_TIMEOUT));
    }
	if ( !lock->owns_lock() ) {
        SAMBAG_THROW(SAMBAG_DEADLOCK_EXCEPTION, "PluginScriptCtrl: deadlock exception");
    }
    return LuaState(__luaState, lock);
}
///////////////////////////////////////////////////////////////////////////////
// register function approach
namespace {
template <int Val>
struct Int2Type {
	enum { Value = Val };
};
template <class FrxFunction, class Result>
void __bind(Ctrl *ctrl, const Ctrl::LuaProcessor &lp,
    Result &out,
    Int2Type<0>)
{
	out = boost::bind(&FrxFunction::process, ctrl, lp);
}
template <class FrxFunction, class Result>
void __bind(Ctrl *ctrl, const Ctrl::LuaProcessor &lp,
    Result &out,
    Int2Type<1>)
{
    out = boost::bind(&FrxFunction::process, _1, ctrl, lp);
}
template <class FrxFunction, class Result>
void __bind(Ctrl *ctrl, const Ctrl::LuaProcessor &lp,
    Result &out,
    Int2Type<2>)
{
    out = boost::bind(&FrxFunction::process, _1, _2, ctrl, lp);
}
template <class FrxFunction, class Result>
void __bind(Ctrl *ctrl, const Ctrl::LuaProcessor &lp,
    Result &out,
    Int2Type<3>)
{
    out = boost::bind(&FrxFunction::process, _1, _2, _3, ctrl, lp);
}
template <class FrxFunction, class Result>
void __bind(Ctrl *ctrl, const Ctrl::LuaProcessor &lp,
    Result &out,
    Int2Type<4>)
{
    out = boost::bind(&FrxFunction::process, _1, _2, _3, _4, ctrl, lp);
}
template <class FrxFunction, class Result>
void __bind(Ctrl *ctrl, const Ctrl::LuaProcessor &lp,
    Result &out,
    Int2Type<5>)
{
    out = boost::bind(&FrxFunction::process, _1, _2, _3, _4, _5, ctrl, lp);
}


template <class _FList>
struct Functions {
    typedef _FList FList;
    Functions(PluginScriptCtrl *master, const Ctrl::LuaProcessor &lp) :
        master(master), lp(lp) {}
    PluginScriptCtrl *master;
    const Ctrl::LuaProcessor &lp;
};

template <class Functions>
struct Accessor {;
    template <int Index, class _Result>
    static void get(const Functions &f, _Result &out) {
        typedef typename Loki::TL::TypeAt<typename Functions::FList, Index>::Result FX;
        enum { NumArgs = FX::Function::arity };
        __bind<FX>(f.master, f.lp, out, Int2Type<NumArgs>());
    }
};

} // namespace(s)
//-----------------------------------------------------------------------------
void PluginScriptCtrl::registerFunctions(const LuaProcessor &lp,
    bool isPublic, bool includeView)
{
    sambag::lua::LuaStateRef state = boost::get<0>(lp).lock();
    if (!state) {
        return;
    }
    if (isPublic) {
        typedef Loki::TL::NoDuplicates<FrxPublicFunctionList>::Result Fz; // arf
        sambag::lua::registerFunctions<Fz, Accessor>(
            state.get(), Functions<Fz>(this, lp), "frx"
        );
    } else {
        typedef Loki::TL::Append<FrxPublicFunctionList,
            FrxPrivateFunctionList>::Result FsUnion;
        typedef Loki::TL::NoDuplicates<FsUnion>::Result Fz; // arf
        sambag::lua::registerFunctions<Fz, Accessor>(
            state.get(), Functions<Fz>(this, lp), "frx"
        );

    }
    if (includeView) {
        lua_getglobal(state.get(), "frx");
        int index = lua_gettop(state.get());
        LuaFrxView::createAndPush(state.get(), editor);
        lua_setfield(state.get(), index, "view");
        lua_pop(state.get(), 1); // pop frx
    }
}
}} // namespace(s)
