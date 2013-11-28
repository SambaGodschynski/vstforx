/*
 * Session.cpp
 *
 *  Created on: Thu Nov 28 11:20:05 2013
 *      Author: Johannes Unger
 */

#include "Session.hpp"
#include "ShmCom.hpp"

namespace frx { namespace processing { namespace interprocess {
//=============================================================================
//  Class Session::IPChannel
//=============================================================================
struct Session::IPChannel {
    typedef ::sambag::com::interprocess::OffsetPtr<void>::Class VoidPtr;
    Integer opc;
    UInteger argsize, retsize;
    VoidPtr argmem;
    VoidPtr retmem;
};
//=============================================================================
//  Class Session
//=============================================================================
//-----------------------------------------------------------------------------
Session::Session(const std::string &id, ChannelSize a, ChannelSize b) : id(id)
{
    createBuffer(a, b);
}
//-----------------------------------------------------------------------------
Session::Session(const std::string &id) : id(id) {
    openBuffer();
}
//-----------------------------------------------------------------------------
Session::~Session() {
}
//-------------------------------------------------------------------------
void Session::destroyBuffer() {
}
//-----------------------------------------------------------------------------
void Session::process() {
}
//-----------------------------------------------------------------------------
Session::IPChannel * Session::getChannel() const {
    return NULL;
}
//-----------------------------------------------------------------------------
void Session::openBuffer() {
    using namespace ::sambag::com::interprocess;
    shm = findSharedMemoryObject(id.c_str());
    void *raw;
    boost::tie(raw, memorySize, mapped_region) = ipOpen( shm );
    memory_ptr = raw;
    pIt.setPointer(raw, memorySize);
    assignMemory(pIt);
    ++(*num_references);

    processChannel = channelB;
    requestChannel = channelA;
}
//-----------------------------------------------------------------------------
Integer Session::getNeededSize(ChannelSize a, ChannelSize b) {
    return boost::get<0>(a) +
           boost::get<1>(a) +
           boost::get<0>(b) +
           boost::get<1>(b) +
           sizeof(IPChannel) * 2 +
           sizeof(Mutex) +
           sizeof(UInteger);
}
//-----------------------------------------------------------------------------
void Session::createBuffer(ChannelSize a, ChannelSize b) {
  
	UInteger byteSize = getNeededSize(a, b);

    using namespace ::sambag::com::interprocess;
    shm = createSharedMemoryObject(id.c_str(), byteSize);
    
    void *raw;
    boost::tie(raw, memorySize, mapped_region) = ipMalloc( shm, byteSize );
    memory_ptr = raw;
    pIt.setPointer(raw, memorySize);
    
    assignMemory(pIt,
        boost::optional<ChannelSizes>(
            ChannelSizes(a,b)
        )
    );
    ++(*num_references);

    processChannel = channelA;
    requestChannel = channelB;
}
//-----------------------------------------------------------------------------
void Session::assignMemory(sambag::com::interprocess::PointerIterator &pIt,
        boost::optional<ChannelSizes> channelSizes)
{
    using namespace ::sambag::com::interprocess;
    typedef PlacementAlloc<Integer> Allocator;
    Allocator alloc(pIt);
    num_references = Allocator::rebind<Integer>::other(alloc).allocate(1);
    mutex = Allocator::rebind<Mutex>::other(alloc).allocate(1);
    channelA = Allocator::rebind<IPChannel>::other(alloc).allocate(1);
    channelB = Allocator::rebind<IPChannel>::other(alloc).allocate(1);
    
    UInteger size_aarg, size_aret, size_barg, size_bret;
    if (channelSizes) { // creatememory
        new(mutex) Mutex();
        ChannelSize a = boost::get<0>(*channelSizes);
        ChannelSize b = boost::get<1>(*channelSizes);
        size_aarg = boost::get<0>(a);
        size_aret = boost::get<1>(a);
        size_barg = boost::get<0>(b);
        size_bret = boost::get<1>(b);
    } else { // openmemory
        size_aarg = channelA->argsize;
        size_aret = channelA->retsize;
        size_barg = channelB->argsize;
        size_bret = channelB->retsize;
    }
    
    channelA->argmem = Allocator::rebind<char>::other(alloc).allocate(size_aarg);
    channelA->retmem = Allocator::rebind<char>::other(alloc).allocate(size_aret);
    channelB->argmem = Allocator::rebind<char>::other(alloc).allocate(size_barg);
    channelB->retmem = Allocator::rebind<char>::other(alloc).allocate(size_bret);
    
}
//-----------------------------------------------------------------------------
void Session::waitForResult(Opc opc) {
}
//-----------------------------------------------------------------------------
void * Session::getArgmem() const {
    return NULL;
}
//-----------------------------------------------------------------------------
void * Session::getRetmem() const {
    return NULL;
}
}}} // namespace(s)
