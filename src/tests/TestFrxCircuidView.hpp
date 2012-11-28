/*
 * TestFrxCircuidView.hpp
 *
 *  Created on: Thu Aug 23 12:20:07 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_TESTFRXCIRCUIDVIEW_H
#define SAMBAG_TESTFRXCIRCUIDVIEW_H

#include <cppunit/extensions/HelperMacros.h>

namespace tests {
//=============================================================================
class TestFrxCircuidView : public CPPUNIT_NS::TestFixture {
//=============================================================================
private:
	CPPUNIT_TEST_SUITE( TestFrxCircuidView );
	CPPUNIT_TEST( testZOrder );
	CPPUNIT_TEST( testFindComponentsFiltered );
	CPPUNIT_TEST( testFindComponentsInArea );
	CPPUNIT_TEST( testGetIndexOf );
	CPPUNIT_TEST( testFindAllComponents );
	CPPUNIT_TEST_SUITE_END();
public:
	void testZOrder();
	void testFindComponentsFiltered();
	void testFindComponentsInArea();
	void testFindAllComponents();
	void testGetIndexOf();
};

} // namespace

#endif /* SAMBAG_TESTFRXCIRCUIDVIEW_H */
