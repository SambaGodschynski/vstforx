/*
 * TestBridgeSessionManager.cpp
 *
 *  Created on: Mon Dec  2 10:44:37 2013
 *      Author: Johannes Unger
 */

#include "TestBridgeSessionManager.hpp"
#include <cppunit/config/SourcePrefix.h>
#include <processing/interprocess/BridgeSessionManager.hpp>
#include <sambag/com/Thread.hpp>
#include <processing/FrxAsyncDSPTimer.hpp>
#include <sambag/com/BoostTimer2.hpp>


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( tests::TestBridgeSessionManager );

namespace tests {
//=============================================================================
//  Class TestBridgeSessionManager
//=============================================================================
//-----------------------------------------------------------------------------
void TestBridgeSessionManager::testStartupBridge() {
    using namespace frx::processing::interprocess;
    
    typedef sambag::com::BoostTimer2 Timer;
    Timer::WorkerThreadHolder wth = Timer::startWorkerThread();
    
    BridgeSessionManager &bm = BridgeSessionManager::instance();
    bm.setBridgePath("./bridge");
    
    PluginSessionClientPtr plugin = bm.createPluginSession("plugin", 44100.f, 512);
 
    while(bm.isBridgeSessionEstabished()) {
        boost::this_thread::sleep( boost::posix_time::seconds(1) );
    }
    
    Timer::closeAllTimer();
}
} //namespace
