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
#include <string>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( tests::TestOne4All );

namespace tests {
//=============================================================================
void TestOne4All::testExtractVSTPluginFilename() {
//=============================================================================
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
} // namespace tests
