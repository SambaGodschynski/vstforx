/*
 * AudioBuffer.hpp
 *
 *  Created on: Tue Nov 12 10:31:05 2013
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_AUDIOBUFFER_H
#define SAMBAG_AUDIOBUFFER_H

#include <boost/shared_ptr.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/static_assert.hpp>
#include <memory>
namespace frx { namespace processing {
//=============================================================================
/** 
  * @class AudioBuffer.
  *
  * Motivation:
  *
  * typedef AudioBuffer<float, 2> ABff;
  * ABff abff;
  * Frames a(512), b(512);
  * float raw[512] = {0};
  * abff<<a;
  * abff>>b;
  * abff<<a;
  * abff>>ABff::Raw(&raw, 256);  // different blocksizes!
  * abff>>ABff::Raw(&raw, 250)>>ABff::Raw(&raw, 6);
  */
template <
	typename T, 
	int _NumChannels, 
	template <class> class _Allocator=std::allocator
>
class AudioBuffer {
BOOST_STATIC_ASSERT( _NumChannels > 0 );
//=============================================================================
public:
	//-------------------------------------------------------------------------
	enum { NumChannels = _NumChannels };
	//-------------------------------------------------------------------------
	typedef T ValueType;
	//-------------------------------------------------------------------------
	typedef _Allocator<T> Allocator;
	//-------------------------------------------------------------------------
	typedef typename Allocator::size_type SizeType;
	//-------------------------------------------------------------------------
	typedef boost::circular_buffer_space_optimized<ValueType, Allocator>
			Buffer;
private:
	//-------------------------------------------------------------------------
	SizeType blockSize;
	//-------------------------------------------------------------------------
	Allocator alloc;
	//-------------------------------------------------------------------------
	Buffer buffers[NumChannels];
public:
	//-------------------------------------------------------------------------
	AudioBuffer( SizeType blockSize=0 );
	//-------------------------------------------------------------------------
	/**
	 * @note only for predictive memory allocation.
	 */
	void setBlockSize(SizeType bs);
	//-------------------------------------------------------------------------
	SizeType getBlockSize() const {
		return blockSize;
	}
	//-------------------------------------------------------------------------
	SizeType size() const {
		return buffers[0].size();
	}
	//-------------------------------------------------------------------------
	void writeIn(ValueType **data, SizeType numSamples);
	//-------------------------------------------------------------------------
	void readOut(ValueType **data, SizeType numSamples);
	//-------------------------------------------------------------------------
	bool isEmpty() const {
		return buffers[0].empty();
	}
}; // AudioBuffer
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
template < typename T, 
	int NC, 
	template <class> class A
>
AudioBuffer<T, NC, A>::AudioBuffer(SizeType blockSize) :
	blockSize(0)
{
	setBlockSize(blockSize);
}
//-----------------------------------------------------------------------------
template < typename T, 
	int NC, 
	template <class> class A
>
void AudioBuffer<T, NC, A>::setBlockSize(SizeType bs) 
{
	blockSize = bs;
	for (int i=0; i<NumChannels; ++i) {
		buffers[i].set_capacity( typename Buffer::capacity_type(bs, bs) );
	}
}
//-----------------------------------------------------------------------------
template < typename T, 
	int NC, 
	template <class> class A
>
void AudioBuffer<T, NC, A>::writeIn(T **data, SizeType numSamples) 
{
	SizeType p = size();
	for (SizeType i=0; i<NumChannels; ++i) {
		buffers[i].resize( p + numSamples, T() );
		Buffer::iterator it=buffers[i].begin() + p;
		for (SizeType j=0; j<numSamples; ++j) {
			*(it++) = data[i][j];
		}	
	}
}
//-----------------------------------------------------------------------------
template < typename T, 
	int NC, 
	template <class> class A
>
void AudioBuffer<T, NC, A>::readOut(T **data, SizeType numSamples) 
{
	for (SizeType i=0; i<NumChannels; ++i) {
		Buffer::iterator it=buffers[i].begin();
		for (SizeType j=0; j<numSamples; ++j) {
			data[i][j] = *(it++);
		}	
		buffers[i].erase( buffers[i].begin(), buffers[i].begin()+numSamples );
	}
}
}} // namespace(s)

#endif /* SAMBAG_AUDIOBUFFER_H */
