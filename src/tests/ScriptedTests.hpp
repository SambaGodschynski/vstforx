/*
 * ScriptedTests.hpp
 *
 *  Created on: Sat Dec 15 23:04:17 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_SCRIPTEDTESTS_H
#define SAMBAG_SCRIPTEDTESTS_H

#include <cppunit/extensions/HelperMacros.h>
#include <processing/VstForxPlug.hpp>
#include <sambag/dsp/VST2xPluginWrapper.hpp>
#include <string>
#include <com/Settings.h>
#include <exception>
#include <aeffect.h>
#include <boost/tuple/tuple.hpp>
#include <boost/thread.hpp>
#include <scripts/PluginScriptCtrl.hpp>
#include <boost/thread.hpp>
#include <sambag/com/UnitEx.hpp>

// settingup plugin
typedef sambag::dsp::vst::VST2xPluginWrapper<
	frx::processing::VstForxPlug, // Processor
	'frxr', // uid
	sambag::dsp::StdPluginTraits<
		2,2,false,::com::Settings::PROGRAM_PARAMETER
	>,
	frx::gui::components::CreateVstForxEditor
> TestPlugin;

namespace tests {
//=============================================================================
class ScriptedTests : public CPPUNIT_NS::TestFixture {
//=============================================================================
private:
	CPPUNIT_TEST_SUITE( ScriptedTests );
//    CPPUNIT_TEST_KNOWN_ISSUE( frxLuaObjects );
//    CPPUNIT_TEST_KNOWN_ISSUE( issue163 );
//	CPPUNIT_TEST_KNOWN_ISSUE( issue320 );
//	CPPUNIT_TEST_KNOWN_ISSUE( issue322 );
//	CPPUNIT_TEST_KNOWN_ISSUE( issue272 );
//	CPPUNIT_TEST_KNOWN_ISSUE( issue255 );
//	CPPUNIT_TEST_KNOWN_ISSUE( issue265 );
//	CPPUNIT_TEST_KNOWN_ISSUE( issue269 );
//	CPPUNIT_TEST_KNOWN_ISSUE( testOpenClose );
// 	CPPUNIT_TEST_KNOWN_ISSUE( testSerializing );
    CPPUNIT_TEST_KNOWN_ISSUE(legacySerialization);
	CPPUNIT_TEST_SUITE_END();
	TestPlugin * plug;
	TestPlugin * createPlug();
	frx::scripts::PluginScriptCtrl *scriptCtrl;
	bool failed;
	void onScriptExeFailed(void *src, const frx::scripts::ScriptExeFailedEvent &ev);
	void onScriptEnd(void *src, const frx::scripts::ScriptEnded &ev);
	boost::thread processingThread;
public:
	ScriptedTests();
	virtual void setUp();
	virtual void tearDown();
	void testOpenClose();
	void testSerializing();
    void issue163();
	void issue255();
	void issue265();
	void issue269();
	void issue272();
	void issue320();
	void issue322();
	void issue324();
    void frxLuaObjects();
    void legacySerialization();
};

} // namespace

#endif /* SAMBAG_SCRIPTEDTESTS_H */
