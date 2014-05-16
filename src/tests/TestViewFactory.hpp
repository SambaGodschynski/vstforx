/*
 * TestViewFactory.hpp
 *
 *  Created on: Wed Dec 18 09:57:35 2013
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_TESTVIEWFACTORY_H
#define SAMBAG_TESTVIEWFACTORY_H

#include <cppunit/extensions/HelperMacros.h>

namespace tests {
//=============================================================================
class TestViewFactory : public CPPUNIT_NS::TestFixture {
//=============================================================================
private:
	CPPUNIT_TEST_SUITE( TestViewFactory );
	CPPUNIT_TEST( testCreateProducts );
    CPPUNIT_TEST( testArchiveRegister );
    CPPUNIT_TEST( testGetRegisteredIds );
	CPPUNIT_TEST_SUITE_END();
public:
	void testCreateProducts();
    void testArchiveRegister();
    void testGetRegisteredIds();
};

} // namespace

#endif /* SAMBAG_TESTVIEWFACTORY_H */
