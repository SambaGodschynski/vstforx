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

namespace {
    const int PARAM_OBSERVER_INTERVAL_MS=30;
}

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
    dcStream.setSize( hostInfo->getBlockSize(),  0/*hostInfo->getBlockSize()*/);
    initParameters(ipStream->getNumParameter());
}
//-----------------------------------------------------------------------------
void RemoteChReceiver::processAdapter( pr::Processor::Int numSamples ) {
    using namespace ::processing;
    // reading from ip stream
    float **data = frames.getData();
    int res = ipStream->read(data, blocksRead);
    if (res!=0) {
        blocksRead-=res;
        ipStream->read(data, blocksRead);
    }
    size_t bs = getHostInfo()->getBlockSize();
    // add ip data into dc stream
    dcStream.addFrame(&frames, bs, 0);
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
    neu->initParameterObserver();
    return neu;
}
//-----------------------------------------------------------------------------
void RemoteChReceiver::initParameterObserver() {
    parameterObserver = FrxAsyncDSPTimer::create(PARAM_OBSERVER_INTERVAL_MS);
    parameterObserver->setNumRepetitions(-1);
    parameterObserver->addTrackedEventListener(
        boost::bind(&RemoteChReceiver::onParameterObserver, this),
        getPtr()
    );
    parameterObserver->start();
}
//-----------------------------------------------------------------------------
void RemoteChReceiver::onParameterObserver()
{
    if (!ipStream) {
        return;
    }
    
    SAMBAG_ASSERT(ipStream->getNumParameter()==parameters.size());
    
    Stream::ValueType *pvalue = ipStream->getParameter();
    size_t n = ipStream->getNumParameter();
    size_t i = 0;
    while(n-- > 0) {
        if (parameters[i]->getValue() != *pvalue) {
            parameters[i]->setValue(*(pvalue));
        }
        ++i;
        ++pvalue;
    }
}
//-----------------------------------------------------------------------------
void RemoteChReceiver::initParameters(size_t num) {
    parameters.reserve(num);
    for (size_t i=0; i<num; ++i) {
        prp::Parameter::Ptr p = prp::Parameter::create();
        p->setName("receiver.param"+sambag::com::toString(i+1));
        parameters.push_back(p);
    }
}
//-----------------------------------------------------------------------------
prp::Parameter::Ptr RemoteChReceiver::getParameter (size_t index) const
{
    return parameters.at(index);
}
//-----------------------------------------------------------------------------
size_t RemoteChReceiver::getNumParameter () const {
    return parameters.size();
}
}}} // namespace(s)
