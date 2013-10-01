/*
 * Stream.cpp
 *
 *  Created on: Thu Sep 12 12:18:46 2013
 *      Author: Johannes Unger
 */

#include "Stream.hpp"
#include <sambag/com/exceptions/IllegalArgumentException.hpp>
#include <sambag/com/Interprocess.hpp>
#include <boost/functional/hash.hpp>
#include <boost/tuple/tuple.hpp>
#include <cstring>

namespace {
using sambag::com::interprocess::Integer;
using sambag::com::interprocess::UInteger;
void createSharedMemoryObject(SharedMemoryObject &shm, const char * name) {
    using namespace boost::interprocess;
    shm = SharedMemoryObject(open_or_create, name, read_write);
}
void findSharedMemoryObject(SharedMemoryObject &shm, const char * name) {
    using namespace boost::interprocess;
    shm = SharedMemoryObject(open_only, name, read_write);
}
boost::tuple<void*, UInteger>
ipMalloc(SharedMemoryObject &shm, MappedRegion &mp, UInteger size)
{
    using namespace boost::interprocess;
    if (size==0) {
        return NULL;
    }
    shm.truncate(size+sizeof(int));
    mp = MappedRegion(shm, read_write);
    void *res = mp.get_address();
    
    UInteger *memorySize = (UInteger*)res;
    *memorySize = size;
    res = memorySize+1;
    return boost::make_tuple(res, size);
}

boost::tuple<void*, UInteger>
ipOpen(SharedMemoryObject &shm, MappedRegion &mp)
{
    using namespace boost::interprocess;
    mp = MappedRegion(shm, read_write);
    void *res =  mp.get_address();
    UInteger *memorySize = (UInteger*)res;
    res = memorySize+1;
    return boost::make_tuple(res, *memorySize);
}

void ipFree(const char *name)
{
    using namespace boost::interprocess;
    shared_memory_object::remove(name);
}



UInteger checksum(void *ptr, UInteger bytesize) {
    unsigned char *c = (unsigned char*)ptr;
    std::stringstream ss;
    while (bytesize-- > 0) {
        ss<<*c;
        ++c;
    }
    boost::hash<std::string> stringHash;
    return stringHash(ss.str());
}

} // namespace(s)


namespace frx { namespace processing { namespace interprocess {
//=============================================================================
//  Class Stream
//=============================================================================
//-----------------------------------------------------------------------------
Stream::Stream() :
    blockSize_ist(NULL),
    numChannels_ist(NULL),
    num_references(NULL)
{
}
//-----------------------------------------------------------------------------
Stream::~Stream() {
    using namespace ::sambag::com::interprocess;
    if (num_references==NULL) {
        return;
    }
    if (--(*num_references)==0) {
        ipFree(id.c_str());
    }
}
//-----------------------------------------------------------------------------
UInteger Stream::getNeededSize(UInteger blockSize, UInteger numChannel) const {
    return  sizeof(Integer) +
            sizeof(UInteger)*3 +
            sizeof(Mutex)  +
            sizeof(ValueType)*blockSize*Buffer::NumChannels*Buffer::NumBlocks +
            6400;
}
//-----------------------------------------------------------------------------
void Stream::assignMemory(sambag::com::interprocess::PointerIterator &pIt,
    UInteger numChannel, UInteger numBlockSize)
{
    using namespace ::sambag::com::interprocess;
    typedef PlacementAlloc<ValueType> Allocator;
    Allocator alloc(pIt);

    num_references = Allocator::rebind<Integer>::other(alloc).allocate(1);
    blockSize_ist = Allocator::rebind<UInteger>::other(alloc).allocate(1);
    numChannels_ist = Allocator::rebind<UInteger>::other(alloc).allocate(1);
    mutex = Allocator::rebind<Mutex>::other(alloc).allocate(1);
    
    // always at last, because the pointer iterator is alose used in createBuffer()
    // ,for allocating buffer memory, but not in openBuffer().
    // so after createBuffer or openBuffer the pointer iteraror points to
    // different locations.
    buffer = Allocator::rebind<Buffer>::other(alloc).allocate(1);
    
}
//-----------------------------------------------------------------------------
void Stream::createBuffer(UInteger blockSize_soll, UInteger numChannels_soll) {
    if (numChannels_soll > 2) {
        SAMBAG_THROW(sambag::com::exceptions::IllegalArgumentException,
        "interprocess::Stream multichannel not supported yet.");
    }

    if (blockSize_soll==0 || numChannels_soll==0) {
        SAMBAG_THROW(sambag::com::exceptions::IllegalArgumentException,
        "creating interprocess::Stream with illegal arguments.");
    }

    using namespace ::sambag::com::interprocess;
    createSharedMemoryObject(shm, id.c_str());
    UInteger byteSize = getNeededSize(blockSize_soll, numChannels_soll);
    void *raw;
    boost::tie(raw, memorySize) = ipMalloc( shm, mapped_region, byteSize );
    memory_ptr = raw;
    pIt.setPointer(raw, memorySize);
    
    assignMemory(pIt, numChannels_soll, blockSize_soll);
    ++(*num_references);
    (*blockSize_ist) = blockSize_soll;
    *numChannels_ist = numChannels_soll;
    new(mutex) Mutex();
    
    typedef Buffer::Allocator Allocator;
    Allocator alloc(pIt);
    new(buffer) Buffer();
    buffer->setAllocator(&alloc);
    buffer->allocate(blockSize_soll);
    buffer->setZero();
}
//-----------------------------------------------------------------------------
void Stream::openBuffer() {
    using namespace ::sambag::com::interprocess;
    findSharedMemoryObject(shm, id.c_str());
    void *raw;
    boost::tie(raw, memorySize) = ipOpen( shm, mapped_region );
    memory_ptr = raw;
    pIt.setPointer(raw, memorySize);
    assignMemory(pIt);
    ++(*num_references);
}
//-----------------------------------------------------------------------------
Stream::ValueType * Stream::operator[](UInteger channel) const {
    return (*buffer)[channel];
}
//-----------------------------------------------------------------------------
Stream::Ptr Stream::create(const std::string &id,   
    UInteger blockSize, UInteger numChannels)
{
    Ptr res = Ptr( new Stream() );
    res->id = id;
    res->createBuffer(blockSize, numChannels);
    return res;
}
//-----------------------------------------------------------------------------
Stream::Ptr Stream::open(const std::string &id)
{
    Ptr res = Ptr( new Stream() );
    res->id = id;
    try {
        res->openBuffer();
    } catch (...) {
        return Stream::Ptr();
    }
    return res;
}
//-----------------------------------------------------------------------------
void Stream::resize(UInteger blockSize, UInteger numChannels) {
    #ifdef NDEBUG
    #error "implement me befor release!";
    #endif
}
//-----------------------------------------------------------------------------
UInteger Stream::getMemoryChecksum() {
    return checksum(memory_ptr, memorySize);
}




}}} // namespace(s)
