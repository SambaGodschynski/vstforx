/*
 * RemoteChReceiver.cpp
 *
 *  Created on: Tue Sep 17 21:10:30 2013
 *      Author: Johannes Unger
 */

#include "RemoteChReceiver.hpp"
#include "RemoteChannelManager.hpp"
#include "Stream.hpp"
#include <fstream>
#include <sambag/com/exceptions/IllegalStateException.hpp>

namespace frx { namespace processing { namespace interprocess {
//=============================================================================
//  Class RemoteChReceiver
//=============================================================================
//-----------------------------------------------------------------------------
RemoteChReceiver::RemoteChReceiver(frx::processing::IHostInfo::Ptr hostInfo,
                                   const std::string &rcId,
                                   size_t numOutputs) :
    pr::ProcessAdapter( hostInfo, 0, numOutputs ),
    blocksRead(Stream::UndefinedNumBlocks)
{
    setName ("RemoteChReceiver");
    RemoteChannelManager &rm = RemoteChannelManager::instance();
    ipStream = rm.getStream(rcId);
    if (ipStream->getBlockSize()!=(size_t)hostInfo->getBlockSize()) {
        SAMBAG_THROW(sambag::com::exceptions::IllegalStateException,
        "different blocksizes isn't supported yet.");
    }
    frames.setSize( hostInfo->getBlockSize() );
    frames.setZero( hostInfo->getBlockSize() );
    dcStream.setSize( hostInfo->getBlockSize(),  hostInfo->getBlockSize()*100);
}
//-----------------------------------------------------------------------------
void RemoteChReceiver::processAdapter( pr::Processor::Int numSamples ) {
    using namespace ::processing;
    // reading from ip stream
    float **data = frames.getData();
    int res = ipStream->read(data, blocksRead);
    if (res!=0) {
        static int c=0;
        std::cout<<++c<<": "<<res<<std::endl;
        blocksRead-=res;
        ipStream->read(data, blocksRead);
    }
    size_t bs = getHostInfo()->getBlockSize();
    // add ip data into dc stream
    dcStream.addFrame(&frames, bs, bs*100);
    // read from dc stream
	dcStream.flush(bs, data);
    outputNodes[0]->pushAndCopy( &frames, numSamples );
}
//-----------------------------------------------------------------------------
RemoteChReceiver::Ptr
RemoteChReceiver::create(frx::processing::IHostInfo::Ptr hostInfo,
    const std::string &rcId)
{
    RemoteChannelManager &rm = RemoteChannelManager::instance();
    Stream::Ptr ipStream = rm.getStream(rcId);
    if (!ipStream) {
        return RemoteChReceiver::Ptr();
    }
    size_t numChannels = ipStream->getNumChannels();
    if (numChannels==0) {
        return RemoteChReceiver::Ptr();
    }
    size_t numOutputs = ::com::numChannels2Xputs(numChannels);
    Ptr neu( new RemoteChReceiver(hostInfo, rcId, numOutputs) );
    neu->self = neu;
    neu->initListener();
    return neu;
}
}}} // namespace(s)
