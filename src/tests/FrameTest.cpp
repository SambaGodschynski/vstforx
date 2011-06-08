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
void fillFrame ( com::Frame *f, float left, float right ) {
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
//=============================================================================
struct ProcessException {
	std::string text;
	ProcessException ( const std::string &text ) : text(text) {}
};
//=============================================================================
void processGraph ( processing::Graph *graph, 
				    int blockSize, 
				    float inLeft, 
					float inRight,
					float expectedLeft,
					float expectedRight,
					size_t repeat = 1000 ) 
//=============================================================================
{
	if ( !graph->isActive() ) 
		throw ProcessException("graph inactive.");
	com::Frame fIn ( blockSize );
	com::Frame fOut ( blockSize );
	fillFrame ( &fIn, inLeft, inRight );
	while ( repeat-- > 0 ) {
		fillFrame ( &fIn, inLeft, inRight );
		graph->pushAndCopy ( &fIn, blockSize );
		graph->processGraph( fOut.getData(), blockSize  );
		float res = isFilledWith<float>( fOut[0], blockSize, expectedLeft );
		if ( expectedLeft != res )
			throw ProcessException ( com::MyString(res) + "(res.) !=  (exp.)" + com::MyString(expectedLeft) );
		
		res = isFilledWith<float>( fOut[1], blockSize, expectedRight );
		if ( expectedRight != res )
			throw ProcessException ( com::MyString(res) + "(res.) !=  (exp.)" + com::MyString(expectedRight) );
	}
}
} // anonymous namespace 

namespace tests {
//=============================================================================
processing::Graph::Ptr FrameTest::createGraph( int blockSize, float samplerate ) 
{
//=============================================================================
	using namespace processing;
	Graph::Ptr graph = Graph::create ( dummyFX );
	Graph::Janitor::Ptr janitor = graph->getJanitor();
	dummyFX->setSampleRate ( samplerate );
	dummyFX->setBlockSize ( blockSize );
	janitor->hostInfoChanged();
	return graph;
}
//=============================================================================
FrameTest::FrameTest() {
//=============================================================================
	dummyFX = new processing::DummyFX ( NULL );
}
//=============================================================================
FrameTest::~FrameTest() {
//=============================================================================
	delete dummyFX;
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
	Frame fr;
	CPPUNIT_ASSERT ( fr.getSize() == 0 );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> with given size + assign op.
	fr = Frame( 512 );
	CPPUNIT_ASSERT_EQUAL ( (Frame::Int)512, fr.getSize() );
	FILL_AND_CHECK (fr, 1.0f);
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> with extern data
	Frame fr2 ( fr.getData(), fr.getSize()/2 );
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
	Frame fr;
	CPPUNIT_ASSERT ( fr.getSize() == 0 );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> with given size + assign op.
	fr = Frame( 512 );
	CPPUNIT_ASSERT_EQUAL ( (Frame::Int)512, fr.getSize() );
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
} // namespace tests
