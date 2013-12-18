/*
 * TestModelFactory.hpp
 *
 *  Created on: Wed Dec 18 09:55:05 2013
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_TESTMODELFACTORY_H
#define SAMBAG_TESTMODELFACTORY_H

#include <cppunit/extensions/HelperMacros.h>

namespace tests {
//=============================================================================
class TestModelFactory : public CPPUNIT_NS::TestFixture {
//=============================================================================
private:
	CPPUNIT_TEST_SUITE( TestModelFactory );
	CPPUNIT_TEST( testCreateProducts );
    CPPUNIT_TEST( testArchiveRegister );
	CPPUNIT_TEST_SUITE_END();
public:
	void testCreateProducts();
    void testArchiveRegister();
};

} // namespace

#endif /* SAMBAG_TESTMODELFACTORY_H */
