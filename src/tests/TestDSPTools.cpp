/*
 * ===========================================================================================================
 * TestDSPTools.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */

#include "TestDSPTools.h"
#include <cppunit/config/SourcePrefix.h>
#include <processing/DSPTools.h>
#include <time.h>


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( tests::TestDSPTools );

void _testTimer(processing::Timer::SecondType timeToWait) {
	using namespace processing;
	Timer timer;
	time_t start = time(NULL);
	while(!timer.isElapsed(timeToWait));
	CPPUNIT_ASSERT_EQUAL((time_t)timeToWait, time(NULL) - start);
	cout<<".";
}


namespace tests {
//=============================================================================
void TestDSPTools::testTimer() {
//=============================================================================
	_testTimer(1.0);
	_testTimer(2.0);
	_testTimer(3.0);
	_testTimer(4.0);
	_testTimer(5.0);
}

} // namespace tests
