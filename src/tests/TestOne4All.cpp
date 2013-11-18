/*
 * ===========================================================================================================
 * FrameTest.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */

#include <cppunit/config/SourcePrefix.h>
#include "TestOne4All.hpp"
#include "com/one4All.h"
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>
#include <boost/tuple/tuple_io.hpp>
#include <string>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( tests::TestOne4All );

namespace tests {
//=============================================================================
void TestOne4All::testExtractVSTPluginFilename() {
//=============================================================================
//-----------------------------------------------------------------------------
	// instr, outstr, outint
	boost::tuple<std::string, std::string, int> in[] = {
		boost::make_tuple("abc", "abc", 0),
		boost::make_tuple("abc@123", "abc", 123),
		boost::make_tuple("abc@ 123", "abc@ 123", 0),
		boost::make_tuple("abc@ 123abc", "abc@ 123abc", 0),
		boost::make_tuple("abc@123abc@123", "abc@123abc", 123),
		boost::make_tuple("[]<>^´.,°  |`'#~{}@", "[]<>^´.,°  |`'#~{}@", 0),
		boost::make_tuple("[]<>^´.,°  |`'#~{}@@123", "[]<>^´.,°  |`'#~{}@", 123),
		boost::make_tuple("c:\\Eigene Dateien\\VSTPlugins(x86)\\schnulli.dll", 
		 "c:\\Eigene Dateien\\VSTPlugins(x86)\\schnulli.dll", 
		 0),
		boost::make_tuple("c:\\Eigene Dateien\\VSTPlugins(x86)\\schnulli.dll@2333", 
		 "c:\\Eigene Dateien\\VSTPlugins(x86)\\schnulli.dll",
		 2333),
		boost::make_tuple("c:\\Eigene Dateien\\VSTPlugins(x86)\\schnulli@home.dll@2333", 
		 "c:\\Eigene Dateien\\VSTPlugins(x86)\\schnulli@home.dll",
		 2333)
	};
	const size_t NUM = sizeof(in)/sizeof(in[0]);

	enum {InStr, ExpStr, ExpInt};

	for (size_t i=0; i<NUM; ++i) {
		std::pair<std::string, int> res;
		res = com::extractVSTPluginFilename(boost::get<InStr>(in[i]));
		CPPUNIT_ASSERT_EQUAL(boost::get<ExpStr>(in[i]), res.first);
		CPPUNIT_ASSERT_EQUAL(boost::get<ExpInt>(in[i]), res.second);
	}
}
//-----------------------------------------------------------------------------
void TestOne4All::testMapNumChannels2Xput() {
    CPPUNIT_ASSERT_EQUAL((size_t)0, com::numChannels2Xputs(0));
    CPPUNIT_ASSERT_EQUAL((size_t)1, com::numChannels2Xputs(1));
    CPPUNIT_ASSERT_EQUAL((size_t)1, com::numChannels2Xputs(2));
    CPPUNIT_ASSERT_EQUAL((size_t)2, com::numChannels2Xputs(3));
    CPPUNIT_ASSERT_EQUAL((size_t)2, com::numChannels2Xputs(4));
    CPPUNIT_ASSERT_EQUAL((size_t)3, com::numChannels2Xputs(5));
    CPPUNIT_ASSERT_EQUAL((size_t)3, com::numChannels2Xputs(6));
    CPPUNIT_ASSERT_EQUAL((size_t)4, com::numChannels2Xputs(7));
    CPPUNIT_ASSERT_EQUAL((size_t)4, com::numChannels2Xputs(8));
    CPPUNIT_ASSERT_EQUAL((size_t)5, com::numChannels2Xputs(9));
    CPPUNIT_ASSERT_EQUAL((size_t)5, com::numChannels2Xputs(10));
}
//-----------------------------------------------------------------------------
void TestOne4All::testProcessorId() {
    using namespace com;
    CPPUNIT_ASSERT_EQUAL(
        FRX_NULL_PROCESSOR,
        extractProcessorDescriptor("")
    );
    CPPUNIT_ASSERT_EQUAL(
        FRX_NULL_PROCESSOR,
        extractProcessorDescriptor("no.none")
    );
    CPPUNIT_ASSERT_EQUAL(
        ProcessorDescriptor("vst2x", "DelayX", -1, -1),
        extractProcessorDescriptor("frx.processing.vst2x.DelayX")
    );
    CPPUNIT_ASSERT_EQUAL(
        ProcessorDescriptor("vst2x", "FrxTestplugin", 2, 3),
        extractProcessorDescriptor("frx.processing.vst2x.FrxTestplugin(2,3)")
    );
    CPPUNIT_ASSERT_EQUAL(
        ProcessorDescriptor("internal", "FrxADSR", -1, -1),
        extractProcessorDescriptor("frx.processing.internal.FrxADSR")
    );
   CPPUNIT_ASSERT_EQUAL(
        FRX_NULL_PROCESSOR,
        extractProcessorDescriptor("frx.processing.vst2x.FrxTestplugin(2)")
    );
   CPPUNIT_ASSERT_EQUAL(
        FRX_NULL_PROCESSOR,
        extractProcessorDescriptor("frx.processing.vst2x(2)")
    );
   CPPUNIT_ASSERT_EQUAL(
        FRX_NULL_PROCESSOR,
        extractProcessorDescriptor("vst2x(2)")
    );
    CPPUNIT_ASSERT_EQUAL(
        ProcessorDescriptor("vst2x", "FrxTestplugin", 2, 3),
        extractProcessorDescriptor("frx.processing.vst2x.FrxTestplugin( 2 , 3 )")
    );
}
} // namespace tests
