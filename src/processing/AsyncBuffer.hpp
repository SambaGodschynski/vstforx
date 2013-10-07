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
#include <sambag/com/Interprocess.hpp>

namespace frx { namespace processing {
using ::sambag::com::interprocess::Integer;
using ::sambag::com::interprocess::UInteger;
//=============================================================================
/** 
  * @class MemoryPolicy
  * Since we need the AsyncBuffer for interprocess purpose, we need
  * some special memory action moves.
  */
template <
    typename T,
    typename _SizeType,
    int _NumBlocks,
    int _NumChannels
>
class MemoryPolicyBase {
public:
    //-------------------------------------------------------------------------
    enum { NumBlocks = _NumBlocks, NumChannels = _NumChannels };
    //-------------------------------------------------------------------------
    typedef _SizeType SizeType;
private:
    //-------------------------------------------------------------------------
    SizeType blockSize;
public:
    //-------------------------------------------------------------------------
    MemoryPolicyBase() : blockSize(0) {}
    //-------------------------------------------------------------------------
    /*TODO: test void copy(T *from, T *to, Samples size) const {
        memcpy(to, from, sizeof(T)*size);
    }*/
    //-------------------------------------------------------------------------
    template <typename U, typename V>
    inline void copy(U *from, V *to, SizeType size) const {
        for (SizeType i=0; i<size; ++i) {
            to[i] = (V)from[i];
        }
    }
    //-------------------------------------------------------------------------
    template <class Alloc, typename Buffer>
    void deallocateImpl(Alloc &allocator, Buffer &buffer, SizeType size);
    //-------------------------------------------------------------------------
    template <class Alloc, typename Buffer>
    void allocateImpl(Alloc &allocator, Buffer &buffer, SizeType blockSize);
    //-------------------------------------------------------------------------
    /**
     * @return the number of samples of the whole buffer = blockSize*NumBlocks 
     */
    inline SizeType getSize() const {
        return blockSize*NumBlocks;
    }
    //-------------------------------------------------------------------------
    /**
     * @return the number of samples of one block
     */
    inline SizeType getBlockSize() const {
        return blockSize;
    }
    //-------------------------------------------------------------------------
    template <class Buffer>
    void setZeroImpl(Buffer &buffer) {
        for (SizeType i = 0; i<NumChannels; ++i) {
            // TODO: replace with memset
            for (SizeType j=0; j<blockSize*NumBlocks; ++j) {
                buffer[i][j] = 0;
            }
        } 
    }
    //-------------------------------------------------------------------------
    void setBlockSize(SizeType blockSize) {
        this->blockSize = blockSize;
    }
};
//-----------------------------------------------------------------------------
template < typename T, typename S, int I, int J>
template <class Alloc, typename Buffer>
void MemoryPolicyBase<T, S, I, J>::allocateImpl(Alloc &allocator,
    Buffer &buffer, SizeType blockSize)
{
    for (SizeType i = 0; i<NumChannels; ++i) {
        buffer[i] = allocator.allocate(blockSize*NumBlocks);
    } 
}
//-----------------------------------------------------------------------------
template < typename T, typename S, int I, int J>
template <class Alloc, typename Buffer>
void MemoryPolicyBase<T, S, I, J>::deallocateImpl(Alloc &allocator,
    Buffer &buffer, SizeType size)
{
    for (SizeType i = 0; i<NumChannels; ++i) {
        allocator.deallocate(buffer[i], size);
    }
}

//=============================================================================
/** 
  * @class MemoryPolicy
  * Since we need the AsyncBuffer for interprocess purpose, we need
  * some special memory action moves.
  */
template <
    typename T,
    int _NumBlocks,
    int _NumChannels
>
class DefaultMemoryPolicy :
    public MemoryPolicyBase<T, std::size_t, _NumBlocks, _NumChannels>
{
public:
    //-------------------------------------------------------------------------
    enum { NumBlocks = _NumBlocks, NumChannels = _NumChannels };
    //-------------------------------------------------------------------------
    typedef std::allocator<T> Allocator;
    //-------------------------------------------------------------------------
    typedef MemoryPolicyBase<T, std::size_t, _NumBlocks, _NumChannels> Super;
protected:
    //-------------------------------------------------------------------------
    T *__buffer_[NumChannels];
public:
    //-------------------------------------------------------------------------
    DefaultMemoryPolicy() {}
    //-------------------------------------------------------------------------
    void deallocate();
    //-------------------------------------------------------------------------
    void allocate(UInteger blockSize);
    //-------------------------------------------------------------------------
    inline T * operator[](UInteger channel) const {
        return __buffer_[channel];
    }
    //-------------------------------------------------------------------------
    void setZero() {
        setZeroImpl(__buffer_);
    }

};
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
template < typename T, int I, int J>
void DefaultMemoryPolicy<T, I, J>::allocate(UInteger blockSize)
{
    Allocator allocator;
    allocateImpl(allocator, __buffer_, blockSize);
    Super::setBlockSize(blockSize);
    setZero();
}
//-----------------------------------------------------------------------------
template < typename T, int I, int J >
void DefaultMemoryPolicy<T, I, J>::deallocate()
{
    Allocator allocator;
    deallocateImpl(allocator, __buffer_, Super::getSize());
    Super::setBlockSize(0);
}

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
    template <typename, int, int> class _MemoryPolicy = DefaultMemoryPolicy
>
class AsyncBuffer : public _MemoryPolicy<T, _NumBlocks, _NumChannels>
{
//=============================================================================
BOOST_STATIC_ASSERT(_NumBlocks>=2);
BOOST_STATIC_ASSERT(_NumChannels>=1);
public:
    //-------------------------------------------------------------------------
    typedef T ValueType;
    //-------------------------------------------------------------------------
    typedef _MemoryPolicy<T, _NumBlocks, _NumChannels> MemoryPolicy;
    //-------------------------------------------------------------------------
    static const UInteger InvalidSamplePos =
        boost::integer_traits<UInteger>::const_max;
    //-------------------------------------------------------------------------
    static const UInteger UndefinedNumBlocks =
        boost::integer_traits<UInteger>::const_max;;
protected:
    //-------------------------------------------------------------------------
    UInteger blocksWritten;
    //-------------------------------------------------------------------------
    inline UInteger cyc_bounds( UInteger i ) { return i%getSize(); }
    //-------------------------------------------------------------------------
    inline Integer getLastWrittenBlockNumber() const {
        if (blocksWritten==0) {
            return -1;
        }
        Integer x=toBufferBlocks(blocksWritten);
        if (x==0) {
            return MemoryPolicy::NumBlocks-1;
        };
        return x-1;
    }
    //-------------------------------------------------------------------------
    /**
     * offset to lastwritten block
     */
    inline Integer blockOffset(UInteger readBlocks) const {
        return readBlocks-blocksWritten+1;
    }
    //-------------------------------------------------------------------------
    /**
     * concernes the buffer bounds
     */
    inline Integer toBufferBlocks(UInteger blocks) const {
        return blocks%MemoryPolicy::NumBlocks;
    }
    //-------------------------------------------------------------------------
    inline Integer getCurrentBlockNumber() const {
        return toBufferBlocks(blocksWritten);
    }
    //-------------------------------------------------------------------------
    inline UInteger toSamplePos(Integer numBlocks) const {
        if (numBlocks<0 || numBlocks>=MemoryPolicy::NumBlocks) {
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
    inline Integer missingBlocks(UInteger readBlocks) const {
        Integer bo=blockOffset(readBlocks);
        if (bo>=0) {
            return bo;
        }
        bo+=MemoryPolicy::NumBlocks-1;
        if (bo<0) {
            return bo;
        }
        return 0;
    }
    //-------------------------------------------------------------------------
    ~AsyncBuffer();
    //-------------------------------------------------------------------------
    AsyncBuffer();
    //-------------------------------------------------------------------------
    template <typename U>
    void writeBlock(U **data);
    //-------------------------------------------------------------------------
    /**
     * @return the number of samples of one block
     */
    inline UInteger getBlockSize() const {
        return MemoryPolicy::getBlockSize();
    }
    //-------------------------------------------------------------------------
    /**
     * @return the number of samples of the whole buffer = blockSize*NumBlocks 
     */
    inline UInteger getSize() const {
        return MemoryPolicy::getSize();
    }
    //-------------------------------------------------------------------------
    inline UInteger getNumChannels() const {
        return MemoryPolicy::NumChannels;
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
    int readBlock(U **out, UInteger &blocksRead) const;
}; // AsyncBuffer
///////////////////////////////////////////////////////////////////////////////
template < typename T, int I, int J, template <class, int, int> class A >
AsyncBuffer<T, I, J, A>::AsyncBuffer() :
    blocksWritten(0)
{
}
//-----------------------------------------------------------------------------
template < typename T, int I, int J, template <class, int, int> class A >
AsyncBuffer<T, I, J, A>::~AsyncBuffer()
{
    MemoryPolicy::deallocate();
}
//-----------------------------------------------------------------------------
template < typename T, int I, int J, template <class, int, int> class A >
template < typename U>
void AsyncBuffer<T, I, J, A>::writeBlock(U **data)
{    
    for (UInteger i=0; i<getNumChannels(); ++i) {
        T *ptr = (*this)[i];
        ptr+=toSamplePos(getCurrentBlockNumber());
        MemoryPolicy::copy(data[i], ptr, getBlockSize());
    }
    ++blocksWritten;
}
//-----------------------------------------------------------------------------
template < typename T, int I, int J, template <class, int, int> class A >
template < typename U>
int
AsyncBuffer<T, I, J, A>::readBlock(U **out, UInteger &blocksRead) const
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
    for (UInteger i=0; i<getNumChannels(); ++i) {
        T *ptr = (*this)[i];
        ptr+=toSamplePos(toBufferBlocks(blocksRead));
        copy(ptr, out[i], getBlockSize());
    }
    ++blocksRead;
    return 0;
}
}} // namespace(s)


#endif /* SAMBAG_ASYNCBUFFER_H */
