/*
 * RemoteChannelcounterpart.cpp
 *
 *  Created on: Thu Sep 16 12:18:46 2013
 *      Author: Johannes Unger
 */


#include <processing/interprocess/Stream.hpp>
#include <processing/interprocess/RemoteChannelManager.hpp>
#include <boost/tuple/tuple.hpp>
#include <sambag/com/Interprocess.hpp>
#include <sambag/com/Common.hpp>
#include <iostream>
#include <boost/tuple/tuple_io.hpp>
#include <boost/foreach.hpp>
#include <cassert>
#include <cstring>

namespace si = sambag::com::interprocess;
namespace fi = frx::processing::interprocess;


std::string get_remote_channels() {
    fi::RemoteChannelManager &rm = fi::RemoteChannelManager::instance();
    
    std::vector< fi::RemoteChannelManager::RCId > res;
    rm.getChannels(res);

    std::stringstream ss;
    BOOST_FOREACH(const std::string &x, res) {
        ss<<x<<" ";
    }
    return ss.str();
}

std::string sum(si::UInteger stream_checksum) {
    fi::RemoteChannelManager &rm = fi::RemoteChannelManager::instance();
    std::vector< fi::RemoteChannelManager::RCId > res;
    rm.getChannels(res);
    assert(res.size() == 1);
    fi::Stream::Ptr stream = rm.getStream( res[0] );
    assert(stream);
    if (stream->getMemoryChecksum()!=stream_checksum) {
        std::stringstream ss;
        SAMBAG_LOG_WARN<<"checksum test failed: "<<stream_checksum<<"!="<<stream->getMemoryChecksum();
    }
    size_t nc =stream->getNumChannels();
    assert(nc>0);
    double *dblResult = new double[nc];
    memset(dblResult,0,sizeof(double)*nc);
    for (size_t i=0; i<stream->getBlockSize(); ++i) {
        for (size_t j=0; j<nc; ++j) {
            dblResult[j] += (*stream)[j][i];
        }
    }
    
    for (size_t j=0; j<stream->getNumParameter(); ++j) {
        dblResult[0] += stream->getParameter()[j];
        dblResult[1] += stream->getParameter()[j];
    }

    std::stringstream ss;
    for (size_t j=0; j<nc; ++j) {
        ss << dblResult[j] << ", ";
    }
    delete[] dblResult;
    return ss.str();
}


int main() {
    using namespace frx::processing::interprocess;
    using namespace boost::interprocess;
    using namespace ::sambag::com::interprocess;
    
    SharedMemoryObject shm = SharedMemoryObject(open_or_create, "RCC", read_write);
    shm.truncate(6400);
    MappedRegion mapped_region = MappedRegion(shm, read_write);
    void *ptr = mapped_region.get_address();
    PointerIterator pIt(ptr, 6400);
    typedef PlacementAlloc<char> Allocator;
    Allocator alloc(pIt);
    char *opc = alloc.allocate(50);
    char *result = alloc.allocate(255);
    UInteger *checksum = Allocator::rebind<UInteger>::other(alloc).allocate(1);
    std::string res="NO_OPC";
    if (strlen(opc)==0) {
        std::cout<<"no opcode"<<std::endl;
        return 1;
    }
    if (strcmp(opc, "get_remote_channels") == 0) {
        res = get_remote_channels();
    }
    if (strcmp(opc, "sum")==0) {
        res = sum(*checksum);
    }
    if (res=="NO_OPC") {
        std::cout<<"unknown opcode"<<std::endl;
        return 1;
    }
    strcpy(result, res.c_str());
}