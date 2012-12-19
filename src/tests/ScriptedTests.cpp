/*
 * ScriptedTests.cpp
 *
 *  Created on: Sat Dec 15 23:04:17 2012
 *      Author: Johannes Unger
 */

#include "ScriptedTests.hpp"
#include <cppunit/config/SourcePrefix.h>
#include <sambag/disco/components/Window.hpp>
#include <gui/components/VstForxEditor.hpp>
#include <sambag/disco/IResourceManager.hpp>
#include <sambag/disco/components/WindowToolkit.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( tests::ScriptedTests );

namespace tests {

int testHostCallback(AEffect* effect, VstInt32 opcode, 
	VstInt32 index, VstIntPtr value, void* ptr, float opt);


//=============================================================================
//  Class ScriptedTests
//=============================================================================
//-----------------------------------------------------------------------------
ScriptedTests::ScriptedTests() {

}
//-----------------------------------------------------------------------------
void ScriptedTests::setUp() {
	plug = createPlug();
	scriptCtrl = new frx::scripts::PluginScriptCtrl();
	scriptCtrl->setPlugin(plug);
	scriptCtrl->EventSender<frx::scripts::ScriptExeFailedEvent>::addEventListener(
		boost::bind(&ScriptedTests::onScriptExeFailed, this, _1, _2)
	);
	scriptCtrl->EventSender<frx::scripts::ScriptEnded>::addEventListener(
		boost::bind(&ScriptedTests::onScriptEnd, this, _1, _2)
	);
	failed = false;
}
//-----------------------------------------------------------------------------
void ScriptedTests::tearDown() {
	delete scriptCtrl;
	delete plug;
}
//-----------------------------------------------------------------------------
TestPlugin * ScriptedTests::createPlug() {
	// create plugin
	try {
		TestPlugin *pl = new TestPlugin(&testHostCallback);
		frx::processing::VstForxPlug &vpl = *pl;
		vpl.setEffectPtr(pl);
		vpl.setMasterCallback(&testHostCallback);
		return pl;
	} catch(...) {
		return NULL;
	}
	return NULL;
}
//-----------------------------------------------------------------------------
void ScriptedTests::
onScriptExeFailed(void *src, const frx::scripts::ScriptExeFailedEvent &ev) {
	failed = true;
	sambag::disco::components::getWindowToolkit()->quit();
}
//-----------------------------------------------------------------------------
void ScriptedTests::
onScriptEnd(void *src, const frx::scripts::ScriptEnded &ev) {
	frx::gui::components::VstForxEditor *ed =
		scriptCtrl->getEditor();
	if (ed->isOpen()) {
		ed->close();
	}
	sambag::disco::components::getWindowToolkit()->quit();
}
//-----------------------------------------------------------------------------
void ScriptedTests::testOpenClose() {
	sambag::disco::IResourceManager &rm =
		sambag::disco::getResourceManager();
	
	scriptCtrl->addScript( rm.getString("testScripts/testOpenClose.lua") );
	scriptCtrl->start();
	sambag::disco::components::Window::startMainLoop();
	scriptCtrl->join();
	CPPUNIT_ASSERT(!failed);
	
}
//-----------------------------------------------------------------------------
void ScriptedTests::testSerializing() {
	sambag::disco::IResourceManager &rm =
		sambag::disco::getResourceManager();
	
	scriptCtrl->addScript( rm.getString("testScripts/testSerializing.lua") );
	scriptCtrl->start();
	sambag::disco::components::Window::startMainLoop();
	scriptCtrl->join();
	CPPUNIT_ASSERT(!failed);
	
}
///////////////////////////////////////////////////////////////////////////////
int testHostCallback(AEffect* effect, VstInt32 opcode, 
 VstInt32 index, VstIntPtr value, void* ptr, float opt) 
{
	std::cout<<"testHostCallback request("<<opcode<<")"<<std::endl;
	return 0;
}
} //namespace


