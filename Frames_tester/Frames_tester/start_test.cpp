#include <iostream>
#include "Frame.h"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <assert.h>
#include <boost/thread.hpp> 

using namespace std;
using namespace com;


void printFrame( const Frame &f ) {
	for ( Frame::Int i = 0; i<f.getSize(); ++i ) {
		for ( Frame::Int j = 0; j<f.getNumChannels(); ++j ) {
			cout<<f[j][i]<<" ";
		}
		cout<<endl;
	}
}

void printStream( DCStream &f ) {
	for ( Frame::Int i = 0; i<f.getBufferSize(); ++i ) {
		for ( Frame::Int j = 0; j<f.getNumChannels(); ++j ) {
			cout<<f.getBuffer()[j][i]<<" ";
		}
		cout<<endl;
	}
}

void testFrame01() {
	cout<<endl<<"testframe01"<<endl;
	Frame fr (10);
	assert ( fr.getSize() == 10 );
	fr.fillWith ( -1 );
	fr[0][0] = 1.0f;
	fr[1][0] = 1.0f;
	printFrame( fr );
	for ( Frame::Int i=0; i<fr.getSize(); ++i ) {
		for ( Frame::Int j=0; j<fr.CHANNELS; ++j ) {
			if ( i == 0 ) assert ( fr[j][i] == 1.0f );
			else assert  ( fr[j][i] == -1.0f );
		}
	}
}
void testFrame02() {
	cout<<endl<<"testframe02"<<endl;
	Frame fr01 (10);
	fr01.fillWith ( 1 );
	Frame fr02;
	fr02 = fr01;
	assert ( fr01.getSize() == fr02.getSize() );
	assert ( fr01[0] != fr02[0] );
	fr02+=fr01;
	printFrame(fr02);
	for ( Frame::Int i=0; i<fr01.getSize(); ++i ) {
		for ( Frame::Int j=0; j<fr01.CHANNELS; ++j ) {
			assert  ( fr02[j][i] == 2.0f );
		}
	}
}

void testStream01() {
	cout<<endl<<"testStream01"<<endl;
	
	DCStream stream( 100, 10 );
	assert ( stream.getBufferSize() == 110 );
	
	stream.setSize ( 100, 50 );
	assert ( stream.getBufferSize() == 150 );
	
	stream.setSize ( 0, 0 );
	assert ( stream.getBufferSize() == 0 );
	
	stream.setSize ( 10, 0 );
	assert ( stream.getBufferSize() == 10 );
	printStream ( stream );
	
	stream.setSize ( 1000, 100 );
	assert ( stream.getBufferSize() == 1100 );
}

void testStream02() {
	cout<<endl<<"testStream02"<<endl;
	Frame::Int frSize = 10;
	Frame::Int mDelay = 5;
	DCStream stream( frSize, mDelay );
	Frame a( frSize );
	float v[] = { 1, 2 };
	a.fillChannelsWith ( v );
	stream.addFrame ( &a, 0 );
	stream.addFrame ( &a, 1 );
	/*
	Frame b( frSize );
	stream.flush ( b.getData() );
	stream.addFrame (&a, 0);
	stream.flush ( b.getData() );
	stream.addFrame (&a, 0);
	printFrame (b);
	//printStream ( stream );*/
	
}
boost::timed_mutex mutex;

void testLock() {
	boost::unique_lock<boost::timed_mutex> lock(mutex, boost::try_to_lock); 
    if (!lock.owns_lock()) 
      lock.timed_lock(boost::get_system_time() + boost::posix_time::seconds(5));
	if ( !lock.owns_lock() ) cout << "lock failed" <<P<<endl;
	else cout<<"lock succeed"<<endl;
}

int main ( int argc, char **argv ) {
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF ); //VS memory tracking
	testFrame01();
	testFrame02();
	//testStream01();
	//testStream02();
	mutex.lock();
	testLock();
	cout<<"--------------------------"<<endl<<"mein held;"<<endl;
	char bff[10];
	scanf( &bff[0] );
}