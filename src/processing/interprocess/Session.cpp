/*
 * Session.cpp
 *
 *  Created on: Thu Nov 28 11:20:05 2013
 *      Author: Johannes Unger
 */

#include "Session.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>
#include "ShmCom.hpp"
#include <sambag/com/exceptions/IllegalStateException.hpp>
#include <sambag/com/exceptions/IllegalArgumentException.hpp>

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
    struct TransferDataPurpose { // see @Session::transferData()
        Integer bytesToCopy;
        Integer opc;
        Mutex mutex;
        char data[FRX_SHMSESS_MAX_DATA_LENGTH];
    };
    TransferDataPurpose trData;
};
//=============================================================================
//  Class Session
//=============================================================================
//-----------------------------------------------------------------------------
struct Session::TransferSenderGuard {
    Mutex *mutex;
    typedef boost::shared_ptr<TransferSenderGuard> Ptr;
};
struct Session::TransferReceiverGuard {
    TransferReceiverGuard(Mutex *mutex) : mutex(mutex) {}
    Mutex *mutex;
    TransferReceiverGuard() { mutex->unlock(); }
    typedef boost::shared_ptr<TransferReceiverGuard> Ptr;
};
//-----------------------------------------------------------------------------
Session::Session(const std::string &id, ChannelSize a, ChannelSize b) : id(id)
{
    createBuffer(a, b);
    setPriority(DEFAULT_PRIORITY);
}
//-----------------------------------------------------------------------------
void Session::setPriority (Priority val) {
    SAMBAG_ASSERT(priority);
    if (priority) {
        *priority = (Integer)val;
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
    priority = NULL;
    if (processThread) {
        processThread->join();
        processThread.reset();
    }

    if (num_references && --(*num_references)==0) {
       destroyShm();
    }
    num_references = NULL;
}
//-----------------------------------------------------------------------------
void Session::destroyShm() {
    SAMBAG_LOG_INFO<<"destroying: " << name();
    mapped_region.reset();
    shm.reset();
    ipFree(id.c_str());
    SAMBAG_LOG_INFO<<"destroyed: " << name();
}
//-----------------------------------------------------------------------------
void Session::process() {
    SAMBAG_LOG_INFO<<"session "<<name()<<" process thread started";
    while (channelA && channelB)
    {
        if ( processChannel->opc != IDLE ) {
            if (processChannel->opc == TRANSFER_DATA) {
                _transferData();
                processChannel->opc = IDLE;
                continue;
            }
            if (processChannel->opc == CLEAR_DATA) {
                trData.second.clear();
                processChannel->opc = IDLE;
                continue;
            }
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
            processChannel->opc = IDLE;

        }
        sleep();
    }
    SAMBAG_LOG_INFO<<"session "<<name()<<" process thread closed";
}
//-----------------------------------------------------------------------------
void Session::startProcessThread() {
    processThread = ThreadPtr (
        new std::thread( &Session::process, this )
    );
}
//-----------------------------------------------------------------------------
void Session::openBuffer() {
    SAMBAG_LOG_INFO<<"try to establish session: '"<<name()<<"'";
    using namespace ::sambag::com::interprocess;
    try {
        shm = findSharedMemoryObject(id.c_str());
    } catch (const boost::interprocess::interprocess_exception &ex) {
        SAMBAG_THROW(Exception, "create session '" + name() + "' failed: " + ex.what());
    }

    void *raw;
    boost::tie(raw, memorySize, mapped_region) = ipOpen( shm );
    memory_ptr = raw;
    pIt.setPointer(raw, memorySize);
    assignMemory(pIt);
    
    if (*num_references>=2) {
        SAMBAG_THROW(Exception,
        "session already established");
    }
    ++(*num_references);

    processChannel = channelB;
    requestChannel = channelA;
    SAMBAG_LOG_INFO<<"session established: '"<<name()<<"'";
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
    SAMBAG_LOG_INFO<<"try to create session: '"<<name()<<"'";
	UInteger byteSize = getNeededSize(a, b);

    using namespace ::sambag::com::interprocess;
    try {
        shm = createSharedMemoryObject(id.c_str(), byteSize);
    } catch (const boost::interprocess::interprocess_exception &ex) {
        SAMBAG_THROW(Exception, "create session '" + name() + "' failed: " + ex.what());
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
    SAMBAG_LOG_INFO<<"session created: '"<<name()<<"'";
}
//-----------------------------------------------------------------------------
void Session::assignMemory(sambag::com::interprocess::PointerIterator &pIt,
        boost::optional<ChannelSizes> channelSizes)
{
    SAMBAG_LOG_TRACE<<sizeof(IPChannel);
    using namespace ::sambag::com::interprocess;
    typedef PlacementAlloc<Integer> Allocator;
    Allocator alloc(pIt);
    num_references = Allocator::rebind<Integer>::other(alloc).allocate(1);
    
    if (channelSizes && (*num_references) !=0) {
        SAMBAG_THROW(Exception, "Session exist already");
    }
    
    channelA = Allocator::rebind<IPChannel>::other(alloc).allocate(1);
    channelB = Allocator::rebind<IPChannel>::other(alloc).allocate(1);
    priority = Allocator::rebind<Integer>::other(alloc).allocate(1);

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
std::string Session::name() const {
    if (!requestChannel) {
        return "session-? " + getId();
    }
    bool host = requestChannel == channelB;
    std::stringstream ss;
    ss<<"session-"<<(host?"host":"client")<<" "<<getId();
    return ss.str();
}
//-----------------------------------------------------------------------------
void Session::waitForResultImpl(Opc opc, MemoryGuard::Ptr g, Integer timeout) const
{

    using namespace boost::interprocess;
    timeout*=1000; // millisec to microsec
    boost::posix_time::ptime ptout = boost::posix_time::from_time_t(std::time(NULL));
    ptout += boost::posix_time::microsec(timeout);
    
    requestChannel->opc = opc;
    int waited = 0;
    while (requestChannel->opc!=IDLE) {
        waited+=sleep();
        if (waited>=timeout) {
            std::stringstream ss;
            ss<<name()<<" OPC("<<opc<<") timed out";
            SAMBAG_THROW(TimeOut, ss.str());
        }
    }
}
//-----------------------------------------------------------------------------
void * Session::getArgmem() const {
    return requestChannel->argmem.get();
}
//-----------------------------------------------------------------------------
void * Session::getRetmem() const {
    return requestChannel->retmem.get();
}
//-----------------------------------------------------------------------------
void * Session::getArgmem() {
    return requestChannel->argmem.get();
}
//-----------------------------------------------------------------------------
void * Session::getRetmem() {
    return requestChannel->retmem.get();
}

//-----------------------------------------------------------------------------
size_t Session::getRequestArgmemSize() const {
    return requestChannel->argsize;
}
//-----------------------------------------------------------------------------
size_t Session::getRequestRetmemSize() const {
    return requestChannel->retsize;
}
//-----------------------------------------------------------------------------
size_t Session::getProcessArgmemSize() const {
    return processChannel->argsize;
}
//-----------------------------------------------------------------------------
size_t Session::getProcessRetmemSize() const {
    return processChannel->retsize;
}
//-----------------------------------------------------------------------------
void Session::_transferData() {
    if (processChannel->trData.bytesToCopy == 0) {
        return;
    }
    IPChannel::TransferDataPurpose &td = processChannel->trData;
    trData.first = td.opc;
    RawData &vec = trData.second;
    vec.insert(vec.end(), td.data, td.data + td.bytesToCopy);
}
//-----------------------------------------------------------------------------
void Session::transferData(Opc opc, void *data, int size, TransferSenderGuardPtr guard)
{
    if (!guard) {
        SAMBAG_THROW(sambag::com::exceptions::IllegalArgumentException,
        "Session::transferData no guard");
    }
    waitForProcess(CLEAR_DATA, getMemoryGuard());
    requestChannel->trData.opc=opc;
    static const int maxBytes = FRX_SHMSESS_MAX_DATA_LENGTH;
    while (size>0) {
        int bytesToCopy = size<maxBytes ? size:maxBytes;
        requestChannel->trData.bytesToCopy = bytesToCopy;
        memcpy(requestChannel->trData.data, data, bytesToCopy);
        waitForProcess(TRANSFER_DATA, getMemoryGuard());
        //iterate
        data=(char*)data+bytesToCopy;
        size-=bytesToCopy;
    }
}
//-----------------------------------------------------------------------------
std::pair<void *, Session::TransferReceiverGuard::Ptr>
Session::getTransferedData(Opc opc)
{
    if (trData.first != opc) {
        SAMBAG_LOG_WARN<<"Session::getTransferedDataPointer OPCs dosen't match";
        return std::make_pair((void*)NULL, TransferReceiverGuard::Ptr());
    }
    if (trData.second.empty()) {
        return std::make_pair((void*)NULL, TransferReceiverGuard::Ptr());
    }
    
    TransferReceiverGuard::Ptr guard(
        new TransferReceiverGuard(&processChannel->trData.mutex)
    );
    
    return std::make_pair(&(trData.second[0]), guard);
}
//-----------------------------------------------------------------------------
size_t Session::getTransferedDataSize(Opc opc) const {
    if (trData.first != opc) {
        return 0;
    }
    return trData.second.size();
}
//-----------------------------------------------------------------------------
Session::TransferSenderGuard::Ptr Session::getTransferSenderGuard(Integer timeout)
{
    using namespace boost::interprocess;
    TransferSenderGuard::Ptr res( new TransferSenderGuard() );
    res->mutex = &requestChannel->trData.mutex;
    
    boost::posix_time::ptime ptout = boost::posix_time::from_time_t(std::time(NULL));
    ptout += boost::posix_time::millisec(timeout);
    bool locked = res->mutex->timed_lock(ptout);
    if (!locked) {
        std::stringstream ss;
        ss<<name()<<"Session::beginDataTransfer() timed out";
        SAMBAG_THROW(TimeOut, ss.str());
    }
    return res;
}
//-----------------------------------------------------------------------------
Session::MemoryGuard::Ptr Session::getMemoryGuard(Integer timeout) const {
    using namespace boost::interprocess;
    MemoryGuard::Ptr res(new MemoryGuard(getArgmem(), getRetmem()));
    boost::posix_time::ptime ptout = boost::posix_time::from_time_t(std::time(NULL));
    ptout += boost::posix_time::millisec(timeout);
    res->lock = scoped_lock<Mutex>(requestChannel->mutex, ptout);
    if (!(res->lock)) {
        std::stringstream ss;
        ss<<name()<<"Session::getRequestMemory() timed out";
        SAMBAG_THROW(TimeOut, ss.str());
    }
    return res;
}
}}} // namespace(s)
