/*
 * ===========================================================================================================
 * FrameTest.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */

#include <cppunit/config/SourcePrefix.h>
#include "FrameTest.hpp"
#include "com/MyString.h"
#include "processing/processing.h"
#include "processing/ConcreteProcessAdapter.h"
#include "com/one4All.h"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( tests::FrameTest );

namespace {
//=============================================================================
void fillFrame ( processing::Frames *f, float left, float right ) {
//=============================================================================
	using namespace com;
	for ( size_t i = 0; i<f->getSize(); ++i ) {
		(*f)[0][i] = left;
		(*f)[1][i] = right;
	}
}
//=============================================================================
// liefert v wenn data nur mit v gefuellt ist.
// liefert ansonsten den ersten wert der nicht v entspricht.
template <typename T>
T isFilledWith (  T *data, size_t num,  T v ) {
//=============================================================================
	for ( size_t i=0; i<num; ++i ) {
		if ( data[i] != v ) return data[i];
	}
	return v;
}
}

namespace tests {
//=============================================================================
FrameTest::FrameTest() {
//=============================================================================
}
//=============================================================================
FrameTest::~FrameTest() {
//=============================================================================
}

#define FILL_AND_CHECK(frame, value) \
	(frame).setZero( (frame).getSize() );	\
	fillFrame ( &(frame), (value), (-value) ); \
	CPPUNIT_ASSERT_EQUAL ( (float)(value), isFilledWith<float>( (frame)[0], (frame).getSize(), (value) ) ); \
	CPPUNIT_ASSERT_EQUAL ( (float)(-value), isFilledWith<float>( (frame)[1], (frame).getSize(), (-value) ) ); \
	

//=============================================================================
void FrameTest::testConstructor() {
//=============================================================================
	using namespace std;
	using namespace com;
	using namespace processing;
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> std constr.
	Frames fr;
	CPPUNIT_ASSERT ( fr.getSize() == 0 );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> with given size + assign op.
	fr = Frames( 512 );
	CPPUNIT_ASSERT_EQUAL ( (Frames::Int)512, fr.getSize() );
	FILL_AND_CHECK (fr, 1.0f);
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> with extern data
	Frames fr2 ( fr.getData(), fr.getSize()/2 );
	CPPUNIT_ASSERT ( isFilledWith<float>( fr2[0], fr2.getSize(),  1.0 ) ); 
	CPPUNIT_ASSERT ( isFilledWith<float>( fr2[0], fr2.getSize(), -1.0 ) ); 
}
//=============================================================================
void FrameTest::testResize() {
//=============================================================================
	using namespace std;
	using namespace com;
	using namespace processing;
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> std constr.
	Frames fr;
	CPPUNIT_ASSERT ( fr.getSize() == 0 );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> with given size + assign op.
	fr = Frames( 512 );
	CPPUNIT_ASSERT_EQUAL ( (Frames::Int)512, fr.getSize() );
	FILL_AND_CHECK (fr, 1.0f);
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> resize
	fr.setSize( 1034 );
	FILL_AND_CHECK (fr, 1.0f);
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> set zero
	// previous bug: occurs memory leak. caused by skipping realloc with
	// "if size==0 return"
	fr.setSize( 0 );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> resize
	fr.setSize( 1011 );
	FILL_AND_CHECK (fr, 1.0f);
	
}
//=============================================================================
void FrameTest::testDCStreamReadWrite() {
//=============================================================================
	using namespace std;
	using namespace com;
	using namespace processing;
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> blocksize_read == blocksize_write
	DCStream s(10, 10);
	Frames f( 10 );
	fillFrame(&f, 1.f, -1.f);
	s.addFrame(&f, 10, 0);
	s.addFrame(&f, 10, 0);
	CPPUNIT_ASSERT_EQUAL(1.f, isFilledWith(s.getBuffer()[0], 20, 1.f));
	CPPUNIT_ASSERT_EQUAL(-1.f, isFilledWith(s.getBuffer()[1], 20, -1.f));
	
}
} // namespace tests
