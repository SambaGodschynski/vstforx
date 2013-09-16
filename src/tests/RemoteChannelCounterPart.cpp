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

std::string sum() {
    fi::RemoteChannelManager &rm = fi::RemoteChannelManager::instance();
    std::vector< fi::RemoteChannelManager::RCId > res;
    rm.getChannels(res);
    assert(res.size() == 1);
    fi::Stream::Ptr stream = rm.getStream( res[0] );
    assert(stream);
    size_t nc =stream->getNumChannels();
    assert(nc>0);
    double *dblResult = new double[nc];
    memset(dblResult,0,sizeof(double)*nc);
    for (size_t i=0; i<stream->getBlockSize(); ++i) {
        for (size_t j=0; j<nc; ++j) {
            dblResult[j] += stream->getBuffer()[j][i];
        }
    }
    std::stringstream ss;
    for (size_t j=0; j<nc; ++j) {
        ss << dblResult[j] << ", ";
    }
    delete[] dblResult;
    return ss.str();
}


int main() {
    si::SharedMemoryHolder shmh("RCC", 6400);
    si::String::Class *opc = si::String::findOrCreate("opc", shmh.get());
    std::string res="NO_OPC";
    if (!opc) {
        std::cout<<"no opcode"<<std::endl;
        return 1;
    }
    if (*opc=="get_remote_channels") {
        res = get_remote_channels();
    }
    if (*opc=="sum") {
        res = sum();
    }
    if (res=="NO_OPC") {
        std::cout<<"unknown opcode"<<std::endl;
        return 1;
    }
    si::String::Class *result = si::String::findOrCreate("result", shmh.get());
    *result = res.c_str();
}