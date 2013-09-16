/*
 * TestInterprocessStream.hpp
 *
 *  Created on: Thu Sep 12 12:19:08 2013
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_TESTINTERPROCESSSTREAM_H
#define SAMBAG_TESTINTERPROCESSSTREAM_H

#include <cppunit/extensions/HelperMacros.h>

namespace tests {
//=============================================================================
class TestInterprocessStream : public CPPUNIT_NS::TestFixture {
//=============================================================================
private:
	CPPUNIT_TEST_SUITE( TestInterprocessStream );
	CPPUNIT_TEST( testStreamConstruction );
    CPPUNIT_TEST( testReadWrite );
	CPPUNIT_TEST_SUITE_END();
public:
	void testStreamConstruction();
    void testReadWrite();
};

} // namespace

#endif /* SAMBAG_TESTINTERPROCESSSTREAM_H */
