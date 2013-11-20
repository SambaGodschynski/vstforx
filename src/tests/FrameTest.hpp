
#ifndef FRAME_TEST_H
#define FRAME_TEST_H

#include <cppunit/extensions/HelperMacros.h>
#include "com/PPIError.h"
#include "processing/processing.h"

namespace tests {
//=============================================================================
class FrameTest : public CPPUNIT_NS::TestFixture {
//=============================================================================
	CPPUNIT_TEST_SUITE( FrameTest );
	CPPUNIT_TEST( testConstructor );
	CPPUNIT_TEST( testResize );
	CPPUNIT_TEST_SUITE_END();
private:
public:
	FrameTest();
	~FrameTest();
	void setUp(){}
	void tearDown(){}

	void testConstructor();
	void testResize();
};
} // namespace tests
#endif 
