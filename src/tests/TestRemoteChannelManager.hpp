/*
 * TestRemoteChannelManager.hpp
 *
 *  Created on: Sat Sep 14 09:24:21 2013
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_TESTREMOTECHANNELMANAGER_H
#define SAMBAG_TESTREMOTECHANNELMANAGER_H

#include <cppunit/extensions/HelperMacros.h>
#include <sambag/com/UnitEx.hpp>

namespace tests {
//=============================================================================
class TestRemoteChannelManager : public CPPUNIT_NS::TestFixture {
//=============================================================================
private:
	CPPUNIT_TEST_SUITE( TestRemoteChannelManager );
	CPPUNIT_TEST_KNOWN_ISSUE(testAddGetChannels);
    CPPUNIT_TEST_KNOWN_ISSUE(testRemoteChannelManager);
	CPPUNIT_TEST_SUITE_END();
public:
    void startUp();
    void tearDown();
	void testAddGetChannels();
    void testRemoteChannelManager();
};

} // namespace

#endif /* SAMBAG_TESTREMOTECHANNELMANAGER_H */
