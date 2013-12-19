/*
 * TestComponentFactory.hpp
 *
 *  Created on: Thu Dec 19 16:20:53 2013
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_TESTCOMPONENTFACTORY_H
#define SAMBAG_TESTCOMPONENTFACTORY_H

#include <cppunit/extensions/HelperMacros.h>

namespace tests {
//=============================================================================
class TestComponentFactory : public CPPUNIT_NS::TestFixture {
//=============================================================================
private:
	CPPUNIT_TEST_SUITE( TestComponentFactory );
	// CPPUNIT_TEST( XXX );
	CPPUNIT_TEST_SUITE_END();
public:
	//void XXX();
};

} // namespace

#endif /* SAMBAG_TESTCOMPONENTFACTORY_H */
