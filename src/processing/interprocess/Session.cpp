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
    Mutex mutex;
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
    setMaxSleeping(10);
}
//-----------------------------------------------------------------------------
void Session::setMaxSleeping (Integer ms) {
    SAMBAG_ASSERT(sleepingTime);
    if (sleepingTime) {
        *sleepingTime = ms;
    }
}
//-----------------------------------------------------------------------------
Session::Session(const std::string &id) : id(id) {
    openBuffer();
}
//-----------------------------------------------------------------------------
Session::~Session() {
    channelA = NULL;
    channelB = NULL;
    sleepingTime = NULL;
    processThread->join();
    processThread.reset();

    if (num_references && --(*num_references)==0) {
       destroyShm();
    }
    num_references = NULL;
}
//-----------------------------------------------------------------------------
void Session::destroyShm() {
    SAMBAG_LOG_INFO<<"destroying: " << id;
    mapped_region.reset();
    shm.reset();
    ipFree(id.c_str());
    SAMBAG_LOG_INFO<<"destroyed: " << id;
}
//-----------------------------------------------------------------------------
void Session::process() {
    SAMBAG_LOG_INFO<<"session process thread started";
    while (channelA && channelB)
    {
        if ( processChannel->opc != IDLE ) {
            try {
                processImpl(
                    processChannel->opc,
                    processChannel->argmem.get(),
                    processChannel->retmem.get()
                );
            } catch(const std::exception &ex) {
                SAMBAG_LOG_ERR<<"Session::process(): "<<ex.what();
            }
            catch(...) {
                SAMBAG_LOG_ERR<<"Session::process(): unkown";
            }
        }
        processChannel->opc = IDLE;
        boost::this_thread::sleep(boost::posix_time::millisec(*sleepingTime));
    }
    SAMBAG_LOG_INFO<<"session process thread closed";
}
//-----------------------------------------------------------------------------
void Session::startProcessThread() {
    processThread = ThreadPtr (
        new boost::thread( boost::bind(&Session::process, this) )
    );
}
//-----------------------------------------------------------------------------
void Session::openBuffer() {
    SAMBAG_LOG_INFO<<"try to establish session: '"<<id<<"'";
    using namespace ::sambag::com::interprocess;
    try {
        shm = findSharedMemoryObject(id.c_str());
    } catch (const boost::interprocess::interprocess_exception &ex) {
        SAMBAG_THROW(Exception, "create session '" + id + "' failed: " + ex.what());
    }

    void *raw;
    boost::tie(raw, memorySize, mapped_region) = ipOpen( shm );
    memory_ptr = raw;
    pIt.setPointer(raw, memorySize);
    assignMemory(pIt);
    
    ++(*num_references);

    processChannel = channelB;
    requestChannel = channelA;
    startProcessThread();
    SAMBAG_LOG_INFO<<"session established: '"<<id<<"'";
}
//-----------------------------------------------------------------------------
Integer Session::getNeededSize(ChannelSize a, ChannelSize b) {
    return boost::get<0>(a) +
           boost::get<1>(a) +
           boost::get<0>(b) +
           boost::get<1>(b) +
           sizeof(IPChannel) * 2 +
           sizeof(Mutex) +
           sizeof(UInteger) * 6400;
}
//-----------------------------------------------------------------------------
void Session::createBuffer(ChannelSize a, ChannelSize b) {
    SAMBAG_LOG_INFO<<"try to create session: '"<<id<<"'";
	UInteger byteSize = getNeededSize(a, b);

    using namespace ::sambag::com::interprocess;
    try {
        shm = createSharedMemoryObject(id.c_str(), byteSize);
    } catch (const boost::interprocess::interprocess_exception &ex) {
        SAMBAG_THROW(Exception, "create session '" + id + "' failed: " + ex.what());
    }
    void *raw;
    boost::tie(raw, memorySize, mapped_region) = ipMalloc( shm, byteSize );
    memory_ptr = raw;
    pIt.setPointer(raw, memorySize);
    
    assignMemory(pIt,
        boost::optional<ChannelSizes>(
            ChannelSizes(a,b)
        )
    );
    
    *num_references=1;
    
    processChannel = channelA;
    requestChannel = channelB;
    startProcessThread();
    SAMBAG_LOG_INFO<<"session created: '"<<id<<"'";
}
//-----------------------------------------------------------------------------
void Session::assignMemory(sambag::com::interprocess::PointerIterator &pIt,
        boost::optional<ChannelSizes> channelSizes)
{
    using namespace ::sambag::com::interprocess;
    typedef PlacementAlloc<Integer> Allocator;
    Allocator alloc(pIt);
    num_references = Allocator::rebind<Integer>::other(alloc).allocate(1);
    
    if (channelSizes && (*num_references) !=0) {
        SAMBAG_THROW(Exception, "Session exist already");
    }
    
    channelA = Allocator::rebind<IPChannel>::other(alloc).allocate(1);
    channelB = Allocator::rebind<IPChannel>::other(alloc).allocate(1);
    sleepingTime = Allocator::rebind<Integer>::other(alloc).allocate(1);

    if (channelSizes) { // creatememory
        // init values
        channelA->opc = IDLE;
        channelB->opc = IDLE;
        ChannelSize a = boost::get<0>(*channelSizes);
        ChannelSize b = boost::get<1>(*channelSizes);
        channelA->argsize = boost::get<0>(a);
        channelA->retsize = boost::get<1>(a);
        channelB->argsize = boost::get<0>(b);
        channelB->retsize = boost::get<1>(b);
    }
    channelA->argmem = Allocator::rebind<char>::other(alloc).allocate(channelA->argsize);
    channelA->retmem = Allocator::rebind<char>::other(alloc).allocate(channelA->retsize);
    channelB->argmem = Allocator::rebind<char>::other(alloc).allocate(channelB->argsize);
    channelB->retmem = Allocator::rebind<char>::other(alloc).allocate(channelB->retsize);
}
//-----------------------------------------------------------------------------
void * Session::waitForResultImpl(Opc opc, Integer timeout) {
    using namespace boost::interprocess;
    scoped_lock<Mutex> lock(requestChannel->mutex);
    requestChannel->opc = opc;
    int waited = 0;
    while (requestChannel->opc!=IDLE) {
        boost::this_thread::sleep(boost::posix_time::millisec(*sleepingTime));
        waited+=*sleepingTime;
        if (waited>=timeout) {
            SAMBAG_THROW(TimeOut, "Session::waitForResult timed out");
        }
    }
    return getRetmem();
}
//-----------------------------------------------------------------------------
void * Session::getArgmem() const {
   return requestChannel->argmem.get();
}
//-----------------------------------------------------------------------------
void * Session::getRetmem() const {
    return requestChannel->retmem.get();
}
}}} // namespace(s)
