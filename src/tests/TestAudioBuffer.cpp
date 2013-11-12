/*
 * TestAudioBuffer.cpp
 *
 *  Created on: Tue Nov 12 10:32:22 2013
 *      Author: Johannes Unger
 */

#include "TestAudioBuffer.hpp"
#include <cppunit/config/SourcePrefix.h>
#include <processing/AudioBuffer.hpp>
#include <processing/Frames.h>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( tests::TestAudioBuffer );

namespace {
//=============================================================================
template <typename T>
void _fill ( T *data, size_t num, T v ) {
//=============================================================================
	for ( size_t i=0; i<num; ++i ) {
		data[i]=v;
	}
}
//=============================================================================
// liefert v wenn data nur mit v gefuellt ist.
// liefert ansonsten den ersten wert der nicht v entspricht.
template <typename T>
T _isFilledWith (  T *data, size_t num,  T v ) {
//=============================================================================
	for ( size_t i=0; i<num; ++i ) {
		if ( data[i] != v ) return data[i];
	}
	return v;
}
//=============================================================================
template <typename T>
void _row ( T *data, size_t num, T s ) {
//=============================================================================
	for ( size_t i=0; i<num; ++i ) {
		data[i]=s++;
	}
}
//=============================================================================
template <typename T>
bool _isRow (  T *data, size_t num,  T s ) {
//=============================================================================
	for ( size_t i=0; i<num; ++i ) {
		if ( data[i] != s++ ) { 
			return false;
		}
	}
	return true;
}
}

namespace tests {
//=============================================================================
//  Class TestAudioBuffer
/** Motivation:
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
//=============================================================================
//-----------------------------------------------------------------------------
void TestAudioBuffer::testReadWrite() {
	using namespace frx::processing;
	using namespace processing;
	typedef AudioBuffer<float, 2> ABff;
    //prepare containers
	ABff abff;
	abff.setBlockSize(512);
	CPPUNIT_ASSERT( abff.isEmpty() );
	CPPUNIT_ASSERT_EQUAL((size_t)512, abff.getBlockSize() );
	Frames a(512), b(512);
	float raw_l[512], raw_r[512];
	float *raw[2] = {&raw_r[0], &raw_l[0]};
	//abff<<a
	_fill(a[0], 512, 1.f);
	_fill(a[1], 512, -1.f);
	abff.writeIn(a.getData(), 512);
	CPPUNIT_ASSERT( !abff.isEmpty() );
	//abff>>b
	abff.readOut(b.getData(), 512);
	CPPUNIT_ASSERT( abff.isEmpty() );
	//a==b
	CPPUNIT_ASSERT_EQUAL( 1.f, _isFilledWith(b[0], 512, 1.f) );
	CPPUNIT_ASSERT_EQUAL(-1.f, _isFilledWith(b[1], 512,-1.f) );
	//abff<<a
	CPPUNIT_ASSERT_EQUAL( (size_t)0, abff.size() );
	_row(a[0], 512, 1.f);
	_row(a[1], 512, 10.f);
	abff.writeIn(a.getData(), 512);
	CPPUNIT_ASSERT_EQUAL( (size_t)512, abff.size() );
	// abff>>ABff::Raw(&raw, 256);  // different blocksizes!
	abff.readOut(&raw[0], 256);
	CPPUNIT_ASSERT_EQUAL( (size_t)256, abff.size() );
	// abff>>ABff::Raw(&raw, 250)>>ABff::Raw(&raw, 6);
	{
		size_t i = 256;
		float *raw[2] = {&raw_r[i], &raw_l[i]};
		abff.readOut(&raw[0], 250);
	}
	CPPUNIT_ASSERT_EQUAL( (size_t)6, abff.size() );
	{
		size_t i = 506;
		float *raw[2] = {&raw_r[i], &raw_l[i]};
		abff.readOut(&raw[0], 6);
	}
	CPPUNIT_ASSERT_EQUAL( (size_t)0, abff.size() );
	_isRow(raw[0], 512, 1.f);
	_isRow(raw[1], 512, 10.f);
}
} //namespace
