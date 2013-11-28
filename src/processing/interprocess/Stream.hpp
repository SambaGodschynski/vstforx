/*
 * Stream.hpp
 *
 *  Created on: Thu Sep 12 12:18:46 2013
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_INTERPROCESS_STREAM_H
#define SAMBAG_INTERPROCESS_STREAM_H

#include <boost/shared_ptr.hpp>
#include <string>
#include <boost/interprocess/sync/interprocess_upgradable_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/sharable_lock.hpp>
#include <processing/Frames.h>
#include <sambag/com/SharedMemory.hpp>
#include <sambag/com/PlacementAlloc.hpp>
#include <processing/AsyncBuffer.hpp>
#include <boost/static_assert.hpp>
#include "ShmCom.hpp"


namespace frx { namespace processing { namespace interprocess {
//=============================================================================
/**
 * @class IPMemoryPolicy
 */
 template <typename T, int BlockSize, int NumChannels>
 struct IPMemoryPolicy :
    public MemoryPolicyBase<T, UInteger, BlockSize, NumChannels>
//=============================================================================
{
    typedef MemoryPolicyBase<T, UInteger, BlockSize, NumChannels> Super;
    typedef typename ::sambag::com::interprocess::OffsetPtr<T>::Class ValuePtr;
    typedef ::sambag::com::interprocess::PlacementAlloc<T> Allocator;
    ValuePtr buffer[NumChannels];
    void allocate(UInteger blockSize, Allocator &allocator)
    {
        Super::allocateImpl(allocator, buffer, blockSize);
        Super::setBlockSize(blockSize);
    }
    void deallocate()
    {
        // no need for deallocating
    }
    inline T * operator[](UInteger channel) const
    {
        return buffer[channel].get();
    }
    void setZero() {
        Super::setZeroImpl(buffer);
    }
};
//=============================================================================
/** 
  * @class Stream.
  * Interprocess audio stream
  */
class Stream {
//=============================================================================
public:
    //-------------------------------------------------------------------------
    enum { MAX_BUFFER_BLOCKS = 2 };
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<Stream> Ptr;
    //-------------------------------------------------------------------------
    typedef float ValueType;
    //-------------------------------------------------------------------------
    static const UInteger UndefinedNumBlocks =
        boost::integer_traits<UInteger>::const_max;
protected:
    //-------------------------------------------------------------------------
    Stream();
    //-------------------------------------------------------------------------
    UInteger getNeededSize(UInteger blockSize,
    UInteger numChannel, UInteger numParameter) const;
    //-------------------------------------------------------------------------
    void assignMemory(sambag::com::interprocess::PointerIterator &pIt,
                      UInteger numChannel=0, UInteger numBlockSize=0,
                      UInteger numParameter=0);
private:
    //-------------------------------------------------------------------------
    void destroyMemory();
    //-------------------------------------------------------------------------
    void *memory_ptr;
    //-------------------------------------------------------------------------
    UInteger memorySize;
    //-------------------------------------------------------------------------
    sambag::com::interprocess::PointerIterator pIt;
    //-------------------------------------------------------------------------
    typedef boost::interprocess::interprocess_upgradable_mutex Mutex;
    //-------------------------------------------------------------------------
    Mutex *mutex;
    //-------------------------------------------------------------------------
    UInteger *blockSize_ist, *numChannels_ist, *numParameter_ist, *lastWrittenTime;
    //-------------------------------------------------------------------------
    Integer *num_references;
    //-------------------------------------------------------------------------
    ValueType *parameter;
    //-------------------------------------------------------------------------
    typedef AsyncBuffer<ValueType, MAX_BUFFER_BLOCKS, 2, IPMemoryPolicy> AudioBuffer;
    AudioBuffer *buffer;
    // ensure that size of value type dosen't changes with compiler/arch
    BOOST_STATIC_ASSERT( sizeof(AudioBuffer::ValueType) == 4 );
    //-------------------------------------------------------------------------
    void createBuffer(UInteger blockSize_soll,
        UInteger numChannels_soll, UInteger numParameter_soll);
    //-------------------------------------------------------------------------
    void openBuffer();
    //-------------------------------------------------------------------------
    std::string id;
    //-------------------------------------------------------------------------
    SharedMemoryObjectPtr shm;
    MappedRegionPtr mapped_region;
public:
    //-------------------------------------------------------------------------
    /**
     * @return checksum of the whole shared memory.
     * @note stream has to be opened or created befores
     */
    UInteger getMemoryChecksum();
    //-------------------------------------------------------------------------
    virtual ~Stream();
    //-------------------------------------------------------------------------
    static Ptr create(const std::string &id, UInteger blockSize,
        UInteger numChannels, UInteger numParameter);
    //-------------------------------------------------------------------------
    static Ptr open(const std::string &id);
    //-------------------------------------------------------------------------
    template <typename T>
    void write(T **data);
    //-------------------------------------------------------------------------
    template <typename T>
    void write(T **data, UInteger numSamples);
    //-------------------------------------------------------------------------
    template <typename T>
    int read(T **out, UInteger &inoutReadBlocks);
    //-------------------------------------------------------------------------
    ValueType * operator[](UInteger channel) const;
    //-------------------------------------------------------------------------
    UInteger getBlockSize() const {
        if (!blockSize_ist) {
            return 0;
        }
        return *blockSize_ist;
    }
    //-------------------------------------------------------------------------
    UInteger getNumChannels() const {
        if (!numChannels_ist) {
            return 0;
        }
        return *numChannels_ist;
    }
    //-------------------------------------------------------------------------
    UInteger getNumParameter() const {
        if (!numParameter_ist) {
            return 0;
        }
        return *numParameter_ist;
    }
    //-------------------------------------------------------------------------
    ValueType * getParameter() const {
        return parameter;
    }
    //-------------------------------------------------------------------------
    const std::string & getId() const { return id; }
    //-------------------------------------------------------------------------
    UInteger getLastWrittenTime() const {
        if (!lastWrittenTime) {
            return 0;
        }
        return *lastWrittenTime;
    }
}; // Stream
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
template <typename T>
void Stream::write(T **data) {
    using namespace boost::interprocess;
    scoped_lock<Mutex> lock(*mutex);
    buffer->writeBlock(data);
    *lastWrittenTime = ::time(NULL);
}
//-----------------------------------------------------------------------------
template <typename T>
void Stream::write(T **data, UInteger numSamples) {
    using namespace boost::interprocess;
    scoped_lock<Mutex> lock(*mutex);
    buffer->write(data, numSamples);
    *lastWrittenTime = ::time(NULL);
}
//-----------------------------------------------------------------------------
template <typename T>
int Stream::read(T **out, UInteger &blocksRead) {
    using namespace boost::interprocess;
    sharable_lock<Mutex> lock(*mutex);
    return buffer->readBlock(out, blocksRead);
}

}}} // namespace(s)

#endif /* SAMBAG_INTERPROCESS_STREAM_H */
