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
	scriptCtrl.setPlugin(plug);
}
//-----------------------------------------------------------------------------
void ScriptedTests::tearDown() {
	// wait for threads
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
void ScriptedTests::scriptTests() {
	sambag::disco::IResourceManager &rm =
		sambag::disco::getResourceManager();
	
	scriptCtrl.addScript( rm.getString("testScripts/testOpenClose.lua") );
	scriptCtrl.start();
	sambag::disco::components::Window::startMainLoop();
	scriptCtrl.join();
	
}
///////////////////////////////////////////////////////////////////////////////
int testHostCallback(AEffect* effect, VstInt32 opcode, 
 VstInt32 index, VstIntPtr value, void* ptr, float opt) 
{
	std::cout<<"testHostCallback request("<<opcode<<")"<<std::endl;
	return 0;
}
} //namespace


