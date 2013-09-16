/*
 * Stream.cpp
 *
 *  Created on: Thu Sep 12 12:18:46 2013
 *      Author: Johannes Unger
 */

#include "Stream.hpp"
#include <sambag/com/exceptions/IllegalArgumentException.hpp>
#include <sambag/com/Interprocess.hpp>


namespace {
void createSharedMemoryObject(SharedMemoryObject &shm, const char * name) {
    using namespace boost::interprocess;
    shm = SharedMemoryObject(open_or_create, name, read_write);
}
void findSharedMemoryObject(SharedMemoryObject &shm, const char * name) {
    using namespace boost::interprocess;
    shm = SharedMemoryObject(open_only, name, read_write);
}
void * ipMalloc(SharedMemoryObject &shm, MappedRegion &mp, size_t size)
{
    using namespace boost::interprocess;
    if (size==0) {
        return NULL;
    }
    shm.truncate(size);
    mp = MappedRegion(shm, read_write);
    return mp.get_address();
}
void * ipOpen(SharedMemoryObject &shm, MappedRegion &mp)
{
    using namespace boost::interprocess;
    mp = MappedRegion(shm, read_write);
    return mp.get_address();
}

void ipFree(const char *name)
{
    using namespace boost::interprocess;
    shared_memory_object::remove(name);
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
    num_references(NULL),
    buffer(NULL)
{
}
//-----------------------------------------------------------------------------
Stream::~Stream() {
    using namespace ::sambag::com::interprocess;
    if (num_references==NULL) {
        return;
    }
    delete[] buffer;
    
    if (--(*num_references)==0) {
        ipFree(id.c_str());
    }
}
//-----------------------------------------------------------------------------
void Stream::createBuffer(size_t blockSize_soll, size_t numChannels_soll) {

    if (blockSize_soll==0 || numChannels_soll==0) {
        SAMBAG_THROW(sambag::com::exceptions::IllegalArgumentException,
        "creating interprocess::Stream with illegal arguments.");
    }


    using namespace ::sambag::com::interprocess;
    createSharedMemoryObject(shm, id.c_str());
    void *raw = ipMalloc( shm, mapped_region,
        sizeof(int) +
        sizeof(size_t) +
        sizeof(size_t) +
        sizeof(double)*blockSize_soll*numChannels_soll
    );
    // get num references
    num_references = (int*)raw;
    ++(*num_references);
    raw = num_references + 1;
    
    blockSize_ist = (size_t*)raw;
    (*blockSize_ist) = blockSize_soll;
    raw = blockSize_ist + 1;
    
    numChannels_ist = (size_t*)raw;
    *numChannels_ist = numChannels_soll;
    raw = numChannels_ist + 1;
    
    
     // map raw memory to buffer[]
    double *dPtr = (double*)(raw);
    buffer = new double*[numChannels_soll];
    for (size_t i=0; i<numChannels_soll; ++i) {
        buffer[i] = dPtr;
        dPtr+=blockSize_soll;
    }
}
//-----------------------------------------------------------------------------
void Stream::openBuffer() {

  
    using namespace ::sambag::com::interprocess;
    findSharedMemoryObject(shm, id.c_str());
    void *raw = ipOpen( shm, mapped_region );
    // get num references
    num_references = (int*)raw;
    ++(*num_references);
    raw = num_references + 1;
    
    blockSize_ist = (size_t*)raw;
    raw = blockSize_ist + 1;
    
    numChannels_ist = (size_t*)raw;
    raw = numChannels_ist + 1;
    
    
     // map raw memory to buffer[]
    double *dPtr = (double*)(raw);
    size_t n = getNumChannels();
    size_t bs = getBlockSize();
    
    if (n==0 || bs==0) {
        SAMBAG_THROW(sambag::com::exceptions::IllegalArgumentException,
        "open interprocess::Stream; illegal arguments.");
    }
    
    buffer = new double*[n];
    for (size_t i=0; i<n; ++i) {
        buffer[i] = dPtr;
        dPtr+=bs;
    }
}
//-----------------------------------------------------------------------------
Stream::Ptr Stream::create(const std::string &id,   
    size_t blockSize, size_t numChannels)
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
void Stream::write(double **data) {
    size_t nC = getNumChannels();
    size_t bC = getBlockSize();
    for (size_t i=0; i<nC; ++i) {
        for (size_t j=0; j<bC; ++j ) {
            buffer[i][j] = data[i][j];
        }
    }
}
//-----------------------------------------------------------------------------
void Stream::read(double **data) {
    size_t nC = getNumChannels();
    size_t bC = getBlockSize();
    for (size_t i=0; i<nC; ++i) {
        for (size_t j=0; j<bC; ++j ) {
            data[i][j] = buffer[i][j];
        }
    }
}
//-----------------------------------------------------------------------------
void Stream::resize(size_t blockSize, size_t numChannels) {
    #ifdef NDEBUG
    #error "implement me befor release!";
    #endif
}




}}} // namespace(s)
