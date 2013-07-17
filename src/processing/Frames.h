/*
 * ===========================================================================================================
 * Frames.h
 *      Author: Johannes Unger
 * ===========================================================================================================
 */

#ifndef _FRAMES_H_
#define _FRAMES_H_

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <vector>
#include <boost/shared_ptr.hpp>

// TODO: durch ErrorKlassen ersetzen 
#define SIZE_ERROR "size_error"
#define RANGE_ERROR "range_error"
#define INVALID_MEM_ALLOC "invalid_mem_alloc"

#ifdef _FORX_TESTSUITE
#define ONLY_FOR_FORX_TEST(expr) expr
#else
#define ONLY_FOR_FORX_TEST(expr)
#endif

namespace processing {
//============================================================================================================
// Klasse Frames:
//============================================================================================================
class Frames {
friend class DCStream;
public:
	ONLY_FOR_FORX_TEST( 
		static size_t num_copyintos;
	)
	//--------------------------------------------------------------------------------------------------------
	typedef float T;
	//--------------------------------------------------------------------------------------------------------
	typedef size_t Int;
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<Frames> Ptr;
	//--------------------------------------------------------------------------------------------------------
	static const Int CHANNELS = 2;
private:
	//--------------------------------------------------------------------------------------------------------
	bool externData;
	//--------------------------------------------------------------------------------------------------------
	T *data[CHANNELS];
	//--------------------------------------------------------------------------------------------------------
	Int ___size_;
	//--------------------------------------------------------------------------------------------------------
	void allocData ( Int size ) {
		if (externData)
			throw INVALID_MEM_ALLOC;
		___size_ = size; 
		if ( size==0 ) return;
		for ( Int i=0; i<CHANNELS; ++i ) data[i] = new T[size];
		
	}
	//--------------------------------------------------------------------------------------------------------
	void reAlloc ( Int size ) {
		if (externData)
			throw INVALID_MEM_ALLOC;
		if ( getSize()==0 ) {
			allocData ( size );
			___size_ = size;
			return;
		}
		___size_ = size;
		for ( Int i=0; i<CHANNELS; ++i ) {
            delete[] data[i];
			data[i] = new T[size]; 
		}
	}
public:
	//--------------------------------------------------------------------------------------------------------
	void setSize ( Int size ) {
		if ( getSize()!=size ) {
			reAlloc ( size );
		}
	}
	//--------------------------------------------------------------------------------------------------------
	void applySize ( const Frames & f ) {
		if ( getSize() == f.getSize() ) return;
		reAlloc ( f.getSize() );
	}
	//--------------------------------------------------------------------------------------------------------
	T ** getData() { return (T**)&data; }
	//--------------------------------------------------------------------------------------------------------
	const T & getValue ( Int i, Int channel ) const { return data[channel][i]; } 
	//--------------------------------------------------------------------------------------------------------
	void setValue ( Int i, Int channel, const T &val ) { data[channel][i] = val; } 
	//--------------------------------------------------------------------------------------------------------
	Int getSize () const { return ___size_; }
	//--------------------------------------------------------------------------------------------------------
	Int getNumChannels() const { return CHANNELS; }
	//--------------------------------------------------------------------------------------------------------
	const T * operator[] ( Int channel ) const {
		return &data[channel][0];
	}
	//--------------------------------------------------------------------------------------------------------
	T * operator[] ( Int channel ) {
		return &data[channel][0];
	}
	//--------------------------------------------------------------------------------------------------------
	Frames( Int size=0 ) : ___size_(size), externData(false)
	{
		allocData ( ___size_ );
	}
	//--------------------------------------------------------------------------------------------------------
	Frames( const Frames & f ) : ___size_( f.getSize() ), externData(false)
	{
		allocData ( ___size_ );
		for ( Int i=0; i<CHANNELS; ++i ) {
			memcpy ( data[i], f.data[i], sizeof (T) * f.getSize() );
		}
	}
	//--------------------------------------------------------------------------------------------------------
	const Frames & operator = ( const Frames &f ) {
		setSize ( f.getSize() );
		for ( Int i=0; i<CHANNELS; ++i ) {
			memcpy ( data[i], f.data[i], sizeof (T) * f.getSize() );
		}
		return *this;
	}
	//--------------------------------------------------------------------------------------------------------
	Frames( T **data, Int size  ) : ___size_(size), externData(true) {
		for ( Int i=0; i<CHANNELS; ++i ) Frames::data[i] = data[i];
	}
	//--------------------------------------------------------------------------------------------------------
	void copyIntoFrom ( const Frames &f, Int numSamples  ) {

		ONLY_FOR_FORX_TEST(num_copyintos++;)

		if ( getSize() != f.getSize() ) {
			reAlloc ( f.getSize() );
		}
		for ( Int i=0; i<CHANNELS; ++i ) {
			memcpy ( data[i], f.data[i], sizeof (T) * numSamples );
			// den rest auf 0 setzen
			memset ( &data[i][numSamples], 0, sizeof (T) * ( getSize() - numSamples ) );
		}
	}
	//--------------------------------------------------------------------------------------------------------
	void setZero ( Int numSamples ) {
		for ( Int i=0; i<CHANNELS; ++i ) {
			memset( &data[i][0], 0, sizeof(T) * numSamples );
		}
	}
	//--------------------------------------------------------------------------------------------------------
	void setBlock ( T **_data, Int numSamples ) {
		for ( Int i=0; i<CHANNELS; ++i ) {
			memcpy ( &data[i][0], _data[i], sizeof(T) * numSamples );
			// den rest auf 0 setzen
			memset ( &data[i][numSamples], 0, sizeof (T) * ( getSize() - numSamples ) );
		}
	}
	//--------------------------------------------------------------------------------------------------------
	void getBlock ( T **dst, Int numSamples ) {
		for ( Int i=0; i<CHANNELS; ++i ) {
			memcpy ( dst[i], &data[i][0], sizeof(T) * numSamples );
		}
	}
	//--------------------------------------------------------------------------------------------------------
	virtual ~Frames() {
		if ( externData ) return;
		if ( getSize()==0 ) return; 
		for ( Int i=0; i<CHANNELS; ++i ) delete[] data[i];
	}
	//--------------------------------------------------------------------------------------------------------
	Frames & add ( Frames &b, Int numSamples ) {
		for ( Int i=0; i<numSamples; ++ i ) {
			for ( Int j=0; j<CHANNELS; ++j ) {
				(*this)[j][i]+=b[j][i];
			}
		}
		return *this;
	}
	//--------------------------------------------------------------------------------------------------------
	Frames & mul ( Frames &b, Int numSamples ) {
		for ( Int i=0; i<numSamples; ++ i ) {
			for ( Int j=0; j<CHANNELS; ++j ) {
				(*this)[j][i]*=b[j][i];
			}
		}
		return *this;
	}
	//--------------------------------------------------------------------------------------------------------
	Frames & mul ( const T &v, Int numSamples ) {
		for ( Int i=0; i<numSamples; ++ i ) {
			for ( Int j=0; j<CHANNELS; ++j ) {
				(*this)[j][i]*=v;
			}
		}
		return *this;
	}
	//--------------------------------------------------------------------------------------------------------
	void mixAllToMono( Int numSamples ) {
		for ( Int i=0; i<numSamples; ++ i ) {
			for ( Int j=1; j<CHANNELS; ++j ) {
				(*this)[0][i] += (*this)[j][i] / (T)CHANNELS;
			}
		}
	}
	//--------------------------------------------------------------------------------------------------------
	void mixMonoToAll( Int numSamples ) {
		for ( Int i=0; i<numSamples; ++ i ) {
			for ( Int j=1; j<CHANNELS; ++j ) {
				(*this)[j][i] = (*this)[0][i];
			}
		}
	}
};
//============================================================================================================
// Klasse DCStream: Delay Compensations Stream
//   
//   die buffergroesse resultiert aus frameSize + max. delay  
//
//     +--------------------------+------------+
//     |      frameSize           | max.delay  |
//     |______=====frameMitDelay==|            |
//     |      ====frameOhneDelay==+======      |                                                  
//     +--------------------------+------------+
//    
//  - buffer wird zyklisch gelesen und geschrieben
//  - beim schreiben widr strom addiert.
//  - ist lesevorgang abgeschlossen wird buffer auf 0 gesetzt und 
//    startpos+=frameSize
//============================================================================================================
class DCStream {
public:
	//--------------------------------------------------------------------------------------------------------
	typedef Frames::T T;
	//--------------------------------------------------------------------------------------------------------
	typedef Frames::Int Int;
private:
	//--------------------------------------------------------------------------------------------------------
	Int norm( Int i ) { return i%getBufferSize(); }
	//--------------------------------------------------------------------------------------------------------
	static const Int CHANNELS = Frames::CHANNELS;
	//--------------------------------------------------------------------------------------------------------
	Int frameSize, maxDelay, cursor;
	//--------------------------------------------------------------------------------------------------------
	T **buff;
	//--------------------------------------------------------------------------------------------------------
	void zeroBuff() {
		for ( Int j=0; j<getBufferSize(); ++j ) {
			for ( Int i=0; i<CHANNELS; ++i ) {
				buff[i][j] = 0;
			}
		}
	}
	//--------------------------------------------------------------------------------------------------------
	void alloc ( Int size ) {
		if ( size == 0 && buff!=NULL ) {
			releaseBuffer();
			return;
		}
		buff = new T*[CHANNELS];
		for ( Int i=0; i<CHANNELS; ++i ) buff[i] = new T[size];
		
	}
	//--------------------------------------------------------------------------------------------------------
	void reAlloc ( Int size ) {
		if ( !buff ) {
			alloc ( size );
			return;
		}
		if ( size == 0 ) {
			releaseBuffer();
			return;
		}
		for ( Int i=0; i<CHANNELS; ++i ) {
			buff[i] = (T*) realloc ( buff[i], sizeof (T)*size );
		}
	}
	//--------------------------------------------------------------------------------------------------------
	void incrCursor ( Int numSamples ) {
		cursor = ( cursor + numSamples ) % getBufferSize(); 
	}
	//--------------------------------------------------------------------------------------------------------
	void decrCursor ( Int numSamples ) {
		cursor = ( cursor - numSamples ) % getBufferSize(); 
	}
	//--------------------------------------------------------------------------------------------------------
	void releaseBuffer() { 
		if ( !buff ) return;
		for ( Int i=0; i<CHANNELS; ++i ) delete[] buff [i];
		delete[] buff ;
		buff = NULL;
	}
public:
	//--------------------------------------------------------------------------------------------------------
	Int getBufferSize() const { return frameSize + maxDelay; }
	//--------------------------------------------------------------------------------------------------------
	Int getMaxDelay() const { return maxDelay; }
	//--------------------------------------------------------------------------------------------------------
	void setSize ( Int frameSize, Int maxDelay ) {
		if ( getBufferSize() != frameSize + maxDelay ) reAlloc ( frameSize + maxDelay );
		DCStream::frameSize = frameSize;
		DCStream::maxDelay = maxDelay;
		if ( buff ) zeroBuff();
	}
	//--------------------------------------------------------------------------------------------------------
	void setMaxDelay(Int maxDelay) {
		setSize(frameSize, maxDelay);
	}
	//--------------------------------------------------------------------------------------------------------
	T ** getBuffer() { return buff; }
	//--------------------------------------------------------------------------------------------------------
	DCStream ( Int frameSize = 0, Int maxDelay = 0 ) : frameSize(frameSize), maxDelay(maxDelay), cursor(0) {
		buff = NULL;
		alloc ( frameSize + maxDelay );
		if ( buff ) zeroBuff();
	}
	//--------------------------------------------------------------------------------------------------------
	virtual ~DCStream () {
		releaseBuffer();
	}
	//--------------------------------------------------------------------------------------------------------
	Int getNumChannels() const { return CHANNELS; }
	//--------------------------------------------------------------------------------------------------------
	void addFrame ( Frames *frames, Int numSamples, Int delay ) {
		assert ( delay <= maxDelay );
		Int s = cursor + delay;
		Int e = cursor + numSamples + delay;
		Int c = 0;
		Int n = 0;
		for ( Int i=s; i<e; ++i ) {
			n = norm(i);
			for ( Int j=0; j<CHANNELS; ++j ) {
				buff[j][n] += (*frames)[j][c];
			}
			++c;
		}
	}
	//--------------------------------------------------------------------------------------------------------
	void flush( Int numSamples, T **data = NULL ) {
		Int s = cursor;
		Int e = cursor + numSamples;
		Int c = 0;
		Int n = 0;
		for ( Int i=s; i<e; ++i ) {
			n = norm(i);
			for ( Int j=0; j<CHANNELS; ++j ) {
				if ( data ) data[j][c] = buff[j][n];
				buff[j][n] = 0;
			}
			++c;
		}
		incrCursor( numSamples );
	}
};
} // processing
#endif
