/*
 * TestIPSession.hpp
 *
 *  Created on: Thu Nov 28 11:21:11 2013
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_TESTIPSESSION_H
#define SAMBAG_TESTIPSESSION_H

#include <cppunit/extensions/HelperMacros.h>

namespace tests {
//=============================================================================
class TestIPSession : public CPPUNIT_NS::TestFixture {
//=============================================================================
private:
	CPPUNIT_TEST_SUITE( TestIPSession );
	CPPUNIT_TEST( testSession );
    CPPUNIT_TEST( testNoHost );
    CPPUNIT_TEST( testHostLost );
    CPPUNIT_TEST( testFailures );
    CPPUNIT_TEST( testHelper );
	CPPUNIT_TEST_SUITE_END();
public:
	void testSession();
    void testNoHost();
    void testHostLost();
    void testFailures();
    void testHelper();
};

} // namespace

#endif /* SAMBAG_TESTIPSESSION_H */
