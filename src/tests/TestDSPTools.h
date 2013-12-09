
#ifndef DSPTOOLS_TEST_H
#define DSPTOOLS_TEST_H

#include <cppunit/extensions/HelperMacros.h>

namespace tests {
//=============================================================================
class TestDSPTools : public CPPUNIT_NS::TestFixture {
//=============================================================================
	CPPUNIT_TEST_SUITE( TestDSPTools );
	CPPUNIT_TEST( testTimer );
    CPPUNIT_TEST( testIssue446 );
	CPPUNIT_TEST_SUITE_END();
private:
public:
	void setUp(){}
	void tearDown(){}
	void testTimer();
    void testIssue446();
};
} // namespace tests
#endif 
