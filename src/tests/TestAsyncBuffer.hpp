/*
 * TestAsyncBuffer.hpp
 *
 *  Created on: Mon Sep 23 10:44:11 2013
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_TESTASYNCBUFFER_H
#define SAMBAG_TESTASYNCBUFFER_H

#include <cppunit/extensions/HelperMacros.h>

namespace tests {
//=============================================================================
class TestAsyncBuffer : public CPPUNIT_NS::TestFixture {
//=============================================================================
private:
	CPPUNIT_TEST_SUITE( TestAsyncBuffer );
	CPPUNIT_TEST( testAsyncBufferAlloc );
    CPPUNIT_TEST( testAsyncBufferCyclicWriting );
    CPPUNIT_TEST( testAsyncBufferCyclicWriting2 );
    CPPUNIT_TEST( testReading );
	CPPUNIT_TEST_SUITE_END();
public:
    void testAsyncBufferAlloc();
	void testAsyncBufferCyclicWriting();
    void testAsyncBufferCyclicWriting2();
    void testReading();
};

} // namespace

#endif /* SAMBAG_TESTASYNCBUFFER_H */
