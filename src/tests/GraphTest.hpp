
#ifndef GRAPH_TEST_H
#define GRAPH_TEST_H

#include <cppunit/extensions/HelperMacros.h>
#include "audioeffectx.h"
#include "com/PPIError.h"
#include "processing/processing.h"
#include "processing/Graph.h"
#include "processing/ConcreteProcessAdapter.h"
#include "processing/IHostInfo.h"
#include "DummyFX.h"
#include <sambag/com/UnitEx.hpp>

namespace tests {
//=============================================================================
// extra Volume: bruch NUMERATOR/DENOMINATOR == initvalue.
// wird fuer Creator Klassen benoetig.
template< int NUMERATOR, int DENOMINATOR >
class VolumeAdapterX : public processing::Volume {
//============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<VolumeAdapterX> Ptr;
protected:
	//-------------------------------------------------------------------------
	VolumeAdapterX( frx::processing::IHostInfo::Ptr g ) : 
		 Volume( g, NUMERATOR / (float)DENOMINATOR ) {}
public:
	//-------------------------------------------------------------------------
	static Ptr create(  frx::processing::IHostInfo::Ptr g ) {
		Ptr neu( new VolumeAdapterX(g) );
		neu->self = neu;
		return neu;
	}
};
//=============================================================================
//=============================================================================
class GraphTest : public CPPUNIT_NS::TestFixture {
//=============================================================================
	CPPUNIT_TEST_SUITE( GraphTest );
	CPPUNIT_TEST( testConstructor );
        CPPUNIT_TEST( testIssue441 );
	CPPUNIT_TEST( testAddRemoveNodes );
	CPPUNIT_TEST( testConnectNodes );
	CPPUNIT_TEST( testSignalProcessPath );
	CPPUNIT_TEST( testDelayAdapter );
	CPPUNIT_TEST( testDelayCompensationSimple );
	CPPUNIT_TEST( testDelayCompensationTree );
	CPPUNIT_TEST( testDelayCompensationComplex1 );
	CPPUNIT_TEST( testDCWithInputSwitch );
	CPPUNIT_TEST( testDCWithInputSwitch02 );
	CPPUNIT_TEST( testGraphConsistency );
	CPPUNIT_TEST( testJanitorCreate );
	CPPUNIT_TEST( testGraphSeries );
	CPPUNIT_TEST( testGraphParallel );
	CPPUNIT_TEST( testGraphComplex1 );
	CPPUNIT_TEST( testGraphComplex2 );
	CPPUNIT_TEST( testGraphComplex3 );
    CPPUNIT_TEST_KNOWN_ISSUE( testGraphIdleHandler );
	CPPUNIT_TEST_EXCEPTION( testJanitorLock, com::ppiError::DeadlockException );
	CPPUNIT_TEST_SUITE_END();
private:
	processing::DummyFX::Ptr dummyFX;
	processing::Graph::Ptr createGraph( int blockSize, float samplerate );
public:
	GraphTest();
	~GraphTest();
	void setUp(){}
	void tearDown(){}
	void testConstructor();
	void testAddRemoveNodes();
	void testSignalProcessPath();
	void testConnectNodes();
	void testDelayCompensationSimple();
	void testDCWithInputSwitch();
	void testDCWithInputSwitch02();
	void testDelayCompensationTree();
	void testDelayCompensationComplex1();
	void testDelayAdapter();
	void testJanitorCreate();
	void testJanitorLock();
	//void testGraphTestBuilder(); wird von testGraphSeries/testGraphParallel/testGraphComplex mitgetestet
	void testGraphSeries();
	void testGraphParallel();
	void testGraphConsistency(); // mehrfach verkuepfungen / feedback / etc...
	void testReleaseGraphObjects();
	void testGraphComplex1();
	void testGraphComplex2();
	void testGraphComplex3();
    void testGraphIdleHandler();
    void testIssue441();
};
} // namespace tests
#endif 
