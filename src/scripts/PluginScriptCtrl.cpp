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
#include <sambag/disco/components/Window.hpp>
#include "LuaParameter.hpp"
#include "LuaFrxView.hpp"
#include <com/PluginCollection.h>
#include <sambag/com/Common.hpp>
#include "LuaTimer.hpp"

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
		static void process(Ctrl *ctrl);
	};
	//-------------------------------------------------------------------------
	struct FrxClosePlugin {
		typedef boost::function<void()> Function;
		static const char * name() { return "closePlugin"; }
		static void process(Ctrl *ctrl);
	};
	//-------------------------------------------------------------------------
	struct FrxOpenEditor {
		typedef boost::function<void()> Function;
		static const char * name() { return "openEditor"; }
		static void process(Ctrl *ctrl);
	};
	//-------------------------------------------------------------------------
	struct FrxCloseEditor {
		typedef boost::function<void()> Function;
		static const char * name() { return "closeEditor"; }
		static void process(Ctrl *ctrl);
	};
	//-------------------------------------------------------------------------
	struct FrxWait {
		typedef boost::function<void(int)> Function;
		static const char * name() { return "wait"; }
		static void process(int sec, Ctrl *ctrl);
	};
	//-------------------------------------------------------------------------
	struct FrxSerializePlugin {
		typedef boost::function<std::string()> Function;
		static const char * name() { return "serializePlugin"; }
		static std::string process(Ctrl *ctrl);
	};
	//-------------------------------------------------------------------------
	struct FrxDeserializePlugin {
		typedef boost::function<void(std::string)> Function;
		static const char * name() { return "deserializePlugin"; }
		static void process(const std::string &, Ctrl *ctrl);
	};
	//-------------------------------------------------------------------------
	struct FrxIsEditorOpen {
		typedef boost::function<Bool()> Function;
		static const char * name() { return "isEditorOpen"; }
		static Bool process(Ctrl *ctrl);
	};
	//-------------------------------------------------------------------------
	struct FrxVerbose {
		typedef boost::function<void(Bool val)> Function;
		static const char * name() { return "verbose"; }
		static void process(Bool val, Ctrl *ctrl);
	};
	//-------------------------------------------------------------------------
	struct FrxGetGraphDelay {
        typedef boost::function<int()> Function;
		static const char * name() { return "getGraphDelay"; }
		static int process(Ctrl *ctrl);
	};
	//-------------------------------------------------------------------------
	struct FrxMessageBox {
        typedef boost::function<void(std::string)> Function;
		static const char * name() { return "messageBox"; }
		static void process(const std::string&, Ctrl *ctrl);
	};
	//-------------------------------------------------------------------------
	struct FrxSelectFile {
        typedef boost::function<std::string(std::string)> Function;
		static const char * name() { return "selectFile"; }
		static std::string process(const std::string&, Ctrl *ctrl);
	};
	//-------------------------------------------------------------------------
	struct FrxSelectDirectory {
        typedef boost::function<std::string(std::string)> Function;
		static const char * name() { return "selectDirectory"; }
		static std::string process(const std::string&, Ctrl *ctrl);
	};
	//-------------------------------------------------------------------------
	struct FrxOpenSceneBrowser {
        typedef boost::function<void(std::string)> Function;
		static const char * name() { return "openSceneBrowser"; }
		static void process(const std::string&, Ctrl *ctrl);
	};
	//-------------------------------------------------------------------------
	struct FrxOpenUrl {
        typedef boost::function<void(std::string)> Function;
		static const char * name() { return "openUrl"; }
		static void process(const std::string&, Ctrl *ctrl);
	};
	//-------------------------------------------------------------------------
	struct FrxOpenSetup {
        typedef boost::function<void()> Function;
		static const char * name() { return "openSetup"; }
		static void process(Ctrl *ctrl);
	};
	//-------------------------------------------------------------------------
	struct FrxOpenAbout {
        typedef boost::function<void()> Function;
		static const char * name() { return "openAbout"; }
		static void process(Ctrl *ctrl);
	};
	//-------------------------------------------------------------------------
	struct FrxGetLastBrowserSelection {
        typedef boost::function<std::string()> Function;
		static const char * name() { return "getLastSceneBrowserSelection"; }
		static std::string process(Ctrl *ctrl);
	};
	//-------------------------------------------------------------------------
	struct FrxGetVersionInteger {
        typedef boost::function<int()> Function;
		static const char * name() { return "getVersionInteger"; }
		static int process(Ctrl *ctrl);
	};
	//-------------------------------------------------------------------------
	struct FrxGetVersionString {
        typedef boost::function<std::string()> Function;
		static const char * name() { return "getVersionString"; }
		static std::string process(Ctrl *ctrl);
	};
	//-------------------------------------------------------------------------
	struct FrxResetMainMenu {
        typedef boost::function<void()> Function;
		static const char * name() { return "resetMainMenu"; }
		static void process(Ctrl *ctrl);
	};
	//-------------------------------------------------------------------------
	struct FrxSetEditorExitOnClose {
		typedef boost::function<void(Bool)> Function;
		static const char * name() { return "setEditorExitOnClose"; }
		static void process(Bool val, Ctrl *ctrl);
	};
	//-------------------------------------------------------------------------
	struct FrxQueryDB {
		typedef boost::function<slua::IgnoreReturn(std::string)> Function;
		static const char * name() { return "queryDB"; }
		static slua::IgnoreReturn process(const std::string &query, Ctrl *ctrl);
	};
	//-------------------------------------------------------------------------
	struct FrxAddTimer {
		typedef boost::function<slua::IgnoreReturn(std::string, int, int)> Function;
		static const char * name() { return "addTimer"; }
		static slua::IgnoreReturn process(const std::string &, int, int, Ctrl *ctrl);
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
	typedef LOKI_TYPELIST_17(
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
        FrxResetMainMenu,
        FrxQueryDB,
        FrxAddTimer
	) FrxPublicFunctionList;
//-----------------------------------------------------------------------------
slua::IgnoreReturn FrxAddTimer::
process(const std::string &luaCallback, int ms, int numRepetitions, Ctrl *ctrl)
{
    FRX_START_SCRIPTCALL
    PluginScriptCtrl::LuaState l = ctrl->getLuaState();
    LuaTimer::Ptr tm = LuaTimer::createAndPush(
        l.first.get(), ctrl->getMutex(), luaCallback, ms, numRepetitions);
    return slua::IgnoreReturn();
}
//-----------------------------------------------------------------------------
slua::IgnoreReturn FrxQueryDB::process(const std::string &query, Ctrl *ctrl)
{
	FRX_START_SCRIPTCALL
    PluginScriptCtrl::LuaState l = ctrl->getLuaState();
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
        lua_createtable(l.first.get(), res.size(), 0);
        int tbl = lua_gettop(l.first.get()), lindex=0;
        BOOST_FOREACH(ResultPtr x, res) {
            lua_pushinteger(l.first.get(), ++lindex);
            lua_createtable(l.first.get(), 0, 0);
            int entry = lua_gettop(l.first.get());
            BOOST_FOREACH(const Columns::value_type &col, x->col2Entry) { // iterate colums
                lua_pushstring(l.first.get(), col.first.c_str()); // first is key
                lua_pushstring(l.first.get(), col.second.c_str()); // second is value
                lua_settable(l.first.get(), entry);
            }
            lua_settable(l.first.get(), tbl);
        }
    } catch(const std::exception &ex) {
        slua::pushLuaError(l.first.get(), ex.what());
    } catch (...) {
        slua::pushLuaError(l.first.get(), "unkown error");
    }
    
    return sambag::lua::IgnoreReturn();
}
//-----------------------------------------------------------------------------
void FrxResetMainMenu::process(Ctrl *ctrl) {
	FRX_START_SCRIPTCALL
	FRX_GET_PLUG
	FRX_GET_EDITOR
    using namespace frx::gui;
	using namespace frx::gui::components;
    FrxCircuidViewPtr view = editor->getCircuidView();
    view->setComponentPopupMenu( sdc::PopupMenuPtr() );
}
//-----------------------------------------------------------------------------
std::string FrxGetVersionString::process(Ctrl *ctrl) {
	FRX_START_SCRIPTCALL
	std::stringstream ss;
    ss<<FRX_VERSION_MAJOR<<"."<<FRX_VERSION_MINOR<<"."<<FRX_VERSION_MICRO;
    return ss.str();
}
//-----------------------------------------------------------------------------
int FrxGetVersionInteger::process(Ctrl *ctrl) {
	FRX_START_SCRIPTCALL
    return FRX_VERSION_MAJOR * 100 + FRX_VERSION_MINOR * 10 + FRX_VERSION_MICRO;
}
//-----------------------------------------------------------------------------
std::string FrxGetLastBrowserSelection::process(Ctrl *ctrl) {
	FRX_START_SCRIPTCALL
	FRX_GET_PLUG
	FRX_GET_EDITOR
    return frx::gui::__lastBrowserSelection;
}
//-----------------------------------------------------------------------------
void FrxOpenSceneBrowser::process(const std::string &path, Ctrl *ctrl) {
	FRX_START_SCRIPTCALL
	FRX_GET_PLUG
	FRX_GET_EDITOR
    using namespace frx::gui;
	using namespace frx::gui::components;
    FrxCircuidViewPtr view = editor->getCircuidView();
	IFrxControl &frxctrl = getFrxControl(view);
    frxctrl.openSceneBrowser(view, path);
}
//-----------------------------------------------------------------------------
void FrxOpenSetup::process(Ctrl *ctrl) {
	FRX_START_SCRIPTCALL
	FRX_GET_PLUG
	FRX_GET_EDITOR
    using namespace frx::gui;
	using namespace frx::gui::components;
    FrxCircuidViewPtr view = editor->getCircuidView();
	IFrxControl &frxctrl = getFrxControl(view);
    frxctrl.openSetup(view);
}
//-----------------------------------------------------------------------------
void FrxOpenAbout::process(Ctrl *ctrl) {
	FRX_START_SCRIPTCALL
	FRX_GET_PLUG
	FRX_GET_EDITOR
	FRX_START_SCRIPTCALL
    using namespace frx::gui;
	using namespace frx::gui::components;
    FrxCircuidViewPtr view = editor->getCircuidView();
	IFrxControl &frxctrl = getFrxControl(view);
    frxctrl.openAbout(view);
}
//-----------------------------------------------------------------------------
void FrxMessageBox::process(const std::string &msg, Ctrl *ctrl) {
	FRX_START_SCRIPTCALL
	FRX_GET_PLUG
	FRX_GET_EDITOR
    ::com::osMessageBox("Lua", msg, ::com::MSG_ALERT);
}
//-----------------------------------------------------------------------------
void FrxOpenUrl::process(const std::string &url, Ctrl *ctrl) {
	FRX_START_SCRIPTCALL
	FRX_GET_PLUG
	FRX_GET_EDITOR
    ::com::openLink(url);
}
//-----------------------------------------------------------------------------
std::string FrxSelectFile::process(const std::string &startPath, Ctrl *ctrl) {
	FRX_START_SCRIPTCALL
	FRX_GET_PLUG
	FRX_GET_EDITOR
    return ::com::osSelectFile("select file", startPath, NULL);
}
//-----------------------------------------------------------------------------
std::string FrxSelectDirectory::process(const std::string &startPath, Ctrl *ctrl) {
	FRX_START_SCRIPTCALL
	FRX_GET_PLUG
	FRX_GET_EDITOR
    return ::com::osSelectFile("select directory", startPath, NULL);
}
//-----------------------------------------------------------------------------
int FrxGetGraphDelay::process(Ctrl *ctrl)
{
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
void FrxSetEditorExitOnClose::process(Bool val, Ctrl *ctrl) {
	FRX_START_SCRIPTCALL
	FRX_GET_EDITOR
	using sambag::disco::components::Window;
	editor->getHostWindow()->setDefaultCloseOperation( 
		val == True ? Window::EXIT_ON_CLOSE :
		Window::DISPOSE_ON_CLOSE
	);
}
//-----------------------------------------------------------------------------
void FrxVerbose::process(Bool val, Ctrl *ctrl) {
	FRX_START_SCRIPTCALL
	ctrl->setVerbose( val == 1 ? true : false );
}
//-----------------------------------------------------------------------------
Bool FrxIsEditorOpen::process(Ctrl *ctrl) {
	FRX_START_SCRIPTCALL
	FRX_GET_EDITOR
	return editor->isOpen() ? True : False;
}
//-----------------------------------------------------------------------------
std::string FrxSerializePlugin::process(Ctrl *ctrl) {
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
void FrxDeserializePlugin::process(const std::string &bytes, Ctrl *ctrl) {
	FRX_START_SCRIPTCALL
	FRX_GET_PLUG
	plug->setChunk((void*)bytes.c_str(), bytes.size());
}
//-----------------------------------------------------------------------------
void FrxOpenPlugin::process(Ctrl *ctrl) {
	FRX_START_SCRIPTCALL
	FRX_GET_PLUG
	FRX_GET_EDITOR
	plug->open();
	boost::this_thread::sleep(boost::posix_time::milliseconds(FRX_OPENCLOSE_WORKAROUND_WAIT));
}
//-----------------------------------------------------------------------------
void FrxClosePlugin::process(Ctrl *ctrl) {
	FRX_START_SCRIPTCALL
	FRX_GET_PLUG
	FRX_GET_EDITOR
	plug->close();
	boost::this_thread::sleep(boost::posix_time::milliseconds(FRX_OPENCLOSE_WORKAROUND_WAIT));
}
//-----------------------------------------------------------------------------
void onEditorOpen(void *src, const frx::gui::components::ViewIsReadyEvent &ev, bool *isOpen) 
{
	*isOpen = true;
}
//-----------------------------------------------------------------------------
void FrxOpenEditor::process(Ctrl *ctrl) {
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
void onEditorClose(void *src, const sambag::disco::components::OnCloseEvent &ev, bool *isClose) 
{
	*isClose = true;
}
//-----------------------------------------------------------------------------
void FrxCloseEditor::process(Ctrl *ctrl) {
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
void FrxWait::process(int millis, Ctrl *ctrl) {
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
	luaState = createLuaStateRef();
	registerFunctions(luaState, isPublic);
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
	SAMBAG_TRY_TO_LOCK_RECURSIVE(scriptCallMutex)
	executeString(luaState.get(), str);
}
//-----------------------------------------------------------------------------
void PluginScriptCtrl::executeFile(const std::string &path) {
	SAMBAG_TRY_TO_LOCK_RECURSIVE(scriptCallMutex)
	sambag::lua::executeFile(luaState.get(), path);
}
//-----------------------------------------------------------------------------
void PluginScriptCtrl::runThread() {
	using namespace sambag::lua;
	BOOST_FOREACH(const std::string &script, scripts) {
		try {
			SAMBAG_TRY_TO_LOCK_RECURSIVE(scriptCallMutex)
			executeString(luaState.get(), script);
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
PluginScriptCtrl::LuaState PluginScriptCtrl::getLuaState()  {
    LockPtr lock( new Lock(scriptCallMutex, boost::try_to_lock));
	if (!lock->owns_lock()) {
        lock->timed_lock(boost::get_system_time() +
        boost::posix_time::seconds(SAMBAG_LOCK_TIMEOUT));
    }
	if ( !lock->owns_lock() ) {
        SAMBAG_THROW(SAMBAG_DEADLOCK_EXCEPTION, "PluginScriptCtrl: deadlock exception");
    }
    return LuaState(luaState, lock);
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
///////////////////////////////////////////////////////////////////////////////
// register function approach
namespace {
template <int Val>
struct Int2Type {
	enum { Value = Val };
};
template <class FrxFunction, class Result>
void __bind(Ctrl *ctrl,
    Result &out,
    Int2Type<0>)
{
	out = boost::bind(&FrxFunction::process, ctrl);
}
template <class FrxFunction, class Result>
void __bind(Ctrl *ctrl,
    Result &out,
    Int2Type<1>)
{
    out = boost::bind(&FrxFunction::process, _1, ctrl);
}
template <class FrxFunction, class Result>
void __bind(Ctrl *ctrl,
    Result &out,
    Int2Type<2>)
{
    out = boost::bind(&FrxFunction::process, _1, _2, ctrl);
}
template <class FrxFunction, class Result>
void __bind(Ctrl *ctrl,
    Result &out,
    Int2Type<3>)
{
    out = boost::bind(&FrxFunction::process, _1, _2, _3, ctrl);
}
template <class FrxFunction, class Result>
void __bind(Ctrl *ctrl,
    Result &out,
    Int2Type<4>)
{
    out = boost::bind(&FrxFunction::process, _1, _2, _3, _4, ctrl);
}
template <class FrxFunction, class Result>
void __bind(Ctrl *ctrl,
    Result &out,
    Int2Type<5>)
{
    out = boost::bind(&FrxFunction::process, _1, _2, _3, _4, _5, ctrl);
}


template <class _FList>
struct Functions {
    typedef _FList FList;
    Functions(PluginScriptCtrl *master) : master(master) {}
    PluginScriptCtrl *master;
};

template <class Functions>
struct Accessor {
    template <int Index, class _Result>
    static void get(const Functions &f, _Result &out) {
        typedef typename Loki::TL::TypeAt<typename Functions::FList, Index>::Result FX;
        enum { NumArgs = FX::Function::arity };
        __bind<FX>(f.master, out, Int2Type<NumArgs>());
    }
};

} // namespace(s)
//-----------------------------------------------------------------------------
void PluginScriptCtrl::registerFunctions(sambag::lua::LuaStateRef luaState, bool isPublic)
{
    if (isPublic) {
        typedef Loki::TL::NoDuplicates<FrxPublicFunctionList>::Result Fz; // arf
        sambag::lua::registerFunctions<Fz, Accessor>(
            luaState.get(), Functions<Fz>(this), "frx"
        );
    } else {
        typedef Loki::TL::Append<FrxPublicFunctionList,
            FrxPrivateFunctionList>::Result FsUnion;
        typedef Loki::TL::NoDuplicates<FsUnion>::Result Fz; // arf
        sambag::lua::registerFunctions<Fz, Accessor>(
            luaState.get(), Functions<Fz>(this), "frx"
        );

    }
    lua_getglobal(luaState.get(), "frx");
    int index = lua_gettop(luaState.get());
    LuaFrxView::createAndPush(luaState.get(), editor);
    lua_setfield(luaState.get(), index, "view");
}
}} // namespace(s)
