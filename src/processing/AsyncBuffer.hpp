/*
 * AsyncBuffer.hpp
 *
 *  Created on: Mon Sep 23 10:43:26 2013
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_ASYNCBUFFER_H
#define SAMBAG_ASYNCBUFFER_H

#include <boost/shared_ptr.hpp>

#include <memory>
#include <cstring>
#include <boost/static_assert.hpp>
#include <boost/interprocess/sync/interprocess_upgradable_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/sharable_lock.hpp>
#include <sambag/com/Common.hpp>

namespace frx { namespace processing {
//=============================================================================
/** 
  * @class AsyncBuffer.
  * A buffer optimized for async reading and writing.
  *   - assumes that reader and write use the same blocksize
  *   - reader gets always the last written block
  */
template <
    typename T,
    int _NumBlocks = 2, //<< Buffersize min 2.
    template <class> class _Allocator = std::allocator
>
class AsyncBuffer {
//=============================================================================
BOOST_STATIC_ASSERT(_NumBlocks>=2);
public:
    //-------------------------------------------------------------------------
    enum { NumBlocks = _NumBlocks };
    //-------------------------------------------------------------------------
    typedef T ValueType;
    //-------------------------------------------------------------------------
    typedef _Allocator<T> Allocator;
    //-------------------------------------------------------------------------
    typedef size_t Samples;
    //-------------------------------------------------------------------------
    static const Samples InvalidSamplePos = UINT_MAX;
protected:
    //-------------------------------------------------------------------------
    void copy(T *from, T *to, Samples size) const {
        memcpy(to, from, sizeof(T)*size);
    }
    //-------------------------------------------------------------------------
    template <typename U, typename V>
    void copy(U *from, V *to, Samples size) const {
        for (Samples i=0; i<size; ++i) {
            to[i] = (V)from[i];
        }
    }
    //-------------------------------------------------------------------------
    void deallocate();
    //-------------------------------------------------------------------------
    T **buffer;
    //-------------------------------------------------------------------------
    size_t blocksWritten, blockSize, numChannels;
    //-------------------------------------------------------------------------
    Allocator allocator;
    //-------------------------------------------------------------------------
    inline size_t cyc_bounds( size_t i ) { return i%getSize(); }
    //-------------------------------------------------------------------------
    inline int getLastWrittenBlockNumber() const {
        if (blocksWritten==0) {
            return -1;
        }
        int x=blocksWritten%NumBlocks;
        if (x==0) {
            return NumBlocks-1;
        };
        return x-1;
    }
    //-------------------------------------------------------------------------
    inline int getCurrentBlockNumber() const {
        return blocksWritten%NumBlocks;
    }
    //-------------------------------------------------------------------------
    inline Samples toSamplePos(int numBlocks) const {
        if (numBlocks<0 || numBlocks>=NumBlocks) {
            return InvalidSamplePos;
        }
        return getBlockSize()*numBlocks;
    }
public:
    //-------------------------------------------------------------------------
    virtual ~AsyncBuffer();
    //-------------------------------------------------------------------------
    AsyncBuffer(const Allocator &alloc = Allocator());
    //-------------------------------------------------------------------------
    void allocate(size_t blockSize, size_t numChannels);
    //-------------------------------------------------------------------------
    template <typename U>
    void writeBlock(U **data);
    //-------------------------------------------------------------------------
    /**
     * @return the number of samples of one block
     */
    inline Samples getBlockSize() const {
        return blockSize;
    }
    //-------------------------------------------------------------------------
    /**
     * @return the number of samples of the whole buffer = blockSize*NumBlocks 
     */
    inline Samples getSize() const {
        return blockSize*NumBlocks;
    }
    //-------------------------------------------------------------------------
    inline size_t getNumChannels() const {
        return numChannels;
    }
    //-------------------------------------------------------------------------
    /**
     * copies last writen block to out.
     * @param allocated out data
     * @param number blocks which was already read by reader 
     * @return blocksRead+1 when reading was successfull otherwise blocksRead.
     */
    template <typename U>
    size_t readLastWrittenBlock(U **out, size_t blocksRead) const;
    //-------------------------------------------------------------------------
    Allocator & getAllocator() {
        return allocator;
    }
    //-------------------------------------------------------------------------
    const Allocator & getAllocator() const {
        return allocator;
    }
    //-------------------------------------------------------------------------
    T ** getBuffer() const {
        return buffer;
    }
}; // AsyncBuffer
///////////////////////////////////////////////////////////////////////////////
template < typename T, int I, template <class> class A >
AsyncBuffer<T, I, A>::AsyncBuffer(const Allocator &allocator) :
    buffer(NULL),
    blocksWritten(0),
    blockSize(0),
    numChannels(0),
    allocator(allocator)
{
}
//-----------------------------------------------------------------------------
template < typename T, int I, template <class> class A >
AsyncBuffer<T, I, A>::~AsyncBuffer()
{
    deallocate();
}
//-----------------------------------------------------------------------------
template < typename T, int I, template <class> class A >
void AsyncBuffer<T, I, A>::allocate(size_t blockSize, size_t numChannels)
{
    typename Allocator:: template rebind<T*>::other ptrAlloc(allocator);
    buffer = ptrAlloc.allocate(numChannels);
    for (size_t i = 0; i<numChannels; ++i) {
        buffer[i] = allocator.allocate(blockSize*NumBlocks);
        for (size_t j=0; j<blockSize*NumBlocks; ++j) {
            buffer[i][j] = 0;
        }
    }
    this->blockSize = blockSize;
    this->numChannels = numChannels;
}
//-----------------------------------------------------------------------------
template < typename T, int I, template <class> class A >
void AsyncBuffer<T, I, A>::deallocate()
{
    typename Allocator:: template rebind<T*>::other ptrAlloc(allocator);
    for (size_t i = 0; i<numChannels; ++i) {
        allocator.deallocate(buffer[i], getSize());
    }
    ptrAlloc.deallocate(buffer, numChannels);
    this->blockSize = 0;
    this->numChannels = 0;
    buffer = NULL;
}
//-----------------------------------------------------------------------------
template < typename T, int I, template <class> class A >
template < typename U>
void AsyncBuffer<T, I, A>::writeBlock(U **data)
{    
    for (size_t i=0; i<getNumChannels(); ++i) {
        T *ptr = buffer[i];
        ptr+=toSamplePos(getCurrentBlockNumber());
        copy(data[i], ptr, getBlockSize());
    }
    ++blocksWritten;
}
//-----------------------------------------------------------------------------
template < typename T, int I, template <class> class A >
template < typename U>
size_t
AsyncBuffer<T, I, A>::readLastWrittenBlock(U **out, size_t blocksRead) const
{
    if ((blocksRead+1)>blocksWritten) {
        return blocksRead;
    }
    for (size_t i=0; i<getNumChannels(); ++i) {
        T *ptr = buffer[i];
        ptr+=toSamplePos(getLastWrittenBlockNumber());
        copy(ptr, out[i], getBlockSize());
    }
    return blocksRead+1;
}
}} // namespace(s)


#endif /* SAMBAG_ASYNCBUFFER_H */
