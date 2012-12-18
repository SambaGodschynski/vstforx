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
#include <gui/IFrxControl.hpp>
#include <sambag/lua/LuaSequence.hpp>

namespace frx { namespace scripts {
namespace {
	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////
	// Access
	typedef PluginScriptCtrl Ctrl;
	//-------------------------------------------------------------------------
	struct FrxOpenPlugin {
		typedef boost::function<void()> Function;
		static const char * name() { return "frxOpenPlugin"; }
		static void process(Ctrl *ctrl);
	};
	//-------------------------------------------------------------------------
	struct FrxClosePlugin {
		typedef boost::function<void()> Function;
		static const char * name() { return "frxClosePlugin"; }
		static void process(Ctrl *ctrl);
	};
	//-------------------------------------------------------------------------
	struct FrxOpenEditor {
		typedef boost::function<void()> Function;
		static const char * name() { return "frxOpenEditor"; }
		static void process(Ctrl *ctrl);
	};
	//-------------------------------------------------------------------------
	struct FrxCloseEditor {
		typedef boost::function<void()> Function;
		static const char * name() { return "frxCloseEditor"; }
		static void process(Ctrl *ctrl);
	};
	//-------------------------------------------------------------------------
	struct FrxWait {
		typedef boost::function<void(int)> Function;
		static const char * name() { return "frxWait"; }
		static void process(int sec, Ctrl *ctrl);
	};
	//-------------------------------------------------------------------------
	struct FrxSetEditorExitOnClose {
		typedef boost::function<void(bool)> Function;
		static const char * name() { return "frxSetEditorExitOnClose"; }
		static void process(bool val, Ctrl *ctrl);
	};
	//-------------------------------------------------------------------------
	struct FrxGetProcessors {
		typedef sambag::lua::LuaSequence<std::string> Strings;
		typedef boost::function<Strings()> Function;
		static const char * name() { return "frxGetProcessors"; }
		static Strings process(Ctrl *ctrl);
	};
	//-------------------------------------------------------------------------
	struct FrxAddProcessor {
		typedef boost::function<void(std::string)> Function;
		static const char * name() { return "frxAddProcessor"; }
		static void process(std::string, Ctrl *ctrl);
	};
	//-------------------------------------------------------------------------
	typedef LOKI_TYPELIST_8(FrxOpenPlugin,
		FrxClosePlugin,
		FrxOpenEditor,
		FrxCloseEditor,
		FrxWait,
		FrxSetEditorExitOnClose,
		FrxGetProcessors,
		FrxAddProcessor
	) FrxFunctionList;
///////////////////////////////////////////////////////////////////////////////
// FrxFunction impl.
#define FRX_START_SCRIPTCALL ctrl->startScriptCall(std::string(name()));
#define FRX_END_SCRIPTCALL ctrl->endScriptCall();
#define FRX_GET_PLUG frx::processing::VstForxPlug * plug = ctrl->getPlugin();
#define FRX_GET_EDITOR frx::gui::components::VstForxEditor * editor = ctrl->getEditor();
//-----------------------------------------------------------------------------
void FrxOpenPlugin::process(Ctrl *ctrl) {
	FRX_START_SCRIPTCALL
	FRX_GET_PLUG
	FRX_GET_EDITOR
		plug->open();
	FRX_END_SCRIPTCALL
}
//-----------------------------------------------------------------------------
void FrxClosePlugin::process(Ctrl *ctrl) {
	FRX_START_SCRIPTCALL
	FRX_GET_PLUG
	FRX_GET_EDITOR
		plug->close();
	FRX_END_SCRIPTCALL
}
//-----------------------------------------------------------------------------
void FrxOpenEditor::process(Ctrl *ctrl) {
	FRX_START_SCRIPTCALL
	FRX_GET_PLUG
	FRX_GET_EDITOR
		editor->open();
	FRX_END_SCRIPTCALL
}
//-----------------------------------------------------------------------------
void FrxCloseEditor::process(Ctrl *ctrl) {
	FRX_START_SCRIPTCALL
	FRX_GET_PLUG
	FRX_GET_EDITOR
		editor->close();
	FRX_END_SCRIPTCALL
}
//-----------------------------------------------------------------------------
void FrxWait::process(int millis, Ctrl *ctrl) {
	FRX_START_SCRIPTCALL
	FRX_GET_PLUG
	FRX_GET_EDITOR
		boost::this_thread::sleep(boost::posix_time::milliseconds(millis));
	FRX_END_SCRIPTCALL
}
//-----------------------------------------------------------------------------
void FrxSetEditorExitOnClose::process(bool val, Ctrl *ctrl) {
	FRX_START_SCRIPTCALL
	FRX_GET_PLUG
	FRX_GET_EDITOR
		using namespace sambag::disco::components;
		Window::Ptr win = editor->getParentWindow();
		if (!win) {
			return;
		}
		win->setDefaultCloseOperation (
			val ? Window::EXIT_ON_CLOSE : Window::DISPOSE_ON_CLOSE
		);
	FRX_END_SCRIPTCALL
}
//-----------------------------------------------------------------------------
FrxGetProcessors::Strings FrxGetProcessors::process(Ctrl *ctrl) {
	FRX_START_SCRIPTCALL
	FRX_GET_PLUG
	FRX_GET_EDITOR
		using namespace frx::gui::components;
		std::list<std::string> processors;
		getComponentFactory(editor->getCircuidView()).getProcessorNames(processors);
		Strings res;
		BOOST_FOREACH(const std::string &str, processors) {
			res.push_back(str);
		}
		return res;
	FRX_END_SCRIPTCALL
}
//-----------------------------------------------------------------------------
void FrxAddProcessor::process(std::string _name, Ctrl *ctrl) {
	FRX_START_SCRIPTCALL
	FRX_GET_PLUG
	FRX_GET_EDITOR
		using namespace frx::gui;
		using namespace frx::gui::components;
		FrxCircuidViewPtr view = editor->getCircuidView();
		IFrxComponentFactory &fac = getComponentFactory(view);
		IFrxControl &frxctrl = getFrxControl(view);
		FrxProcessorNodePtr res = fac.getProcessorCreator(_name)(view);
		if (res) {
			frxctrl.addProcessorToView(view, res);
		}
	FRX_END_SCRIPTCALL
}
} // namespace
//=============================================================================
//  Class PluginScriptCtrl
//=============================================================================
//-----------------------------------------------------------------------------
sambag::disco::components::WindowPtr PluginScriptCtrl::getEditorWindow() const {
	if (!editor) {
		return sambag::disco::components::WindowPtr();
	}
	return editor->getParentWindow();
}
//-----------------------------------------------------------------------------
void PluginScriptCtrl::setPlugin(frx::processing::VstForxPlug *plug) {
	if (!plug) {
		return;
	}
	this->plug = plug;
	using namespace sambag::disco::components;
	editor =
		dynamic_cast<frx::gui::components::VstForxEditor*>(plug->getEditor());
	if (!editor) {
		SAMBAG_THROW(
			sambag::com::exceptions::IllegalStateException,
			"VstForxEditor = NULL"
		);
	}
}
//-----------------------------------------------------------------------------
void PluginScriptCtrl::addScript(const std::string &str) {
	scripts.push_back(str);
}
//-----------------------------------------------------------------------------
void PluginScriptCtrl::start() {
	using namespace sambag::lua;
	luaState = createLuaStateRef();
	registerFunctions(luaState);
	thread = boost::thread(
		boost::bind(&PluginScriptCtrl::runThread, this)
	);
}
//-----------------------------------------------------------------------------
void PluginScriptCtrl::join() {
	thread.join();
}
//-----------------------------------------------------------------------------
void PluginScriptCtrl::runThread() {
	using namespace sambag::lua;
	sambag::com::log("start PluginScript thread.");
	BOOST_FOREACH(const std::string &script, scripts) {
		try {
			executeString(luaState.get(), script);
		} catch(const ExecutionFailed &ex) {
			sambag::com::log("executation failed: " + ex.errMsg);
			EventSender<ScriptExeFailedEvent>::notifyListeners(
				this,
				ScriptExeFailedEvent()
			);
			break;
		}
	}
	sambag::com::log("end PluginScript thread.");
}
//-----------------------------------------------------------------------------
void PluginScriptCtrl::startScriptCall(const std::string &fname) {
	if (fname!="") {
		sambag::com::log("call " + fname);
	}
}
//-----------------------------------------------------------------------------
void PluginScriptCtrl::endScriptCall() {
	//boost::this_thread::sleep(boost::posix_time::milliseconds(100));
}
///////////////////////////////////////////////////////////////////////////////
// register function approach
namespace {
template <int Val>
struct Int2Type {
	enum { Value = Val };
};
template <class FrxFunction> 
void registerFunctionImpl(sambag::lua::LuaStateRef luaState, Ctrl *ctrl, Int2Type<0>)
{
	sambag::lua::registerFunction<FrxFunction>(
		luaState.get(),
		boost::bind(&FrxFunction::process, ctrl)
	);
}
template <class FrxFunction> 
void registerFunctionImpl(sambag::lua::LuaStateRef luaState, Ctrl *ctrl, Int2Type<1>)
{
	sambag::lua::registerFunction<FrxFunction>(
		luaState.get(),
		boost::bind(&FrxFunction::process, _1, ctrl)
	);
}
template <class FrxFunction> 
void registerFunctionImpl(sambag::lua::LuaStateRef luaState, Ctrl *ctrl, Int2Type<2>)
{
	sambag::lua::registerFunction<FrxFunction>(
		luaState.get(),
		boost::bind(&FrxFunction::process, _1, _2, ctrl)
	);
}
template <class FrxFunction> 
void registerFunctionImpl(sambag::lua::LuaStateRef luaState, Ctrl *ctrl, Int2Type<3>)
{
	sambag::lua::registerFunction<FrxFunction>(
		luaState.get(),
		boost::bind(&FrxFunction::process, _1, _2, _3, ctrl)
	);
}
template <class FrxFunction> 
void registerFunctionImpl(sambag::lua::LuaStateRef luaState, Ctrl *ctrl, Int2Type<4>)
{
	sambag::lua::registerFunction<FrxFunction>(
		luaState.get(),
		boost::bind(&FrxFunction::process, _1, _2, _3, _4, ctrl)
	);
}
template <class FrxFunction> 
void registerFunctionImpl(sambag::lua::LuaStateRef luaState, Ctrl *ctrl, Int2Type<5>)
{
	sambag::lua::registerFunction<FrxFunction>(
		luaState.get(),
		boost::bind(&FrxFunction::process, _1, _2, _3, _4, _5, ctrl)
	);
}
template <class FuncList>
void _registerFunctions(sambag::lua::LuaStateRef luaState, Ctrl *ctrl) 
{
	typedef typename FuncList::Head FrxFunction;
	enum { NumArgs = typename FrxFunction::Function::arity };
	registerFunctionImpl<FrxFunction>(luaState, ctrl, Int2Type<NumArgs>());
	// register next
	_registerFunctions<FuncList::Tail>(luaState, ctrl);
}
template <>
void _registerFunctions<Loki::NullType>
(sambag::lua::LuaStateRef luaState,  Ctrl *ctrl) {}
} // namespace(s)
//-----------------------------------------------------------------------------
void PluginScriptCtrl::registerFunctions(sambag::lua::LuaStateRef luaState) {
	using namespace sambag::lua;
	_registerFunctions<FrxFunctionList>(luaState, this);
}
}} // namespace(s)
