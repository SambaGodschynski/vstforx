/*
 * TestBridgeSessionManager.hpp
 *
 *  Created on: Mon Dec  2 10:44:37 2013
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_TESTBRIDGESESSIONMANAGER_H
#define SAMBAG_TESTBRIDGESESSIONMANAGER_H

#include <cppunit/extensions/HelperMacros.h>

namespace tests {
//=============================================================================
class TestBridgeSessionManager : public CPPUNIT_NS::TestFixture {
//=============================================================================
private:
	CPPUNIT_TEST_SUITE( TestBridgeSessionManager );
	CPPUNIT_TEST( testStartupBridge );
	CPPUNIT_TEST_SUITE_END();
public:
	void testStartupBridge();
};

} // namespace

#endif /* SAMBAG_TESTBRIDGESESSIONMANAGER_H */
