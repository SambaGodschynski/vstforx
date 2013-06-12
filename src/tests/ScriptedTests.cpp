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

VstIntPtr testHostCallback(AEffect* effect, VstInt32 opcode,
	VstInt32 index, VstIntPtr value, void* ptr, float opt);

bool plugProcessing = false;

void fillBlock(float **data, int blockSize) {
	for (int i=0; i<blockSize; ++i) {
		data[0][i] = (float)rand() / (float)RAND_MAX;
		data[1][i] = (float)rand() / (float)RAND_MAX;
	}
}
void processPlugin(TestPlugin * plug) {
	static const int BS = 512;
	float **in = new float*[2];
	float **out = new float*[2];
	in[0] = new float[BS];
	in[1] = new float[BS];
	fillBlock(in, BS);
	out[0] = new float[BS];
	out[1] = new float[BS];
	plug->suspend();
	plug->setSampleRate(44100.);
	plug->setBlockSize(BS);
	plug->resume();
	while(plugProcessing) {
		if (plug->isOpen()) {
			fillBlock(in, BS);
			plug->process(in, out, BS);
			boost::this_thread::sleep(boost::posix_time::milliseconds(10));
		}
	}
	delete[] in[0];
	delete[] in[1];
	delete[] out[0];
	delete[] out[1];
	delete[] in;
	delete[] out;
}

//=============================================================================
//  Class ScriptedTests
//=============================================================================
//-----------------------------------------------------------------------------
ScriptedTests::ScriptedTests() {

}
//-----------------------------------------------------------------------------
void ScriptedTests::setUp() {
	namespace sce = sambag::com::events;
	plug = createPlug();
	scriptCtrl = new frx::scripts::PluginScriptCtrl();
	scriptCtrl->setPlugin(plug);
	scriptCtrl->sce::EventSender<frx::scripts::ScriptExeFailedEvent>::addEventListener(
		boost::bind(&ScriptedTests::onScriptExeFailed, this, _1, _2)
	);
	scriptCtrl->sce::EventSender<frx::scripts::ScriptEnded>::addEventListener(
		boost::bind(&ScriptedTests::onScriptEnd, this, _1, _2)
	);
	plugProcessing = true;
	processingThread = boost::thread(
		boost::bind(&processPlugin, plug)
	);
	failed = false;
}
//-----------------------------------------------------------------------------
void ScriptedTests::tearDown() {
	plugProcessing = false;
	processingThread.join();
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
		vpl.setMasterCallback((void*)&testHostCallback);
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
	
	scriptCtrl->appendJob( rm.getString("testScripts/testOpenClose.lua") );
	scriptCtrl->start();
	sambag::disco::components::Window::startMainLoop();
	scriptCtrl->join();
	CPPUNIT_ASSERT(!failed);
	
}
//-----------------------------------------------------------------------------
void ScriptedTests::testSerializing() {
	sambag::disco::IResourceManager &rm =
		sambag::disco::getResourceManager();
	
	scriptCtrl->appendJob( rm.getString("testScripts/testSerializing.lua") );
	scriptCtrl->start();
	sambag::disco::components::Window::startMainLoop();
	scriptCtrl->join();
	CPPUNIT_ASSERT(!failed);
}
//-----------------------------------------------------------------------------
void ScriptedTests::issue255() {
	sambag::disco::IResourceManager &rm =
		sambag::disco::getResourceManager();
	
	scriptCtrl->appendJob( rm.getString("testScripts/issue255.lua") );
	scriptCtrl->start();
	sambag::disco::components::Window::startMainLoop();
	scriptCtrl->join();
	CPPUNIT_ASSERT(!failed);
}
//-----------------------------------------------------------------------------
void ScriptedTests::issue265() {
	sambag::disco::IResourceManager &rm =
		sambag::disco::getResourceManager();
	
	scriptCtrl->appendJob( rm.getString("testScripts/issue265.lua") );
	scriptCtrl->start();
	sambag::disco::components::Window::startMainLoop();
	scriptCtrl->join();
	CPPUNIT_ASSERT(!failed);
}
//-----------------------------------------------------------------------------
void ScriptedTests::issue269() {
	sambag::disco::IResourceManager &rm =
		sambag::disco::getResourceManager();
	
	scriptCtrl->appendJob( rm.getString("testScripts/issue269.lua") );
	scriptCtrl->start();
	sambag::disco::components::Window::startMainLoop();
	scriptCtrl->join();
	CPPUNIT_ASSERT(!failed);
}
//-----------------------------------------------------------------------------
void ScriptedTests::issue272() {
	sambag::disco::IResourceManager &rm =
		sambag::disco::getResourceManager();
	
	scriptCtrl->appendJob( rm.getString("testScripts/issue272.lua") );
	scriptCtrl->start();
	sambag::disco::components::Window::startMainLoop();
	scriptCtrl->join();
	CPPUNIT_ASSERT(!failed);
}
//-----------------------------------------------------------------------------
void ScriptedTests::issue320() {
	sambag::disco::IResourceManager &rm =
		sambag::disco::getResourceManager();
	
	scriptCtrl->appendJob( rm.getString("testScripts/issue320.lua") );
	scriptCtrl->start();
	sambag::disco::components::Window::startMainLoop();
	scriptCtrl->join();
	CPPUNIT_ASSERT(!failed);
}
//-----------------------------------------------------------------------------
void ScriptedTests::issue322() {
	sambag::disco::IResourceManager &rm =
		sambag::disco::getResourceManager();
	
	scriptCtrl->appendJob( rm.getString("testScripts/issue322.lua") );
	scriptCtrl->start();
	sambag::disco::components::Window::startMainLoop();
	scriptCtrl->join();
	CPPUNIT_ASSERT(!failed);
}
///////////////////////////////////////////////////////////////////////////////
VstIntPtr testHostCallback(AEffect* effect, VstInt32 opcode,
 VstInt32 index, VstIntPtr value, void* ptr, float opt) 
{
	//std::cout<<"testHostCallback request("<<opcode<<")"<<std::endl;
	return 0;
}
} //namespace


