
#ifndef ONE4ALL_TEST_H
#define ONE4ALL_TEST_H

#include <cppunit/extensions/HelperMacros.h>

namespace tests {
//=============================================================================
class TestOne4All : public CPPUNIT_NS::TestFixture {
//=============================================================================
	CPPUNIT_TEST_SUITE( TestOne4All );
	CPPUNIT_TEST( testExtractVSTPluginFilename );
    CPPUNIT_TEST( testMapNumChannels2Xput );
	CPPUNIT_TEST_SUITE_END();
public:
	void testExtractVSTPluginFilename();
	void testResize();
    void testMapNumChannels2Xput();
};
} // namespace tests
#endif 
