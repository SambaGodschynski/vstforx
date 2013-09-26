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
  */
template <
    typename T,
    int _NumBlocks  = 2, //<< Buffersize min 2.
    int _NumChannels = 2, //<< Buffersize min 1.
    template <class> class _Allocator = std::allocator
>
class AsyncBuffer {
//=============================================================================
BOOST_STATIC_ASSERT(_NumBlocks>=2);
BOOST_STATIC_ASSERT(_NumChannels>=1);
public:
    //-------------------------------------------------------------------------
    enum { NumBlocks = _NumBlocks, NumChannels = _NumChannels };
    //-------------------------------------------------------------------------
    typedef T ValueType;
    //-------------------------------------------------------------------------
    typedef _Allocator<T> Allocator;
    //-------------------------------------------------------------------------
    static const size_t InvalidSamplePos = UINT_MAX;
    //-------------------------------------------------------------------------
    static const size_t UndefinedNumBlocks = UINT_MAX;
protected:
    //-------------------------------------------------------------------------
    /*TODO: test void copy(T *from, T *to, Samples size) const {
        memcpy(to, from, sizeof(T)*size);
    }*/
    //-------------------------------------------------------------------------
    template <typename U, typename V>
    void copy(U *from, V *to, size_t size) const {
        for (size_t i=0; i<size; ++i) {
            to[i] = (V)from[i];
        }
    }
    //-------------------------------------------------------------------------
    void deallocate();
    //-------------------------------------------------------------------------
    T *buffer[NumChannels];
    //-------------------------------------------------------------------------
    size_t blocksWritten, blockSize;
    //-------------------------------------------------------------------------
    Allocator allocator;
    //-------------------------------------------------------------------------
    inline size_t cyc_bounds( size_t i ) { return i%getSize(); }
    //-------------------------------------------------------------------------
    inline int getLastWrittenBlockNumber() const {
        if (blocksWritten==0) {
            return -1;
        }
        int x=toBufferBlocks(blocksWritten);
        if (x==0) {
            return NumBlocks-1;
        };
        return x-1;
    }
    //-------------------------------------------------------------------------
    /**
     * offset to lastwritten block
     */
    inline int blockOffset(size_t readBlocks) const {
        return readBlocks-blocksWritten+1;
    }
    //-------------------------------------------------------------------------
    /**
     * concernes the buffer bounds
     */
    inline int toBufferBlocks(size_t blocks) const {
        return blocks%NumBlocks;
    }
    //-------------------------------------------------------------------------
    inline int getCurrentBlockNumber() const {
        return toBufferBlocks(blocksWritten);
    }
    //-------------------------------------------------------------------------
    inline size_t toSamplePos(int numBlocks) const {
        if (numBlocks<0 || numBlocks>=NumBlocks) {
            return InvalidSamplePos;
        }
        return getBlockSize()*numBlocks;
    }
public:
    //-------------------------------------------------------------------------
    /** @return 0 no missing blocks.
     *          negative values: x blocks to late
     *          positive values: x blocks to early
     */
    inline int missingBlocks(size_t readBlocks) const {
        int bo=blockOffset(readBlocks);
        if (bo>=0) {
            return bo;
        }
        bo+=NumBlocks-1;
        if (bo<0) {
            return bo;
        }
        return 0;
    }
    //-------------------------------------------------------------------------
    virtual ~AsyncBuffer();
    //-------------------------------------------------------------------------
    AsyncBuffer(const Allocator &alloc = Allocator());
    //-------------------------------------------------------------------------
    void allocate(size_t blockSize);
    //-------------------------------------------------------------------------
    template <typename U>
    void writeBlock(U **data);
    //-------------------------------------------------------------------------
    /**
     * @return the number of samples of one block
     */
    inline size_t getBlockSize() const {
        return blockSize;
    }
    //-------------------------------------------------------------------------
    /**
     * @return the number of samples of the whole buffer = blockSize*NumBlocks 
     */
    inline size_t getSize() const {
        return blockSize*NumBlocks;
    }
    //-------------------------------------------------------------------------
    inline size_t getNumChannels() const {
        return NumChannels;
    }
    //-------------------------------------------------------------------------
    /**
     * @param the allocated out container
     * @param [in/out] the number of the already read blocks, increments value when
     *        reading was successfull
     * @return 0 when reading was successfull, otherwise the number of missing blocks. 
     *         negative values: x blocks to late
     *         positive values: x blocks to early
     */
    template <typename U>
    int readBlock(U **out, size_t &blocksRead) const;
    //-------------------------------------------------------------------------
    Allocator & getAllocator() {
        return allocator;
    }
    //-------------------------------------------------------------------------
    const Allocator & getAllocator() const {
        return allocator;
    }
    //-------------------------------------------------------------------------
    T * operator[](size_t channel) const {
        return buffer[channel];
    }
}; // AsyncBuffer
///////////////////////////////////////////////////////////////////////////////
template < typename T, int I, int J, template <class> class A >
AsyncBuffer<T, I, J, A>::AsyncBuffer(const Allocator &allocator) :
    blocksWritten(0),
    blockSize(0),
    allocator(allocator)
{
}
//-----------------------------------------------------------------------------
template < typename T, int I, int J, template <class> class A >
AsyncBuffer<T, I, J, A>::~AsyncBuffer()
{
    deallocate();
}
//-----------------------------------------------------------------------------
template < typename T, int I, int J, template <class> class A >
void AsyncBuffer<T, I, J, A>::allocate(size_t blockSize)
{
    for (size_t i = 0; i<getNumChannels(); ++i) {
        buffer[i] = allocator.allocate(blockSize*NumBlocks);
        for (size_t j=0; j<blockSize*NumBlocks; ++j) {
            buffer[i][j] = 0;
        }
    }
    this->blockSize = blockSize;
}
//-----------------------------------------------------------------------------
template < typename T, int I, int J, template <class> class A >
void AsyncBuffer<T, I, J, A>::deallocate()
{
    for (size_t i = 0; i<getNumChannels(); ++i) {
        allocator.deallocate(buffer[i], getSize());
    }
    this->blockSize = 0;
}
//-----------------------------------------------------------------------------
template < typename T, int I, int J, template <class> class A >
template < typename U>
void AsyncBuffer<T, I, J, A>::writeBlock(U **data)
{    
    for (size_t i=0; i<getNumChannels(); ++i) {
        T *ptr = buffer[i];
        ptr+=toSamplePos(getCurrentBlockNumber());
        copy(data[i], ptr, getBlockSize());
    }
    ++blocksWritten;
}
//-----------------------------------------------------------------------------
template < typename T, int I, int J, template <class> class A >
template < typename U>
int
AsyncBuffer<T, I, J, A>::readBlock(U **out, size_t &blocksRead) const
{
    if (blocksRead==UndefinedNumBlocks) {
        if (blocksWritten>0) {
            // set blocksread
            blocksRead=blocksWritten-1;
        } else {
            return 1;
        }
    }
    if (missingBlocks(blocksRead)!=0) {
        // out of sync
        return missingBlocks(blocksRead);
    }
    for (size_t i=0; i<getNumChannels(); ++i) {
        T *ptr = buffer[i];
        ptr+=toSamplePos(toBufferBlocks(blocksRead));
        copy(ptr, out[i], getBlockSize());
    }
    ++blocksRead;
    return 0;
}
}} // namespace(s)


#endif /* SAMBAG_ASYNCBUFFER_H */
