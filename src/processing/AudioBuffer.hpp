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
#include <memory>

namespace {
	template <int I>
	struct Int2Type {
		enum { Value = I };
	};
	template <class Buffer, class _Int2Type>
	void _copyIn( Buffer &buffer, 
			 typename Buffer::value_type **data, 
			 typename Buffer::size_type num,
			 _Int2Type)
	{
		enum { I = _Int2Type::Value };
		for (typename Buffer::size_type j=0; j<num; ++j) {
			buffer.push_back( data[I][j] );
		}
		_copyIn<Buffer>(buffer, data, num, Int2Type<_Int2Type::Value-1>());
	}
	template <class Buffer>
	void _copyIn( Buffer &buffer, 
			 typename Buffer::value_type **data, 
			 typename Buffer::size_type num,
			 Int2Type<-1>)
	{
	}
	template <class Buffer, class _Int2Type>
	void _copyOut( Buffer &buffer, 
			 typename Buffer::value_type **data, 
			 typename Buffer::size_type num,
			 _Int2Type)
	{
		enum { I = _Int2Type::Value };
		for (typename Buffer::size_type j=0; j<num; ++j) {
			data[I][j] = buffer.front();
			buffer.pop_front();
		}
		_copyOut<Buffer>(buffer, data, num, Int2Type<_Int2Type::Value-1>());
	}
	template <class Buffer>
	void _copyOut( Buffer &buffer, 
			 typename Buffer::value_type **data, 
			 typename Buffer::size_type num,
			 Int2Type<-1>)
	{
	}
} // namespace(s)

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
	Buffer buffer;
public:
	//-------------------------------------------------------------------------
	AudioBuffer(typename SizeType blockSize=0, Allocator &alloc = Allocator() );
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
		return buffer.size() / NumChannels;
	}
	//-------------------------------------------------------------------------
	void writeIn(ValueType **data, SizeType numSamples);
	//-------------------------------------------------------------------------
	void readOut(ValueType **data, SizeType numSamples);
	//-------------------------------------------------------------------------
	bool isEmpty() const {
		return buffer.empty();
	}
}; // AudioBuffer
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
template < typename T, 
	int NC, 
	template <class> class A
>
AudioBuffer<T, NC, A>::AudioBuffer(SizeType blockSize, A<T> &alloc) :
	blockSize(blockSize),
	buffer(alloc)
{
}
//-----------------------------------------------------------------------------
template < typename T, 
	int NC, 
	template <class> class A
>
void AudioBuffer<T, NC, A>::setBlockSize(SizeType bs) 
{
	blockSize = bs;
	SizeType minCap = bs*NumChannels;
	buffer.set_capacity( Buffer::capacity_type(bs, bs) );
}
//-----------------------------------------------------------------------------
template < typename T, 
	int NC, 
	template <class> class A
>
void AudioBuffer<T, NC, A>::writeIn(T **data, SizeType numSamples) 
{
	buffer.resize( numSamples*NumChannels, T() );
	_copyIn<Buffer>(buffer, data, numSamples, Int2Type<NumChannels-1>());
}
//-----------------------------------------------------------------------------
template < typename T, 
	int NC, 
	template <class> class A
>
void AudioBuffer<T, NC, A>::readOut(T **data, SizeType numSamples) 
{
	_copyOut<Buffer>(buffer, data, numSamples, Int2Type<NumChannels-1>());
}
}} // namespace(s)

#endif /* SAMBAG_AUDIOBUFFER_H */
