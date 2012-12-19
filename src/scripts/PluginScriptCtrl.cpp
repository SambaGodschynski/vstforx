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
#include <gui/IFrxControl.hpp>
#include <sambag/lua/LuaSequence.hpp>

namespace frx { namespace scripts {
namespace {
	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////
	// Access
	typedef PluginScriptCtrl Ctrl;
	typedef Ctrl::LuaFrxComponent LuaFrxComponent;
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
	struct FrxSerializePlugin {
		typedef boost::function<std::string()> Function;
		static const char * name() { return "frxSerializePlugin"; }
		static std::string process(Ctrl *ctrl);
	};
	//-------------------------------------------------------------------------
	struct FrxDeserializePlugin {
		typedef boost::function<void(std::string)> Function;
		static const char * name() { return "frxDeserializePlugin"; }
		static void process(std::string, Ctrl *ctrl);
	};
	//-------------------------------------------------------------------------
	struct FrxGetComponents {
		typedef sambag::lua::LuaSequence<LuaFrxComponent> Components;
		typedef boost::function<Components()> Function;
		static const char * name() { return "frxGetComponents"; }
		static Components process(Ctrl *ctrl);
	};
	//-------------------------------------------------------------------------
	typedef LOKI_TYPELIST_11(FrxOpenPlugin,
		FrxClosePlugin,
		FrxOpenEditor,
		FrxCloseEditor,
		FrxWait,
		FrxSetEditorExitOnClose,
		FrxGetProcessors,
		FrxAddProcessor,
		FrxSerializePlugin,
		FrxDeserializePlugin,
		FrxGetComponents
	) FrxFunctionList;
///////////////////////////////////////////////////////////////////////////////
// FrxFunction impl.
#define FRX_START_SCRIPTCALL ctrl->startScriptCall(std::string(name()));
#define FRX_END_SCRIPTCALL ctrl->endScriptCall();
#define FRX_GET_PLUG frx::processing::VstForxPlug * plug = ctrl->getPlugin();
#define FRX_GET_EDITOR frx::gui::components::VstForxEditor * editor = ctrl->getEditor();
//-----------------------------------------------------------------------------
FrxGetComponents::Components FrxGetComponents::process(Ctrl *ctrl) {
	FRX_START_SCRIPTCALL
	FRX_GET_PLUG
	FRX_GET_EDITOR
	if (!editor->isOpen()) {
		return Components();
	}
	using namespace frx::gui::components; 
	using namespace sambag::disco::components;
	FrxCircuidViewPtr view = editor->getCircuidView();
	const FrxCircuidView::Components &comps = view->getContentPane()->getComponents();
	Components res;
	res.reserve(comps.size());
	BOOST_FOREACH(AComponentPtr c, comps) {
		FrxComponent::Ptr fc = 
			boost::shared_dynamic_cast<FrxComponent>(c);
		if (!fc) {
			continue;
		}
		res.push_back(ctrl->getLuaFrxComponent(fc));
	}
	FRX_END_SCRIPTCALL
	return res;
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
	FRX_END_SCRIPTCALL
	return std::string(res);
}
//-----------------------------------------------------------------------------
void FrxDeserializePlugin::process(std::string bytes, Ctrl *ctrl) {
	FRX_START_SCRIPTCALL
	FRX_GET_PLUG
	plug->setChunk((void*)bytes.c_str(), bytes.size());
	FRX_END_SCRIPTCALL
}
//-----------------------------------------------------------------------------
void FrxOpenPlugin::process(Ctrl *ctrl) {
	FRX_START_SCRIPTCALL
	FRX_GET_PLUG
	FRX_GET_EDITOR
	plug->open();
	boost::this_thread::sleep(boost::posix_time::milliseconds(500));
	FRX_END_SCRIPTCALL
}
//-----------------------------------------------------------------------------
void FrxClosePlugin::process(Ctrl *ctrl) {
	FRX_START_SCRIPTCALL
	FRX_GET_PLUG
	FRX_GET_EDITOR
	plug->close();
	boost::this_thread::sleep(boost::posix_time::milliseconds(500));
	FRX_END_SCRIPTCALL
}
//-----------------------------------------------------------------------------
void FrxOpenEditor::process(Ctrl *ctrl) {
	FRX_START_SCRIPTCALL
	FRX_GET_PLUG
	FRX_GET_EDITOR
	editor->open();
	boost::this_thread::sleep(boost::posix_time::milliseconds(500));
	FRX_END_SCRIPTCALL
}
//-----------------------------------------------------------------------------
void FrxCloseEditor::process(Ctrl *ctrl) {
	FRX_START_SCRIPTCALL
	FRX_GET_PLUG
	FRX_GET_EDITOR
	editor->close();
	boost::this_thread::sleep(boost::posix_time::milliseconds(500));
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
	/*win->setDefaultCloseOperation (
		val ? Window::EXIT_ON_CLOSE : Window::DISPOSE_ON_CLOSE
	);*/
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
	FRX_END_SCRIPTCALL
	return res;
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
PluginScriptCtrl::LuaFrxComponent 
PluginScriptCtrl::getLuaFrxComponent(FrxComponentPtr c)
{
	std::string key(sambag::com::toString(c.get()));
	componentMap[key] = c;
	return key;
}
//-----------------------------------------------------------------------------
PluginScriptCtrl::FrxComponentPtr 
PluginScriptCtrl::getFrxComponent(const LuaFrxComponent &c)
{
	ComponentMap::const_iterator it = componentMap.find(c);
	if (it==componentMap.end()) {
		return FrxComponentPtr();
	}
	return it->second;
}
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
	EventSender<ScriptEnded>::notifyListeners(
		this,
		ScriptEnded()
	);
}
//-----------------------------------------------------------------------------
void PluginScriptCtrl::startScriptCall(const std::string &fname) {
	/*if (fname!="") {
		sambag::com::log("call " + fname);
	}*/
	lastCall = fname;
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
