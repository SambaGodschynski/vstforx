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
#include <sambag/com/SharedMemory.hpp>
#include <sambag/com/SharedMemoryImpl.hpp>

using sambag::com::interprocess::Integer;
using sambag::com::interprocess::UInteger;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( tests::TestRemoteChannelManager );

#ifdef WIN32
	#ifdef _DEBUG
		const char * COUNTERPART_EXEC = "Debug\\remoteChannelCounterpart.exe";
	#else
		const char * COUNTERPART_EXEC = "Release\\remoteChannelCounterpart.exe";
	#endif
#else   
    const char * COUNTERPART_EXEC = "arch -32 ./remoteChannelCounterpart";
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
    rm.addChannel("RemoteChannel1", boost::make_tuple("stream_1"));
    rm.addChannel("RemoteChannel2", boost::make_tuple("stream_2"));
    rm.addChannel("RemoteChannel3", boost::make_tuple("stream_3"));
    CPPUNIT_ASSERT_EQUAL((size_t)3, (size_t)rm.getNumChannels());
    
    {
        std::vector< RemoteChannelManager::RCId > res;
        rm.getChannels(res);
        CPPUNIT_ASSERT_EQUAL((size_t)3, res.size());
        std::stringstream ss;
        ss<<res[0]<<res[1]<<res[2];
        CPPUNIT_ASSERT_EQUAL(std::string("RemoteChannel1RemoteChannel2RemoteChannel3"), ss.str());
    }
    rm.removeChannel("RemoteChannel3");
    CPPUNIT_ASSERT_EQUAL((UInteger)2, rm.getNumChannels());
    {
        std::vector< RemoteChannelManager::RCId > res;
        rm.getChannels(res);
        CPPUNIT_ASSERT_EQUAL((size_t)2, res.size());
        std::stringstream ss;
        ss<<res[0]<<res[1];
        CPPUNIT_ASSERT_EQUAL(std::string("RemoteChannel1RemoteChannel2"), ss.str());
    }
    
    rm.removeChannel("RemoteChannel1");
    CPPUNIT_ASSERT_EQUAL((UInteger)1, rm.getNumChannels());
    {
        std::vector< RemoteChannelManager::RCId > res;
        rm.getChannels(res);
        CPPUNIT_ASSERT_EQUAL((size_t)1, res.size());
        std::stringstream ss;
        ss<<res[0];
        CPPUNIT_ASSERT_EQUAL(std::string("RemoteChannel2"), ss.str());
    }

    rm.removeChannel("RemoteChannel2");
    CPPUNIT_ASSERT_EQUAL((UInteger)0, rm.getNumChannels());
    {
        std::vector< RemoteChannelManager::RCId > res;
        rm.getChannels(res);
        CPPUNIT_ASSERT_EQUAL((size_t)0, res.size());
    }
    
    rm.addChannel("RemoteChannel4", boost::make_tuple("stream_1"));
    rm.addChannel("RemoteChannel5", boost::make_tuple("stream_2"));
    rm.addChannel("RemoteChannel6", boost::make_tuple("stream_3"));
    {
        std::vector< RemoteChannelManager::RCId > res;
        rm.getChannels(res);
        CPPUNIT_ASSERT_EQUAL((size_t)3, res.size());
        std::stringstream ss;
        ss<<res[0]<<res[1]<<res[2];
        CPPUNIT_ASSERT_EQUAL(std::string("RemoteChannel4RemoteChannel5RemoteChannel6"), ss.str());
    }
    rm.removeAllChannels();
    CPPUNIT_ASSERT_EQUAL((UInteger)0, rm.getNumChannels());


}
//-----------------------------------------------------------------------------
void TestRemoteChannelManager::testRemoteChannelManager() {
    using namespace frx::processing::interprocess;
    using namespace boost::interprocess;
    using namespace ::sambag::com::interprocess;
    
    // ** Consider: Managers totmann timer isn't running here **
    RemoteChannelManager &rm = RemoteChannelManager::instance();
    CPPUNIT_ASSERT_EQUAL((UInteger)0, rm.getNumChannels());
    
    std::string name = rm.createUniqueName();
    Stream::Ptr stream = Stream::create(name,512,2, 100);
    rm.addChannel(
        "RemoteChannel 1",
        boost::make_tuple(name)
    );
    
    SharedMemoryObject shm = SAMBAG_SHARED_MEMORY_OBJECT_CREATE(open_or_create, "RCC", read_write, 6400);
    SAMBAG_SHARED_MEMORY_TRUNC(shm, 6400);
    MappedRegion mapped_region = MappedRegion(shm, read_write);
    void *ptr = mapped_region.get_address();
    PointerIterator pIt(ptr, 6400);
    typedef PlacementAlloc<char> Allocator;
    Allocator alloc(pIt);
    
    char *opc = alloc.allocate(50);
    char *result = alloc.allocate(255);
    UInteger *checksum = Allocator::rebind<UInteger>::other(alloc).allocate(1);
    strcpy(opc, "get_remote_channels");
    
    CPPUNIT_ASSERT_EQUAL((int) 0, std::system(COUNTERPART_EXEC));
    
    CPPUNIT_ASSERT_EQUAL(
        std::string("RemoteChannel 1 "),
        std::string(result)
    );

    double datal[512];
    double datar[512];
    double *in[2] = { &datar[0], &datal[0] };
    for (UInteger i=0; i<512; ++i) {
        in[0][i] = i+1;
        in[1][i] = i+2;
    }
    for (UInteger i=0; i<100; ++i) {
        stream->getParameter()[i] = i+3;
    }
    for (UInteger i=0; i<10; ++i) {
        stream->write(&in[0]);
        *checksum = stream->getMemoryChecksum();
        strcpy(opc, "sum");
        CPPUNIT_ASSERT_EQUAL((int) 0, std::system(COUNTERPART_EXEC));
        CPPUNIT_ASSERT_EQUAL(
            std::string("136578, 137090, "),
            std::string(result)
        );
    }
}

} //namespace
