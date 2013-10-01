/*
 * TestRemoteChannelManager.cpp
 *
 *  Created on: Sat Sep 14 09:24:21 2013
 *      Author: Johannes Unger
 */

#include "TestRemoteChannelManager.hpp"
#include <cppunit/config/SourcePrefix.h>
#include <processing/interprocess/RemoteChannelManager.hpp>
#include <iostream>
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_io.hpp>
#include <sambag/com/Interprocess.hpp>

using sambag::com::interprocess::Integer;
using sambag::com::interprocess::UInteger;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( tests::TestRemoteChannelManager );

#ifdef WIN32
    const char * COUNTERPART_EXEC = "./remoteChannelCounterpart.exe";
#else   
    const char * COUNTERPART_EXEC = "./remoteChannelCounterpart";
#endif

namespace tests {
//=============================================================================
//  Class TestRemoteChannelManager
//=============================================================================
//-----------------------------------------------------------------------------
void TestRemoteChannelManager::startUp() {
}
//-----------------------------------------------------------------------------
void TestRemoteChannelManager::tearDown() {
    using namespace frx::processing::interprocess;
    RemoteChannelManager &rm = RemoteChannelManager::instance();
    rm.removeAllChannels();
}
//-----------------------------------------------------------------------------
void TestRemoteChannelManager::testAddGetChannels() {
    using namespace frx::processing::interprocess;
    
    // ** Consider: Managers totmann timer isn't running here **
    RemoteChannelManager &rm = RemoteChannelManager::instance();
    rm.__addChannel_("RemoteChannel1", boost::make_tuple("stream_1"));
    rm.__addChannel_("RemoteChannel2", boost::make_tuple("stream_2"));
    rm.__addChannel_("RemoteChannel3", boost::make_tuple("stream_3"));
    
    std::vector< RemoteChannelManager::RCId > res;
    rm.getChannels(res);
    CPPUNIT_ASSERT_EQUAL((size_t)3, res.size());
    std::stringstream ss;
    ss<<res[0]<<res[1]<<res[2];
    CPPUNIT_ASSERT_EQUAL(std::string("RemoteChannel1RemoteChannel2RemoteChannel3"), ss.str());
    
    rm.removeChannel("RemoteChannel3");
    CPPUNIT_ASSERT_EQUAL((UInteger)2, rm.getNumChannels());
    rm.removeChannel("RemoteChannel2");
    CPPUNIT_ASSERT_EQUAL((UInteger)1, rm.getNumChannels());
    rm.removeChannel("RemoteChannel1");
    CPPUNIT_ASSERT_EQUAL((UInteger)0, rm.getNumChannels());

}
//-----------------------------------------------------------------------------
void TestRemoteChannelManager::testRemoteChannelManager() {
    using namespace frx::processing::interprocess;
    
    
    // ** Consider: Managers totmann timer isn't running here **
    RemoteChannelManager &rm = RemoteChannelManager::instance();
    CPPUNIT_ASSERT_EQUAL((UInteger)0, rm.getNumChannels());
    
    std::string name = rm.createUniqueName();
    Stream::Ptr stream = Stream::create(name,512,2);
    rm.__addChannel_(
        "RemoteChannel 1",
        boost::make_tuple(name)
    );
    
    si::SharedMemoryHolder shmh("RCC", 6400);
    si::String::Class *opc = si::String::findOrCreate("opc", shmh.get());
    *opc = "get_remote_channels";
    CPPUNIT_ASSERT_EQUAL((int) 0, std::system(COUNTERPART_EXEC));
    
    si::String::Class *result = si::String::findOrCreate("result", shmh.get());
    CPPUNIT_ASSERT_EQUAL(
        std::string("RemoteChannel 1 "),
        std::string(result->c_str())
    );
    
    double datal[512];
    double datar[512];
    double *in[2] = { &datar[0], &datal[0] };
    for (UInteger i=0; i<512; ++i) {
        in[0][i] = i+1;
        in[1][i] = i+2;
    }
    for (UInteger i=0; i<10; ++i) {
        stream->write(&in[0]);
        *opc="sum";
        CPPUNIT_ASSERT_EQUAL((int) 0, std::system(COUNTERPART_EXEC));
        CPPUNIT_ASSERT_EQUAL(
            std::string("131328, 131840, "),
            std::string(result->c_str())
        );
    }
}

} //namespace
