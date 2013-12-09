/*
 * ============================================================================
 * TestDSPTools.cpp
 *      Author: Johannes Unger
 * ============================================================================
 */

#include "TestDSPTools.h"
#include <cppunit/config/SourcePrefix.h>
#include <processing/DSPTools.h>
#include <time.h>


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( tests::TestDSPTools );
namespace tests {
//=============================================================================
namespace {
    void _testTimer(processing::Timer::SecondType timeToWait) {
        using namespace processing;
        Timer timer;
        time_t start = time(NULL);
        while(!timer.isElapsed(timeToWait));
        CPPUNIT_ASSERT_EQUAL((time_t)timeToWait, time(NULL) - start);
        std::cout<<"."<<std::flush;
    }
}
void TestDSPTools::testTimer() {
//-----------------------------------------------------------------------------
	_testTimer(1.0);
	_testTimer(2.0);
	_testTimer(3.0);
	_testTimer(4.0);
	_testTimer(5.0);
}
//=============================================================================
namespace {
    using processing::FadeValue;
    void _fade( FadeValue &fader, float e, float d, int steps)
    {
        fader.setDuration (d);
		fader.setValue(e);
        while (steps-- > 0) {
            float value = fader.getValue(); // getValue incr. faders t by 1
            CPPUNIT_ASSERT( value >= 0.f );
            CPPUNIT_ASSERT( value <= 1.f );
        }
    }
}
void TestDSPTools::testIssue446() {
//-----------------------------------------------------------------------------
    FadeValue fader;
    
    _fade(fader, 1.f, 100.f, 100);
    
    CPPUNIT_ASSERT_EQUAL(1.f, fader.getValue());
    CPPUNIT_ASSERT_EQUAL(1.f, fader.getValue());
    CPPUNIT_ASSERT_EQUAL(1.f, fader.getValue());
    
    _fade(fader, 0.f, 100.f, 100);
    
    CPPUNIT_ASSERT_EQUAL(0.f, fader.getValue());
    CPPUNIT_ASSERT_EQUAL(0.f, fader.getValue());
    CPPUNIT_ASSERT_EQUAL(0.f, fader.getValue());
    
    _fade(fader, 1.f, 100.f, 50);
    _fade(fader, 0.f, 100.f, 20);
    _fade(fader, 1.f, 10.f, 5);
    _fade(fader, 0.f, 1.f, 1);
    _fade(fader, 1.f, 1.f, 1);
    _fade(fader, 0.f, 1000.f, 100);
    _fade(fader, 1.f, 10.f, 10);
    _fade(fader, 0.f, 10.f, 10);
    _fade(fader, 1.f, 1000.f, 100);
    _fade(fader, 0.f, 10.f, 10);
    _fade(fader, 1.f, 1000.f, 2000);
    _fade(fader, 0.f, 10.f, 100);
    _fade(fader, 1.f, 10.f, 100);
    _fade(fader, 0.f, 2.f, 10);
    _fade(fader, 1.f, 10.f, 100);
    _fade(fader, 0.f, 0.1f, 10);
    _fade(fader, 1.f, 0.01f, 10);
    _fade(fader, 0.f, 0.01f, 10);
    _fade(fader, 1.f, 5000.0f, 10);
    _fade(fader, 0.f, 0.01f, 10);
    _fade(fader, 0.117292f, 1.f, 10);
    _fade(fader, 0.f, 2640.f, 2641);
    
}

} // namespace tests
