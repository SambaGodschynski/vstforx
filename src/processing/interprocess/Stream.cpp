/*
 * Stream.cpp
 *
 *  Created on: Thu Sep 12 12:18:46 2013
 *      Author: Johannes Unger
 */

#include "Stream.hpp"
#include <sambag/com/exceptions/IllegalArgumentException.hpp>
#include <boost/functional/hash.hpp>
#include <boost/tuple/tuple.hpp>
#include <cstring>
#include <sambag/com/SharedMemory.hpp> 
#include <sambag/com/SharedMemoryImpl.hpp> 

namespace {
using sambag::com::interprocess::Integer;
using sambag::com::interprocess::UInteger;
SharedMemoryObjectPtr createSharedMemoryObject(const char * name, size_t maxMemory) {
    using namespace boost::interprocess;
    return SharedMemoryObjectPtr(
		new SAMBAG_SHARED_MEMORY_OBJECT_CREATE(open_or_create, name, read_write, maxMemory)
    );
}
SharedMemoryObjectPtr findSharedMemoryObject(const char * name) {
    using namespace boost::interprocess;
    return SharedMemoryObjectPtr(
		new si::SharedMemoryObject(open_only, name, read_write)
    );
}
boost::tuple<void*, UInteger, MappedRegionPtr>
ipMalloc(SharedMemoryObjectPtr shm, UInteger size)
{
    using namespace boost::interprocess;
    if (size==0) {
        return NULL;
    }
    SAMBAG_SHARED_MEMORY_TRUNC(*(shm.get()), size+sizeof(int));
    MappedRegionPtr mp = MappedRegionPtr(
		new si::MappedRegion(*(shm.get()), read_write)
    );
    void *res = mp->get_address();
    
    UInteger *memorySize = (UInteger*)res;
    *memorySize = size;
    res = memorySize+1;
    return boost::make_tuple(res, size, mp);
}

boost::tuple<void*, UInteger, MappedRegionPtr>
ipOpen(SharedMemoryObjectPtr shm)
{
    using namespace boost::interprocess;
    MappedRegionPtr mp = MappedRegionPtr(
		new si::MappedRegion(*(shm.get()), read_write)
    );
    void *res =  mp->get_address();
    UInteger *memorySize = (UInteger*)res;
    res = memorySize+1;
    return boost::make_tuple(res, *memorySize, mp);
}

void ipFree(const char *name)
{
    SAMBAG_SHARED_MEMORY_REMOVE(name);
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
    mutex(NULL),
    blockSize_ist(NULL),
    numChannels_ist(NULL),
    numParameter_ist(NULL),
    lastWrittenTime(NULL),
    num_references(NULL)
{
}
//-----------------------------------------------------------------------------
void Stream::destroyMemory() {
    num_references = NULL;
    blockSize_ist = NULL;
    numParameter_ist = NULL;
    numChannels_ist = NULL;
    lastWrittenTime = NULL;
    mutex = NULL;
    mapped_region.reset();
    shm.reset();
    ipFree(id.c_str());
}
//-----------------------------------------------------------------------------
Stream::~Stream() {
    if (num_references && --(*num_references)==0) {
       destroyMemory();
    }
}
//-----------------------------------------------------------------------------
UInteger Stream::getNeededSize(UInteger blockSize,
    UInteger numChannel, UInteger numParameter) const
{
    return  sizeof(Integer) +
            sizeof(UInteger)*4 +
            sizeof(Mutex)  +
            sizeof(ValueType)*blockSize*AudioBuffer::NumChannels*AudioBuffer::NumBlocks +
            sizeof(ValueType)*numParameter+
            6400;
}
//-----------------------------------------------------------------------------
void Stream::assignMemory(sambag::com::interprocess::PointerIterator &pIt,
    UInteger numChannel, UInteger numBlockSize, UInteger numParameter)
{
    using namespace ::sambag::com::interprocess;
    typedef PlacementAlloc<ValueType> Allocator;
    Allocator alloc(pIt);

    num_references = Allocator::rebind<Integer>::other(alloc).allocate(1);
    blockSize_ist = Allocator::rebind<UInteger>::other(alloc).allocate(1);
    numChannels_ist = Allocator::rebind<UInteger>::other(alloc).allocate(1);
    numParameter_ist = Allocator::rebind<UInteger>::other(alloc).allocate(1);
    lastWrittenTime = Allocator::rebind<UInteger>::other(alloc).allocate(1);
    mutex = Allocator::rebind<Mutex>::other(alloc).allocate(1);
    
    if (numParameter!=0) {
        *numParameter_ist = numParameter;
    }
    
    if (numParameter_ist>0) {
        parameter = Allocator::rebind<ValueType>::other(alloc).allocate(*numParameter_ist);
    }
    
    // always at last, because the pointer iterator is used in createBuffer(),
    // for allocating buffer memory, but not in openBuffer().
    // so after createBuffer or openBuffer the pointer iteraror points to
    // different locations.
    buffer = Allocator::rebind<AudioBuffer>::other(alloc).allocate(1);
}
//-----------------------------------------------------------------------------
void Stream::createBuffer(UInteger blockSize_soll,
    UInteger numChannels_soll, UInteger numParameter_soll)
{
    if (numChannels_soll > 2) {
        SAMBAG_THROW(sambag::com::exceptions::IllegalArgumentException,
        "interprocess::Stream multichannel not supported yet.");
    }

    if (blockSize_soll==0 || numChannels_soll==0) {
        SAMBAG_THROW(sambag::com::exceptions::IllegalArgumentException,
        "creating interprocess::Stream with illegal arguments.");
    }

	UInteger byteSize = getNeededSize(blockSize_soll, numChannels_soll, numParameter_soll);

    using namespace ::sambag::com::interprocess;
    shm = createSharedMemoryObject(id.c_str(), byteSize);
    
    void *raw;
    boost::tie(raw, memorySize, mapped_region) = ipMalloc( shm, byteSize );
    memory_ptr = raw;
    pIt.setPointer(raw, memorySize);
    
    assignMemory(pIt, numChannels_soll, blockSize_soll, numParameter_soll);
    ++(*num_references);
    *blockSize_ist = blockSize_soll;
    *numChannels_ist = numChannels_soll;
    new(mutex) Mutex();
    
    typedef AudioBuffer::Allocator Allocator;
    Allocator alloc(pIt);
    new(buffer) AudioBuffer();
    buffer->allocate(blockSize_soll, alloc);
    buffer->setZero();
}
//-----------------------------------------------------------------------------
void Stream::openBuffer() {
    using namespace ::sambag::com::interprocess;
    shm = findSharedMemoryObject(id.c_str());
    void *raw;
    boost::tie(raw, memorySize, mapped_region) = ipOpen( shm );
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
    UInteger blockSize, UInteger numChannels, UInteger numParameter)
{
    Ptr res = Ptr( new Stream() );
    res->id = id;
    res->createBuffer(blockSize, numChannels, numParameter);
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
UInteger Stream::getMemoryChecksum() {
    return checksum(memory_ptr, memorySize);
}




}}} // namespace(s)