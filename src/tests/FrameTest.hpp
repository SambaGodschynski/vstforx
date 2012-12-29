
#ifndef FRAME_TEST_H
#define FRAME_TEST_H

#include <cppunit/extensions/HelperMacros.h>
#include "audioeffectx.h"
#include "com/PPIError.h"
#include "processing/processing.h"
#include "processing/Graph.h"
#include "DummyFX.h"

namespace tests {
//=============================================================================
class FrameTest : public CPPUNIT_NS::TestFixture {
//=============================================================================
	CPPUNIT_TEST_SUITE( FrameTest );
	CPPUNIT_TEST( testConstructor );
	CPPUNIT_TEST( testResize );
	CPPUNIT_TEST_SUITE_END();
private:
	processing::DummyFX::Ptr dummyFX;
	processing::Graph::Ptr createGraph( int blockSize, float samplerate );
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
