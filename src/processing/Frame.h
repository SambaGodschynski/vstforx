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

#ifdef _FORX_TESTSUITE
#define ONLY_FOR_FORX_TEST(expr) expr
#else
#define ONLY_FOR_FORX_TEST(expr)
#endif

namespace processing {
//============================================================================================================
// Klasse Frame:
//============================================================================================================
class Frame {
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
	typedef boost::shared_ptr<Frame> Ptr;
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
		___size_ = size; 
		if ( size==0 ) return;
		for ( Int i=0; i<CHANNELS; ++i ) data[i] = new T[size];
		
	}
	//--------------------------------------------------------------------------------------------------------
	void reAlloc ( Int size ) {
		if ( getSize()==0 ) {
			allocData ( size );
			___size_ = size;
			return;
		}
		___size_ = size;
		for ( Int i=0; i<CHANNELS; ++i ) {
			data[i] = (T*) realloc ( data[i], sizeof (T)*getSize() );
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
	void applySize ( const Frame & f ) {
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
	Frame( Int size=0 ) : ___size_(size), externData(false)
	{
		allocData ( ___size_ );
	}
	//--------------------------------------------------------------------------------------------------------
	Frame( const Frame & f ) : ___size_( f.getSize() ), externData(false)
	{
		allocData ( ___size_ );
		for ( Int i=0; i<CHANNELS; ++i ) {
			memcpy ( data[i], f.data[i], sizeof (T) * f.getSize() );
		}
	}
	//--------------------------------------------------------------------------------------------------------
	const Frame & operator = ( const Frame &f ) {
		setSize ( f.getSize() );
		for ( Int i=0; i<CHANNELS; ++i ) {
			memcpy ( data[i], f.data[i], sizeof (T) * f.getSize() );
		}
		return *this;
	}
	//--------------------------------------------------------------------------------------------------------
	Frame( T **data, Int size  ) : ___size_(size), externData(true) {
		for ( Int i=0; i<CHANNELS; ++i ) Frame::data[i] = data[i];
	}
	//--------------------------------------------------------------------------------------------------------
	void copyIntoFrom ( const Frame &f, Int sampleFrames  ) {

		ONLY_FOR_FORX_TEST(num_copyintos++;)

		if ( getSize() != f.getSize() ) {
			reAlloc ( f.getSize() );
		}
		for ( Int i=0; i<CHANNELS; ++i ) {
			memcpy ( data[i], f.data[i], sizeof (T) * sampleFrames );
			// den rest auf 0 setzen
			memset ( &data[i][sampleFrames], 0, sizeof (T) * ( getSize() - sampleFrames ) );
		}
	}
	//--------------------------------------------------------------------------------------------------------
	void setZero ( Int sampleFrames ) {
		for ( Int i=0; i<CHANNELS; ++i ) {
			memset( &data[i][0], 0, sizeof(T) * sampleFrames );
		}
	}
	//--------------------------------------------------------------------------------------------------------
	void setBlock ( T **_data, Int sampleFrames ) {
		for ( Int i=0; i<CHANNELS; ++i ) {
			memcpy ( &data[i][0], _data[i], sizeof(T) * sampleFrames );
			// den rest auf 0 setzen
			memset ( &data[i][sampleFrames], 0, sizeof (T) * ( getSize() - sampleFrames ) );
		}
	}
	//--------------------------------------------------------------------------------------------------------
	void getBlock ( T **dst, Int sampleFrames ) {
		for ( Int i=0; i<CHANNELS; ++i ) {
			memcpy ( dst[i], &data[i][0], sizeof(T) * sampleFrames );
		}
	}
	//--------------------------------------------------------------------------------------------------------
	virtual ~Frame() {
		if ( externData ) return;
		if ( getSize()==0 ) return; 
		for ( Int i=0; i<CHANNELS; ++i ) delete[] data[i];
	}
	//--------------------------------------------------------------------------------------------------------
	Frame & add ( Frame &b, Int sampleFrames ) {
		for ( Int i=0; i<sampleFrames; ++ i ) {
			for ( Int j=0; j<CHANNELS; ++j ) {
				(*this)[j][i]+=b[j][i];
			}
		}
		return *this;
	}
	//--------------------------------------------------------------------------------------------------------
	Frame & mul ( Frame &b, Int sampleFrames ) {
		for ( Int i=0; i<sampleFrames; ++ i ) {
			for ( Int j=0; j<CHANNELS; ++j ) {
				(*this)[j][i]*=b[j][i];
			}
		}
		return *this;
	}
	//--------------------------------------------------------------------------------------------------------
	Frame & mul ( const T &v, Int sampleFrames ) {
		for ( Int i=0; i<sampleFrames; ++ i ) {
			for ( Int j=0; j<CHANNELS; ++j ) {
				(*this)[j][i]*=v;
			}
		}
		return *this;
	}
	//--------------------------------------------------------------------------------------------------------
	void mixAllToMono( Int sampleFrames ) {
		for ( Int i=0; i<sampleFrames; ++ i ) {
			for ( Int j=1; j<CHANNELS; ++j ) {
				(*this)[0][i] += (*this)[j][i] / (T)CHANNELS;
			}
		}
	}
	//--------------------------------------------------------------------------------------------------------
	void mixMonoToAll( Int sampleFrames ) {
		for ( Int i=0; i<sampleFrames; ++ i ) {
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
//  - beim schreiben werden stroeme addiert.
//  - ist lesevorgang abgeschlossen wird buffer auf 0 gesetzt und 
//    startpos+=frameSize
//============================================================================================================
class DCStream {
public:
	//--------------------------------------------------------------------------------------------------------
	typedef Frame::T T;
	//--------------------------------------------------------------------------------------------------------
	typedef Frame::Int Int;
private:
	//--------------------------------------------------------------------------------------------------------
	Int norm( Int i ) { return i%getBufferSize(); }
	//--------------------------------------------------------------------------------------------------------
	static const Int CHANNELS = Frame::CHANNELS;
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
	void incrCursor ( Int sampleFrames ) {
		cursor = ( cursor + sampleFrames ) % getBufferSize(); 
	}
	//--------------------------------------------------------------------------------------------------------
	void decrCursor ( Int sampleFrames ) {
		cursor = ( cursor - sampleFrames ) % getBufferSize(); 
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
	void addFrame ( Frame *frame, Int sampleFrames, Int delay ) {
		assert ( delay <= maxDelay );
		Int s = cursor + delay;
		Int e = cursor + sampleFrames + delay;
		Int c = 0;
		Int n = 0;
		for ( Int i=s; i<e; ++i ) {
			n = norm(i);
			for ( Int j=0; j<CHANNELS; ++j ) {
				buff[j][n] += (*frame)[j][c];
			}
			++c;
		}
	}
	//--------------------------------------------------------------------------------------------------------
	void flush( Int sampleFrames, T **data = NULL ) {
		Int s = cursor;
		Int e = cursor + sampleFrames;
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
		incrCursor( sampleFrames );
	}
};
} // processing
#endif
