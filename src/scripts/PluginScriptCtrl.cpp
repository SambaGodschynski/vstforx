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

namespace frx { namespace scripts {
namespace {
	struct FrxOpenPlugin_Tag {
		typedef boost::function<void()> Function;
		static const char * name() { return "frxOpenPlugin"; }
	};
	struct FrxClosePlugin_Tag {
		typedef boost::function<void()> Function;
		static const char * name() { return "frxClosePlugin"; }
	};
	struct FrxOpenEditor_Tag {
		typedef boost::function<void()> Function;
		static const char * name() { return "frxOpenEditor"; }
	};
	struct FrxCloseEditor_Tag {
		typedef boost::function<void()> Function;
		static const char * name() { return "frxCloseEditor"; }
	};
	struct FrxWait_Tag {
		typedef boost::function<void(int)> Function;
		static const char * name() { return "frxWait"; }
	};
	struct FrxAssert_Tag {
		typedef boost::function<void(bool)> Function;
		static const char * name() { return "frxAssert"; }
	};
	struct FrxAssertMsg_Tag {
		typedef boost::function<void(bool, std::string)> Function;
		static const char * name() { return "frxAssertMsg"; }
	};
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
//#define FRX_START_SCRIPTCALL startScriptCall();
//#define FRX_END_SCRIPTCALL endScriptCall();
#define FRX_START_SCRIPTCALL startScriptCall(__FUNCTION__);
#define FRX_END_SCRIPTCALL endScriptCall();
//-----------------------------------------------------------------------------
void PluginScriptCtrl::frxOpenPlugin() {
	FRX_START_SCRIPTCALL
	plug->open();
	FRX_END_SCRIPTCALL
}
//-----------------------------------------------------------------------------
void PluginScriptCtrl::frxClosePlugin() {
	FRX_START_SCRIPTCALL
	plug->close();
	FRX_END_SCRIPTCALL
}
//-----------------------------------------------------------------------------
void PluginScriptCtrl::frxOpenEditor() {
	FRX_START_SCRIPTCALL
	editor->open();
	editor->getParentWindow()->setWindowLocation(sambag::disco::Point2D(100,100));
	EventSender<OnEditorOpening>::notifyListeners(this, OnEditorOpening());
	FRX_END_SCRIPTCALL
}
//-----------------------------------------------------------------------------
void PluginScriptCtrl::frxCloseEditor() {
	FRX_START_SCRIPTCALL
	editor->close();
	EventSender<OnEditorClosing>::notifyListeners(this, OnEditorClosing());
	FRX_END_SCRIPTCALL
}
//-----------------------------------------------------------------------------
void PluginScriptCtrl::frxWait(int sec) {
	FRX_START_SCRIPTCALL
	boost::this_thread::sleep(boost::posix_time::seconds(sec));
	FRX_END_SCRIPTCALL
}
//-----------------------------------------------------------------------------
void PluginScriptCtrl::frxAssert(bool val) {
	FRX_START_SCRIPTCALL
	FRX_END_SCRIPTCALL
}
//-----------------------------------------------------------------------------
void PluginScriptCtrl::frxAssertMsg(bool val, std::string msg) {
	FRX_START_SCRIPTCALL
	FRX_END_SCRIPTCALL
}
//-----------------------------------------------------------------------------
void PluginScriptCtrl::registerFunctions(sambag::lua::LuaStateRef luaState) {
	using namespace sambag::lua;
	registerFunction<FrxOpenPlugin_Tag>(
		luaState.get(),
		boost::bind(&PluginScriptCtrl::frxOpenPlugin, this)
	);
	registerFunction<FrxClosePlugin_Tag>(
		luaState.get(),
		boost::bind(&PluginScriptCtrl::frxClosePlugin, this)
	);
	registerFunction<FrxOpenEditor_Tag>(
		luaState.get(),
		boost::bind(&PluginScriptCtrl::frxOpenEditor, this)
	);
	registerFunction<FrxCloseEditor_Tag>(
		luaState.get(),
		boost::bind(&PluginScriptCtrl::frxCloseEditor, this)
	);
	registerFunction<FrxWait_Tag>(
		luaState.get(),
		boost::bind(&PluginScriptCtrl::frxWait, this, _1)
	);
	registerFunction<FrxAssert_Tag>(
		luaState.get(),
		boost::bind(&PluginScriptCtrl::frxAssert, this, _1)
	);
	registerFunction<FrxAssertMsg_Tag>(
		luaState.get(),
		boost::bind(&PluginScriptCtrl::frxAssertMsg, this, _1, _2)
	);
}
}} // namespace(s)
