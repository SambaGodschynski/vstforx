/*
 * TestViewModelMap.hpp
 *
 *  Created on: Wed Oct 10 12:20:59 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_TESTMODELMAP_H
#define SAMBAG_TESTMODELMAP_H

#include <cppunit/extensions/HelperMacros.h>

namespace tests {
//=============================================================================
class TestViewModelMap : public CPPUNIT_NS::TestFixture {
//=============================================================================
private:
	CPPUNIT_TEST_SUITE( TestViewModelMap );
	CPPUNIT_TEST( testCreating );
	CPPUNIT_TEST( testRegistering );
	CPPUNIT_TEST( testHibernate );
	CPPUNIT_TEST( testSerializing );
	CPPUNIT_TEST_SUITE_END();
public:
	void testCreating();
	void testRegistering();
	// test serializing, closing, deserializing
	void testHibernate();
	void testSerializing();
};

} // namespace

#endif /* SAMBAG_TESTMODELMAP_H */
