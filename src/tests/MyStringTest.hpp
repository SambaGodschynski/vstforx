
#ifndef MYSTRING_TEST_H
#define MYSTRING_TEST_H

#include <cppunit/extensions/HelperMacros.h>

namespace tests {
//=============================================================================
class MyStringTest : public CPPUNIT_NS::TestFixture {
//=============================================================================
	CPPUNIT_TEST_SUITE( MyStringTest );
	CPPUNIT_TEST( testConstructor );
	CPPUNIT_TEST( testMultiplier );
	CPPUNIT_TEST( testShortenCenter );
	CPPUNIT_TEST( testShortenLeft );
	CPPUNIT_TEST( testShortenRight );
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp(){}
	void tearDown(){}
	void testConstructor();
	void testMultiplier();
	void testShortenCenter();
	void testShortenLeft();
	void testShortenRight();
};
} // namespace tests
#endif 
