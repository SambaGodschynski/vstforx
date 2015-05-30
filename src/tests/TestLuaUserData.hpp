/*
 * TestLuaUserData.hpp
 *
 *  Created on: Fri Aug 29 10:44:19 2014
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_TESTLUAUSERDATA_H
#define SAMBAG_TESTLUAUSERDATA_H

#include <cppunit/extensions/HelperMacros.h>

namespace tests {
//=============================================================================
class TestLuaUserData : public CPPUNIT_NS::TestFixture {
//=============================================================================
private:
	CPPUNIT_TEST_SUITE( TestLuaUserData );
	CPPUNIT_TEST( testLuaUserData );
    CPPUNIT_TEST( testPersistLuaUserData );
    CPPUNIT_TEST( testMigration );
	CPPUNIT_TEST_SUITE_END();
public:
	void testLuaUserData();
    void testPersistLuaUserData();
    void testMigration();

};

} // namespace

#endif /* SAMBAG_TESTLUAUSERDATA_H */
