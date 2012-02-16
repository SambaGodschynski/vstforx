#include <cppunit/config/SourcePrefix.h>
#include "GraphTest.hpp"
#include "com/MyString.h"
#include "processing/processing.h"
#include "processing/ConcreteProcessAdapter.h"
#include "com/one4All.h"
#include "GraphBuilder.hpp"
#include "processing/BglGraph.h"
#include <stack>
#include <boost/assign.hpp>
#include "boost/tuple/tuple.hpp"
#include "DelayAdapter.h"
#include <boost/static_assert.hpp>
#include <sstream>
#include "com/Serialization.h"


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( tests::GraphTest );


namespace { 
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
// GraphTest Helfer-Funktionen 
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
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
// liefert wert und position des ersten peaks in float array.
// liefert ansonsten [ startValue, UINT_MAX ]
typedef pair<float, size_t> PeakType;
PeakType firstPeak ( float *f, size_t blockSize, float startValue = 0.0f ) {
//=============================================================================
	for ( size_t i = 0; i<blockSize; ++i ) {
		if ( abs(f[i]) > startValue ) return std::make_pair( f[i], i );
	}
	return std::make_pair( startValue, UINT_MAX );
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
	processing::Frames fIn ( blockSize );
	processing::Frames fOut ( blockSize );
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
processing::Graph::Ptr GraphTest::createGraph( int blockSize, float samplerate ) 
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
GraphTest::GraphTest() {
//=============================================================================
	dummyFX = new processing::DummyFX ( NULL );
}
//=============================================================================
GraphTest::~GraphTest() {
//=============================================================================
	delete dummyFX;
}
//=============================================================================
void GraphTest::testConstructor() {
//=============================================================================
	using namespace std;
	using namespace com;
	using namespace processing;
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> std constr.
	Graph::Ptr graph = Graph::create ( dummyFX );
	CPPUNIT_ASSERT ( graph );
	size_t numNodes = 2;
	CPPUNIT_ASSERT_EQUAL ( numNodes, graph->getNumNodes() );
}
//=============================================================================
void GraphTest::testAddRemoveNodes() {
//=============================================================================
	using namespace std;
	using namespace com;
	using namespace processing;
	using namespace boost::assign;
	typedef stack<NOPNode::Ptr> ResultStack;
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> std constr.
	Graph::Ptr graph = Graph::create ( dummyFX );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>create nopnodes
	NOPNode::Ptr nodes[] = { NOPNode::create("0"), 
							 NOPNode::create("1"),
							 NOPNode::create("2"),
							 NOPNode::create("3"),
							 NOPNode::create("4")};
	const size_t NUM_NOPNODES = sizeof(nodes) / sizeof(nodes[0]);
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>add nodes to graph
	Graph::Janitor::Ptr jan = graph->getJanitor();
	size_t numNodes = graph->getNumNodes();
	for ( size_t i=0; i<NUM_NOPNODES; ++i ) jan->add( nodes[i] );
	numNodes += NUM_NOPNODES;
	CPPUNIT_ASSERT_EQUAL ( numNodes, graph->getNumNodes() );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>add NULL to graph
	jan->add( NOPNode::Ptr() );
	CPPUNIT_ASSERT_EQUAL ( numNodes, graph->getNumNodes() );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>check ids
	ResultStack res;
	// boost assign magic
	res += nodes[4], nodes[3], nodes[2], 
		   nodes[1], nodes[0], 
		   graph->getEndNode(), // versagt falls in graph constr. 
		   graph->getStartNode(); // add reihenfolge vertauscht
	CPPUNIT_ASSERT ( !res.empty() );
	const bgl::G & g = graph->getBglGraph(); 
	bgl::VertexIterator vi,end;
	for ( boost::tie(vi,end) = boost::vertices(g); vi!=end; ++vi ) {
		ProcessorNode::Ptr n1 = res.top();
		ProcessorNode::Ptr n2 = graph->getProcessorNode(*vi);
		CPPUNIT_ASSERT_EQUAL ( n1->getName(), n2->getName() );
		CPPUNIT_ASSERT_EQUAL ( n1, n2 );
		res.pop();
	}
	CPPUNIT_ASSERT ( res.empty() );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>remove nopnode 2
	CPPUNIT_ASSERT_EQUAL( Graph::Janitor::SUCCEED, jan->remove( nodes[2] ) );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>check ids
	res += nodes[4], nodes[3],
		   nodes[1], nodes[0], 
		   graph->getEndNode(), // versagt falls in graph constr. 
		   graph->getStartNode(); // add reihenfolge vertauscht
	CPPUNIT_ASSERT ( !res.empty() );
	for ( boost::tie(vi,end) = boost::vertices(g); vi!=end; ++vi ) {
		ProcessorNode::Ptr n1 = res.top();
		ProcessorNode::Ptr n2 = graph->getProcessorNode(*vi);
		CPPUNIT_ASSERT_EQUAL ( n1->getName(), n2->getName() );
		CPPUNIT_ASSERT_EQUAL ( n1, n2 );
		res.pop();
	}
	CPPUNIT_ASSERT ( res.empty() );
}
//=============================================================================
void GraphTest::testConnectNodes() {
//=============================================================================
	using namespace std;
	using namespace com;
	using namespace processing;
	typedef vector<ProcessorNode::Ptr> NodeContainer;
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> std constr.
	Graph::Ptr graph = Graph::create ( dummyFX );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>create nopnodes
	NOPNode::Ptr nodes[] = { NOPNode::create("0"), 
							 NOPNode::create("1"),
							 NOPNode::create("2"),
							 NOPNode::create("3"),
							 NOPNode::create("4")};
	const size_t NUM_NOPNODES = sizeof(nodes) / sizeof(nodes[0]);
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>add nodes to graph
	Graph::Janitor::Ptr jan = graph->getJanitor();
	for ( size_t i=0; i<NUM_NOPNODES; ++i ) jan->add( nodes[i] );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>connect node 0->1
	CPPUNIT_ASSERT_EQUAL ( 
		Graph::Janitor::SUCCEED,
		jan->connectNodes( nodes[0], nodes[1]) 
	);
	NodeContainer v;
	graph->getChildNodes<NodeContainer>( nodes[0], v );
	CPPUNIT_ASSERT_EQUAL ( (void*)v[0].get(), (void*)nodes[1].get() ); 
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>connect node 0->NULL
	CPPUNIT_ASSERT_EQUAL ( 
		Graph::Janitor::FAILED,
		jan->connectNodes( nodes[0], ProcessorNode::Ptr() )
	);
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>connect node NULL->node 0
	CPPUNIT_ASSERT_EQUAL ( 
		Graph::Janitor::FAILED,
		jan->connectNodes( ProcessorNode::Ptr(), nodes[0] )
	);
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>connect node NULL->NULL
	CPPUNIT_ASSERT_EQUAL ( 
		Graph::Janitor::FAILED,
		jan->connectNodes( ProcessorNode::Ptr(), ProcessorNode::Ptr() )
	);
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>connect node 0->1 again
	CPPUNIT_ASSERT_EQUAL ( 
		Graph::Janitor::FAILED,
		jan->connectNodes( nodes[0], nodes[1] )
	);
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>connect node 4->0
	CPPUNIT_ASSERT_EQUAL ( 
		Graph::Janitor::SUCCEED,
		jan->connectNodes( nodes[4], nodes[0] )
	);
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>connect node 0->2
	CPPUNIT_ASSERT_EQUAL ( 
		Graph::Janitor::SUCCEED,
		jan->connectNodes( nodes[0], nodes[2] )
	);
	v.clear();
	graph->getChildNodes<NodeContainer>( nodes[0], v );
	CPPUNIT_ASSERT ( v.size() == 2 );
	CPPUNIT_ASSERT ( com::contains<NodeContainer>( v, nodes[1] ) ); 
	CPPUNIT_ASSERT ( com::contains<NodeContainer>( v, nodes[2] ) ); 
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>connect node 2->0
	CPPUNIT_ASSERT_EQUAL ( 
		Graph::Janitor::FAILED, 
		jan->connectNodes( nodes[2], nodes[0] )
	);
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>connect node 3->1
	CPPUNIT_ASSERT_EQUAL ( 
		Graph::Janitor::SUCCEED,
		jan->connectNodes( nodes[3], nodes[1] )
	);
	v.clear();
	graph->getParentNodes<NodeContainer>( nodes[1], v );
	CPPUNIT_ASSERT ( v.size() == 2 );
	CPPUNIT_ASSERT ( com::contains<NodeContainer>( v, nodes[0] ) ); 
	CPPUNIT_ASSERT ( com::contains<NodeContainer>( v, nodes[3] ) ); 

	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>remove connection node 0->1
	CPPUNIT_ASSERT_EQUAL ( 
		Graph::Janitor::SUCCEED,
		jan->removeConnection( nodes[0], nodes[1] )
	);
	v.clear();
	graph->getParentNodes<NodeContainer>( nodes[1], v );
	CPPUNIT_ASSERT ( v.size() == 1 );
	CPPUNIT_ASSERT ( com::contains<NodeContainer>( v, nodes[3] ) ); 
}

//.............................................................................
// helfer
//.............................................................................
namespace signalProcessTest {
typedef boost::tuple < 
	processing::ProcessorNode::Ptr,  // node
	bool, // is_active 
	size_t // numActiveChildren
>  ProcessorNodeExpection;
//.............................................................................
ProcessorNodeExpection getNodeExpectionInfo ( processing::ProcessorNode *nPtr ) {
	using namespace processing;
	ProcessorNode::Ptr n = 
		boost::shared_dynamic_cast< ProcessorNode, PObject> ( nPtr->getPtr() );
	return ProcessorNodeExpection ( n , n->isActive(), n->getNumActiveChildren() );
}
//.............................................................................
com::MyString errMsg="no error."; 
//.............................................................................
bool compare ( 
	const ProcessorNodeExpection &soll,
	const ProcessorNodeExpection &ist 
) {
	errMsg = "Expection with " + boost::get<0>(soll)->getName() + " failed because: ";

	if ( boost::get<0>(soll) != boost::get<0>(ist) ) {
		errMsg += "unexpected node = " + boost::get<0>(ist)->getName();
		return false;
	}
	if ( boost::get<1>(soll) != boost::get<1>(ist) ) {
		errMsg += "isActive( " + com::MyString( boost::get<1>(soll) ) + "!=" + 
			       com::MyString( boost::get<1>(ist) ) + " )";
		return false;
	}
	if ( boost::get<2>(soll) != boost::get<2>(ist) ) {
		errMsg += "numActiveChildren( " + com::MyString( boost::get<2>(soll) ) + "!=" + 
			       com::MyString( boost::get<2>(ist) ) + " )";
		return false;
	}
	return true;
}
} // namespace

//=============================================================================
void GraphTest::testSignalProcessPath() {
//=============================================================================
	using namespace std;
	using namespace com;
	using namespace processing;
	using namespace signalProcessTest;
	using namespace boost::assign;
	typedef stack<NOPNode::Ptr> ResultStack;
	//                           Node,       isActive, numActiveChildren
	typedef stack<ProcessorNodeExpection> Expectations;
	Expectations expectations;
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> std constr.
	Graph::Ptr graph = createGraph( 512, 44100.0f );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>create adapter
	OutputStep::Ptr adapter[] = { 
		OutputStep::create( graph.get(), 4 ),
		OutputStep::create( graph.get(), 4 ),
		OutputStep::create( graph.get(), 4 )};
	const size_t NUM_ADAPTER = sizeof(adapter) / sizeof(adapter[0]);
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>add adapter to graph
	Graph::Janitor::Ptr jan = graph->getJanitor();
	size_t numNodes = graph->getNumNodes();
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>test empty graph
	CPPUNIT_ASSERT ( !graph->isActive() );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>test:
	/*      
		    E---------->S           E => endnode
			                        S => startnode
			                        A => adapter                           
	*/
	jan->connectNodes( graph->getStartNode(), graph->getEndNode() );
	jan.reset();
	// soll
	expectations += ProcessorNodeExpection( graph->getEndNode(), true, 0 );
	// ist
	const Graph::SignalProcessPath &sPath = graph->getSignalProcessPath();
	// evaluation
	Graph::SignalProcessPath::const_iterator it = sPath.begin();
	for ( ; it!=sPath.end(); ++it ) {
		CPPUNIT_ASSERT ( !expectations.empty() );
		ProcessorNodeExpection soll = expectations.top();
		ProcessorNodeExpection ist = getNodeExpectionInfo(*it);
		bool res = compare (soll,ist); // wegen errMsg erst auswerten dann assert
		CPPUNIT_ASSERT_MESSAGE ( errMsg, res );
		expectations.pop();
	}
	CPPUNIT_ASSERT ( expectations.empty() );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>remove Connection
	jan = graph->getJanitor();
	jan->removeConnection( graph->getStartNode(), graph->getEndNode() );
	jan.reset();
	CPPUNIT_ASSERT ( sPath.empty() );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>test:
	/*      
		    E---->A0    S       E => endnode
						        S => startnode
			                    A => adapter                   
	*/
	jan = graph->getJanitor();
	jan->add( adapter[0] );
	jan->connectNodes( adapter[0]->getOutputNode(0), graph->getEndNode() );
	jan.reset();
	// soll
	expectations += ProcessorNodeExpection( graph->getEndNode(), true, 0 ),           //   A
					ProcessorNodeExpection( adapter[0]->getOutputNode(0), true, 1 ),  //   |
					ProcessorNodeExpection( adapter[0]->getAdapterNode(), true, 1 ),  //   |
					ProcessorNodeExpection( adapter[0]->getInputNode(0),  true, 1 );  //   _
	// evaluation
	it = sPath.begin();
	for ( ; it!=sPath.end(); ++it ) {
		CPPUNIT_ASSERT ( !expectations.empty() );
		ProcessorNodeExpection soll = expectations.top();
		ProcessorNodeExpection ist = getNodeExpectionInfo(*it);
		bool res = compare(soll,ist); // wegen errMsg erst auswerten dann assert
		CPPUNIT_ASSERT_MESSAGE ( errMsg, res );
		expectations.pop();
	}
	CPPUNIT_ASSERT ( expectations.empty() );
	CPPUNIT_ASSERT ( !adapter[0]->getOutputNode(1)->isActive() );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>test:
	/*      
		    E---->A0--->S           E => endnode
			                        S => startnode
			                        A => adapter                           
	*/
	jan = graph->getJanitor();
	jan->add( adapter[0] );
	jan->connectNodes( graph->getStartNode(), adapter[0]->getInputNode(0) );
	jan.reset();
	// soll
	expectations += ProcessorNodeExpection( graph->getEndNode(), true, 0 ),           //   A
					ProcessorNodeExpection( adapter[0]->getOutputNode(0), true, 1 ),  //   |
					ProcessorNodeExpection( adapter[0]->getAdapterNode(), true, 1 ),  //   |
					ProcessorNodeExpection( adapter[0]->getInputNode(0),  true, 1 );  //   _
	// evaluation
	it = sPath.begin();
	for ( ; it!=sPath.end(); ++it ) {
		CPPUNIT_ASSERT ( !expectations.empty() );
		ProcessorNodeExpection soll = expectations.top();
		ProcessorNodeExpection ist = getNodeExpectionInfo(*it);
		bool res = compare(soll,ist); // wegen errMsg erst auswerten dann assert
		CPPUNIT_ASSERT_MESSAGE ( errMsg, res );
		expectations.pop();
	}
	CPPUNIT_ASSERT ( expectations.empty() );
	CPPUNIT_ASSERT ( !adapter[0]->getOutputNode(1)->isActive() );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>remove Adapter
	jan = graph->getJanitor();
	jan->remove( adapter[0] );
	jan.reset();
	CPPUNIT_ASSERT ( sPath.empty() );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>test:
	/*      
		    E---->A0--->S           E => endnode
			 \         /            S => startnode
			  A2      A1            A => adapter
                                   
	*/
	jan = graph->getJanitor();
	jan->add( adapter[0] );
	jan->connectNodes( graph->getStartNode(), adapter[0]->getInputNode(0) );
	jan->connectNodes( adapter[0]->getOutputNode(0), graph->getEndNode() );
	jan->add( adapter[1] );
	jan->add( adapter[2] );
	jan->connectNodes( graph->getStartNode(), adapter[1]->getInputNode(0) );
	jan->connectNodes( adapter[2]->getOutputNode(0), graph->getEndNode() );
	jan.reset();
	bool a = sPath.front() == adapter[0]->getInputNode(0).get();
	bool b = sPath.front() == adapter[2]->getInputNode(0).get();
	CPPUNIT_ASSERT ( a || b );
	// soll
	if ( a ) { // variante 1
		expectations += ProcessorNodeExpection( graph->getEndNode(), true, 0 ),
			ProcessorNodeExpection( adapter[2]->getOutputNode(0), true, 1 ),  //   A
			ProcessorNodeExpection( adapter[2]->getAdapterNode(), true, 1 ), //    |
			ProcessorNodeExpection( adapter[2]->getInputNode(0), true, 1 ),   //   |
			ProcessorNodeExpection( adapter[0]->getOutputNode(0), true, 1 ),  //   |
			ProcessorNodeExpection( adapter[0]->getAdapterNode(), true, 1 ),  //   |
			ProcessorNodeExpection( adapter[0]->getInputNode(0),  true, 1 );  //   _
	} else { // variante 2
		expectations += ProcessorNodeExpection( graph->getEndNode(), true, 0 ),
			ProcessorNodeExpection( adapter[0]->getOutputNode(0), true, 1 ),  //   A
			ProcessorNodeExpection( adapter[0]->getAdapterNode(), true, 1 ), //    |
			ProcessorNodeExpection( adapter[0]->getInputNode(0), true, 1 ),   //   |
			ProcessorNodeExpection( adapter[2]->getOutputNode(0), true, 1 ),  //   |
			ProcessorNodeExpection( adapter[2]->getAdapterNode(), true, 1 ),  //   |
			ProcessorNodeExpection( adapter[2]->getInputNode(0),  true, 1 );  //   _
	}
	// evaluation
	it = sPath.begin();
	for ( ; it!=sPath.end(); ++it ) {
		CPPUNIT_ASSERT ( !expectations.empty() );
		ProcessorNodeExpection soll = expectations.top();
		ProcessorNodeExpection ist = getNodeExpectionInfo(*it);
		bool res = compare(soll,ist); // wegen errMsg erst auswerten dann assert
		CPPUNIT_ASSERT_MESSAGE ( errMsg, res );
		expectations.pop();
	}
	CPPUNIT_ASSERT ( !adapter[1]->getInputNode(0)->isActive() );
	CPPUNIT_ASSERT ( !adapter[1]->getAdapterNode()->isActive() );
	CPPUNIT_ASSERT ( !adapter[1]->getOutputNode(0)->isActive() );
	CPPUNIT_ASSERT ( !adapter[1]->getOutputNode(1)->isActive() );
	CPPUNIT_ASSERT ( expectations.empty() );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>check activeChildren
	jan = graph->getJanitor();
	jan->connectNodes( adapter[0]->getOutputNode(1), graph->getEndNode() );
	jan.reset();
	CPPUNIT_ASSERT_EQUAL ( (size_t)2, adapter[0]->getAdapterNode()->getNumActiveChildren() );
	jan = graph->getJanitor();
	jan->removeConnection( adapter[0]->getOutputNode(0), graph->getEndNode() );
	jan.reset();
	CPPUNIT_ASSERT_EQUAL ( (size_t)1, adapter[0]->getAdapterNode()->getNumActiveChildren() );
}

#define CPPUNIT_ASSERT_EQUAL_PEAK( peak, value, pos )\
	    CPPUNIT_ASSERT_EQUAL ( (float)(value), (float)(peak).first ); \
	    CPPUNIT_ASSERT_EQUAL ( (size_t)(pos), (size_t)(peak).second );

#define TEST_PEAK( frame, value, pos, blocksize ) {       \
	PeakType peak = firstPeak( (frame)[0], (blocksize) ); \
	CPPUNIT_ASSERT_EQUAL_PEAK ( peak, (value), (pos) );   \
	peak = firstPeak( (frame)[1], (blocksize) );          \
	CPPUNIT_ASSERT_EQUAL_PEAK ( peak, (value), (pos) );}  \

#define TEST_PEAK_2( frame, start, value, pos, blocksize ) {       \
	PeakType peak = firstPeak( (frame)[0], (blocksize), (start) ); \
	CPPUNIT_ASSERT_EQUAL_PEAK ( peak, (value), (pos) );			   \
	peak = firstPeak( (frame)[1], (blocksize), (start) );          \
	CPPUNIT_ASSERT_EQUAL_PEAK ( peak, (value), (pos) );}		   \
	    

//=============================================================================
void GraphTest::testDelayAdapter() {
//=============================================================================
	using namespace std;
	using namespace com;
	using namespace processing;
	const int BLOCKSIZE = 255;
	enum { DELAY = 125 };
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>prepare graph
	Graph::Ptr graph = createGraph( BLOCKSIZE, 44100.0f );
	DelayAdapter<DELAY>::Ptr delay( DelayAdapter<DELAY>::create( graph.get() ) );
	Graph::Janitor::Ptr jan = graph->getJanitor();
	jan->add( delay );
	jan->connectNodes ( graph->getStartNode(), delay->getInputNode(0) );
	jan->connectNodes ( delay->getOutputNode(0), graph->getEndNode() );
	jan.reset();
	CPPUNIT_ASSERT( graph->isActive() );
	CPPUNIT_ASSERT_EQUAL( (size_t)DELAY, graph->getGraphDelay() );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>prepare frames
	Frames inFr, outFr;
	inFr.setSize( BLOCKSIZE );
	outFr.setSize( BLOCKSIZE );
	fillFrame ( &inFr, 0.5, 0.5 );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>process graph
	graph->pushAndCopy ( &inFr, BLOCKSIZE );
	graph->processGraph( outFr.getData(), BLOCKSIZE );
	TEST_PEAK( outFr, 0.5f, DELAY, BLOCKSIZE );
}
//=============================================================================
void GraphTest::testDelayCompensationSimple() {
//=============================================================================
	/*                         S
	                         /   \       S = startNode
						    D    ND      E = endNode
							 \   /       D = Adapter with delay
	                           E        ND = Adapter without delay

				   expected output:
				   0, 0, 0, ... , input*2, input*2, ...
				   A                 A
				   |                 |
			      i=0              i=delay
	*/
	using namespace std;
	using namespace com;
	using namespace processing;
	const int BLOCKSIZE = 255;
	enum { DELAY = 125 };
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>prepare graph
	Graph::Ptr graph = createGraph( BLOCKSIZE, 44100.0f );
	DelayAdapter<DELAY>::Ptr delay( DelayAdapter<DELAY>::create( graph.get() ) );
	Volume::Ptr noDelay( Volume::create( graph.get() ) );
	Graph::Janitor::Ptr jan = graph->getJanitor();
	jan->add( delay );
	jan->connectNodes ( graph->getStartNode(), delay->getInputNode(0) );
	jan->connectNodes ( delay->getOutputNode(0), graph->getEndNode() );
	jan.reset();
	CPPUNIT_ASSERT( graph->isActive() );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>add no-delay adapter parallel 
	jan = graph->getJanitor();
	jan->add( noDelay );
	jan->connectNodes ( graph->getStartNode(), noDelay->getInputNode(0) );
	jan->connectNodes ( noDelay->getOutputNode(0), graph->getEndNode() );
	jan.reset();
	CPPUNIT_ASSERT( graph->isActive() );
	CPPUNIT_ASSERT_EQUAL( (size_t)DELAY, graph->getGraphDelay() );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>prepare frames
	Frames inFr, outFr;
	inFr.setSize( BLOCKSIZE );
	outFr.setSize( BLOCKSIZE );
	fillFrame ( &inFr, 0.5, 0.5 );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>process graph
	graph->pushAndCopy ( &inFr, BLOCKSIZE );
	graph->processGraph( outFr.getData(), BLOCKSIZE );
	TEST_PEAK( outFr, 1.0f, DELAY, BLOCKSIZE );
}

//=============================================================================
void GraphTest::testDelayCompensationTree() {
//=============================================================================
	/*                         S
	                         /   \         S = startNode
						    D     D        E = endNode
						   / \   / \       D = Adapter with delay
	                      D   D D   D
							  ...
                       ... \ \  / / ...
                               E          += direct connection between S and E

				   expected output: SUM = 2^N
				   0, 0, 0, ... , SUM, SUM, ...
				   A               A
				   |               |
			      i=0              i=delay * (N-1)
	*/
	using namespace std;
	using namespace com;
	using namespace processing;
	const int BLOCKSIZE = 1024; // !! graph_delay == 500
	enum { N = 5, DELAY = 125 };
	enum { GRAPH_DELAY = DELAY * (N-1) };
	static const float INPUT = 0.125f;
	static const float SUM = INPUT * (float)( 1<<(N-1) ) + INPUT; // + INPUT: direct connection between S and E
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>prepare graph
	Graph::Ptr graph = createGraph( BLOCKSIZE, 44100.0f );
	typedef CreateAdapter< DelayAdapter<DELAY> > Adapter;
	Graph::Janitor::Ptr jan = graph->getJanitor();
	CreateBinaryTree< Adapter, N >( graph, graph->getStartNode(), graph->getEndNode() ); 
	jan->connectNodes ( graph->getStartNode(), graph->getEndNode() ); 
	jan.reset();
	CPPUNIT_ASSERT( graph->isActive() );
	CPPUNIT_ASSERT_EQUAL( graph->getGraphDelay(), (size_t)GRAPH_DELAY );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>prepare frames
	Frames inFr, outFr;
	inFr.setSize( BLOCKSIZE );
	outFr.setSize( BLOCKSIZE );
	fillFrame ( &inFr, INPUT, INPUT );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>process graph
	graph->pushAndCopy ( &inFr, BLOCKSIZE );
	graph->processGraph( outFr.getData(), BLOCKSIZE );
	TEST_PEAK( outFr, SUM, GRAPH_DELAY, BLOCKSIZE );
}
//=============================================================================
void GraphTest::testDelayCompensationComplex1() {
//=============================================================================
	/*                            S             
						      /	     \		        S = startNode
					 _____   /         \  ______      E = endNode
					/	  / | \      / | \      \     D1/2 = Adapter with delay  ( D2 > D1 )
	              N ... D1  D1 D1   D2 D2 D2 ... M    ND = Adapter without delay
 					\	 |   |  |   |  |  |    /
                     \    \   \ \   /  /  /   /      
								  ND
								  |
								  E

				   expected output: INPUT * N + INPUT * M
				   0, 0, 0, ... , SUM, SUM, ...
				   A               A
				   |               |
			      i=0              i = D2
	*/
	using namespace std;
	using namespace com;
	using namespace processing;
	const int BLOCKSIZE = 1024; // !! graph_delay == 500
	enum { N = 5, M = 3, D1 = 125, D2 = 199 };
	BOOST_STATIC_ASSERT ( D1 < D2 );
	enum { GRAPH_DELAY = D2 };
	static const float INPUT = 0.125f;
	static const float SUM = INPUT * N + INPUT * M;
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>prepare graph
	Graph::Ptr graph = createGraph( BLOCKSIZE, 44100.0f );
	typedef CreateAdapter< DelayAdapter<D1> > AdapterD1;
	typedef CreateAdapter< DelayAdapter<D2> > AdapterD2;
	Volume::Ptr nd = Volume::create( graph.get(), 1.0f );
	Graph::Janitor::Ptr jan = graph->getJanitor();
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>add Adapter
	jan->add(nd);
	jan->connectNodes( nd->getOutputNode(0), graph->getEndNode() );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>build complex
	CreateParallel< AdapterD1, N >( graph, graph->getStartNode(), nd->getInputNode(0) ); 
	CreateParallel< AdapterD2, M >( graph, graph->getStartNode(), nd->getInputNode(0) ); 
	jan.reset();
	CPPUNIT_ASSERT( graph->isActive() );
	CPPUNIT_ASSERT_EQUAL( graph->getGraphDelay(), (size_t)GRAPH_DELAY );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>prepare frames
	Frames inFr, outFr;
	inFr.setSize( BLOCKSIZE );
	outFr.setSize( BLOCKSIZE );
	fillFrame ( &inFr, INPUT, INPUT );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>process graph
	graph->pushAndCopy ( &inFr, BLOCKSIZE );
	graph->processGraph( outFr.getData(), BLOCKSIZE );
	TEST_PEAK( outFr, SUM, GRAPH_DELAY, BLOCKSIZE );
}
//=============================================================================
void GraphTest::testJanitorCreate() {
//=============================================================================
	using namespace std;
	using namespace com;
	using namespace processing;
	Graph::Ptr graph = createGraph( 255, 44100.0f );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>get Janitor
	Graph::Janitor::Ptr jan1 = graph->getJanitor();
	CPPUNIT_ASSERT ( jan1 );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>Singleton
	Graph::Janitor::Ptr jan2 = graph->getJanitor();
	CPPUNIT_ASSERT ( jan1.get() == jan2.get() );
}
//=============================================================================
void GraphTest::testJanitorLock() { // erwartet exception
//=============================================================================
	using namespace std;
	using namespace com;
	using namespace processing;
	typedef boost::unique_lock<com::Mutex> UniqueLock;
	Graph::Ptr graph = createGraph( 255, 44100.0f );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Graph unlocked
	{
		UniqueLock lock( graph->getProcessingLock(), boost::try_to_lock);
		CPPUNIT_ASSERT ( lock.owns_lock() ); 
	}
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>get Janitor
	Graph::Janitor::Ptr jan1 = graph->getJanitor();
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>Graph locked
	{
		UniqueLock lock( graph->getProcessingLock(), boost::try_to_lock);
		CPPUNIT_ASSERT ( !lock.owns_lock() ); 
	}
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>get Janitor2
	Graph::Janitor::Ptr jan2 = graph->getJanitor();
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>Graph locked2
	{
		UniqueLock lock( graph->getProcessingLock(), boost::try_to_lock);
		CPPUNIT_ASSERT ( !lock.owns_lock() ); 
	}
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>release Janitor2
	jan2.reset();
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>Graph locked2
	{
		UniqueLock lock( graph->getProcessingLock(), boost::try_to_lock);
		CPPUNIT_ASSERT ( !lock.owns_lock() ); 
	}
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>release Janitor1
	jan1.reset();
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>Graph unlocked2
	{
		UniqueLock lock( graph->getProcessingLock(), boost::try_to_lock);
		CPPUNIT_ASSERT ( lock.owns_lock() ); 
	}
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>get Janitor
	jan1 = graph->getJanitor();
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>dead lock ex.
	TRY_TO_LOCK_TIMED ( graph->getProcessingLock() );
	
}
//=============================================================================
void GraphTest::testGraphConsistency() { 
//=============================================================================
	using namespace std;
	using namespace com;
	using namespace processing;
	int blockSize = 1024;
	float sampleRate = 44100.0f;
	enum { FAILED=Graph::Janitor::FAILED, SUCCEED=Graph::Janitor::SUCCEED };
	Graph::Ptr graph = createGraph( blockSize, sampleRate );
	ProcessorNode::Ptr startNode = graph->getStartNode();
	ProcessorNode::Ptr endNode   = graph->getEndNode();
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>add/remove Adapter loop
	for ( int i=0; i<101; ++i ) {
		// ...add
		Graph::Janitor::Ptr janitor = graph->getJanitor();
		OutputStep::Ptr ad = OutputStep::create( graph.get() );
		CPPUNIT_ASSERT ( janitor->add ( ad ) == SUCCEED );
		// start-, endNode, In, Adapter, Out1, Out2 = 6
		CPPUNIT_ASSERT_EQUAL ( (size_t)6, graph->getNumNodes() );
		janitor.reset();
		// ..remove
		janitor = graph->getJanitor();
		janitor->remove( ad );
	}
	CPPUNIT_ASSERT_EQUAL ( (size_t)0, graph->getNumEdges() );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>add Adapter
	Graph::Janitor::Ptr janitor = graph->getJanitor();
	OutputStep::Ptr ad = OutputStep::create( graph.get() );
	CPPUNIT_ASSERT ( janitor->add ( ad ) == SUCCEED );
	// start-, endNode, In, Adapter, Out1, Out2 = 6
	size_t numNodes = 6; 
	CPPUNIT_ASSERT_EQUAL ( numNodes, graph->getNumNodes() );
	janitor.reset();
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>add NULL
	janitor = graph->getJanitor();
	CPPUNIT_ASSERT ( janitor->add( NOPNode::Ptr() ) == FAILED );
	janitor.reset();
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>add Adapter again
	janitor = graph->getJanitor();
	CPPUNIT_ASSERT ( janitor->add(ad) == FAILED );
	CPPUNIT_ASSERT_EQUAL ( numNodes, graph->getNumNodes() );
	janitor.reset();
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>connect with itself
	janitor = graph->getJanitor();
	CPPUNIT_ASSERT ( 
		janitor->connectNodes(ad->getOutputNode(0).get(), ad->getOutputNode(0).get() ) == FAILED 
	);
	CPPUNIT_ASSERT ( 
		janitor->connectNodes(ad->getInputNode(0).get(), ad->getInputNode(0).get() ) == FAILED 
	);
	CPPUNIT_ASSERT ( 
		janitor->connectNodes( startNode.get(), startNode.get() ) == FAILED 
	);
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>graph inactive
	CPPUNIT_ASSERT ( !graph->isActive() );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>hook in adapter
	janitor = graph->getJanitor();
	CPPUNIT_ASSERT 
		( janitor->connectNodes( startNode.get(), ad->getInputNode(0).get() ) == SUCCEED );
	CPPUNIT_ASSERT 
		( janitor->connectNodes( ad->getOutputNode(0).get(), endNode.get() ) == SUCCEED );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>hook in adapter again
	janitor = graph->getJanitor();
	CPPUNIT_ASSERT 
		( janitor->connectNodes( startNode.get(), ad->getInputNode(0).get() ) == FAILED );
	CPPUNIT_ASSERT 
		( janitor->connectNodes( ad->getOutputNode(0).get(), endNode.get() ) == FAILED );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>graph active
	janitor.reset();
	CPPUNIT_ASSERT ( graph->isActive() );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>calc Graph
	try {
		processGraph ( graph.get(), blockSize, 0.0f, 0.0f, 0.0f, 0.0f ); 
	} catch ( ProcessException &ex ) {
		CPPUNIT_ASSERT_MESSAGE ( ex.text, false );
	}
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>change SampleRate
	janitor = graph->getJanitor();
	sampleRate/=2;
	dummyFX->setSampleRate( sampleRate );
	janitor->hostInfoChanged();
	janitor.reset();
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>calc Graph
	try {
		processGraph ( graph.get(), blockSize, 0.0f, 0.0f, 0.0f, 0.0f, 100 ); 
	} catch ( ProcessException &ex ) {
		CPPUNIT_ASSERT_MESSAGE ( ex.text, false );
	}
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>change BlockSize
	janitor = graph->getJanitor();
	blockSize/=2;
	dummyFX->setBlockSize( blockSize );
	janitor->hostInfoChanged();
	janitor.reset();
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>calc Graph
	try {
		processGraph ( graph.get(), blockSize, 0.0f, 0.0f, 0.0f, 0.0f, 100 ); 
	} catch ( ProcessException &ex ) {
		CPPUNIT_ASSERT_MESSAGE ( ex.text, false );
	}
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>change BlockSize / SampleRate
	janitor = graph->getJanitor();
	blockSize*=8;
	sampleRate*=8;
	dummyFX->setBlockSize( blockSize );
	dummyFX->setSampleRate( sampleRate );
	janitor->hostInfoChanged();
	janitor.reset();
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>calc Graph
	try {
		processGraph ( graph.get(), blockSize, 0.0f, 0.0f, 0.0f, 0.0f, 100 ); 
	} catch ( ProcessException &ex ) {
		CPPUNIT_ASSERT_MESSAGE ( ex.text, false );
	}
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>calc Graph
	try {
		processGraph ( graph.get(), blockSize, 0.0f, 0.0f, 0.0f, 0.0f, 100 ); 
	} catch ( ProcessException &ex ) {
		CPPUNIT_ASSERT_MESSAGE ( ex.text, false );
	}
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>set BlockSize zero
	janitor = graph->getJanitor();
	dummyFX->setBlockSize( 0 );
	CPPUNIT_ASSERT_THROW ( janitor->hostInfoChanged(), ppiError::InvalidBlockSize );
	janitor.reset();
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>set Samplerate zero
	janitor = graph->getJanitor();
	dummyFX->setSampleRate( 0 );
	dummyFX->setBlockSize( 255 );
	CPPUNIT_ASSERT_THROW ( janitor->hostInfoChanged(), ppiError::InvalidSampleRate );
	janitor.reset();
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>reset hostInfo
	janitor = graph->getJanitor();
	dummyFX->setSampleRate( 44100.0f );
	dummyFX->setBlockSize( 255 );
	janitor->hostInfoChanged();
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>connect with non graph object
	janitor = graph->getJanitor();
	OutputStep::Ptr ad2 = OutputStep::create( graph.get() );
	CPPUNIT_ASSERT ( 
		janitor->connectNodes ( startNode.get(), ad2->getInputNode(0).get() ) == FAILED
	);
	CPPUNIT_ASSERT ( 
		janitor->connectNodes ( ad->getInputNode(0).get(), ad2->getOutputNode(0).get() ) == FAILED
	);
	// janitor.reset() // mal weglassen
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>add another adapter
	janitor = graph->getJanitor();
	CPPUNIT_ASSERT ( janitor->add ( ad2 ) == SUCCEED );
	// start-, endNode, ( In, Adapter, Out1, Out2 ) * 2 = 10
	numNodes = 10; 
	janitor.reset();
	CPPUNIT_ASSERT_EQUAL ( numNodes, graph->getNumNodes() );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>try to create feedback connection
	janitor = graph->getJanitor();
	Graph::Janitor::State res;
	res = janitor->connectNodes ( ad->getOutputNode(0).get(), ad2->getInputNode(0).get() );
	CPPUNIT_ASSERT ( res == SUCCEED );
	res = janitor->connectNodes ( ad2->getOutputNode(0).get(), ad->getInputNode(0).get() );
	CPPUNIT_ASSERT ( res == FAILED );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>owner test
	janitor.reset();
	Graph::Ptr graph2 = createGraph ( 255, 44100.0f );
	Graph::Janitor::Ptr janitor2 = graph2->getJanitor(); 
	CPPUNIT_ASSERT_THROW ( janitor2->add ( ad2 ), ppiError::OwnerLock );
	janitor2.reset();
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>owner test: after remove
	graph->getJanitor()->remove(ad2);
	janitor2 = graph2->getJanitor();
	janitor2->add ( ad2 );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>adapter index overflow
	CPPUNIT_ASSERT_THROW ( ad2->getOutputNode(2), ppiError::IndexOutOfBoundException );
	CPPUNIT_ASSERT_THROW ( ad2->getInputNode(1), ppiError::IndexOutOfBoundException );
}	 
//=============================================================================
// check with extra static "num_copyintos" variable in Frames. Which only exists
// when _FORX_TESTSUITE #defined.
void GraphTest::testGraphSeries() { 
//=============================================================================
	using namespace std;
	using namespace com;
	using namespace processing;
	int blockSize = 512;
	enum { GRAPH_DEPTH=13 };
	Graph::Ptr graph = createGraph( blockSize, 44100.0f );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>create GRAPH_DEPTH VolumeAdapters series:
	// 
	CreateSeries< CreateAdapter<Volume>, GRAPH_DEPTH > 
		series( graph, graph->getStartNode(), graph->getEndNode() );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> check creation
	//                              4 = input + adapter + output + helper ( created by CreateSeries )
	//                              |
	CPPUNIT_ASSERT_EQUAL ( (size_t)(4 * GRAPH_DEPTH + 2), graph->getNumNodes() );
	CPPUNIT_ASSERT ( graph->isActive() );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> process
	Frames inFrame( blockSize );
	Frames outFrame( blockSize );
	fillFrame ( &inFrame, 0.5f, -0.5f );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>process graph. expect frame num copied = 0
	Frames::num_copyintos = 0; // reset copy_counter
	graph->pushAndCopy ( &inFrame, blockSize );
	graph->processGraph( outFrame.getData(), blockSize  );
	CPPUNIT_ASSERT_EQUAL ( (size_t) 0, Frames::num_copyintos );
	CPPUNIT_ASSERT_EQUAL ( (float) 0.5f, isFilledWith<float>( outFrame[0], outFrame.getSize(), 0.5  ) );
	CPPUNIT_ASSERT_EQUAL ( (float)-0.5f, isFilledWith<float>( outFrame[1], outFrame.getSize(), -0.5 ) );
}	
//=============================================================================
// check with extra static "num_copyintos" variable in Frames. Which only exists
// when _FORX_TESTSUITE #defined.
void GraphTest::testGraphParallel() { 
//=============================================================================
	using namespace std;
	using namespace com;
	using namespace processing;
	int blockSize = 512;
	enum { GRAPH_DEPTH=13 };
	Graph::Ptr graph = createGraph( blockSize, 44100.0f );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>create GRAPH_DEPTH VolumeAdapters parallel:
	// 
	CreateParallel< CreateAdapter<Volume>, GRAPH_DEPTH > 
		parallel( graph, graph->getStartNode(), graph->getEndNode() );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> check creation
	//                              3= input + adapter + output  
	//                              |
	CPPUNIT_ASSERT_EQUAL ( (size_t)(3 * GRAPH_DEPTH + 2), graph->getNumNodes() );
	CPPUNIT_ASSERT ( graph->isActive() );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> process
	Frames inFrame( blockSize );
	Frames outFrame( blockSize );
	fillFrame ( &inFrame, 0.5f, -0.5f );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>process graph. expect frame num copied = GRAPH_DEPTH - 1
	Frames::num_copyintos = 0; // reset copy_counter
	graph->pushAndCopy ( &inFrame, blockSize );
	graph->processGraph( outFrame.getData(), blockSize  );
	CPPUNIT_ASSERT_EQUAL ( (size_t)GRAPH_DEPTH - 1, Frames::num_copyintos );
	CPPUNIT_ASSERT_EQUAL ( (float) 0.5f * GRAPH_DEPTH, isFilledWith<float>( outFrame[0], outFrame.getSize(), 0.5 * GRAPH_DEPTH ) );
	CPPUNIT_ASSERT_EQUAL ( (float)-0.5f * GRAPH_DEPTH, isFilledWith<float>( outFrame[1], outFrame.getSize(), -0.5 * GRAPH_DEPTH ) );
}	
//=============================================================================
// check with extra static "num_copyintos" variable in Frames. Which only exists
// when _FORX_TESTSUITE #defined.
void GraphTest::testGraphComplex1() { 
//=============================================================================
	/*
			X                     X = 1
		   /|\ ...N... \          O = Volume(0.5)
		  O O O        O
		  | | |        |
		 ...N...      ...
		  O O O        O
	      \ | /       /
		    =
	*/
	using namespace std;
	using namespace com;
	using namespace processing;
	int blockSize = 512;
	enum { N=5 };
	Graph::Ptr graph = createGraph( blockSize, 44100.0f );
	enum { NUMERATOR = 1, DENOMINATOR = 2 };
	static const float VOL = NUMERATOR / (float)DENOMINATOR;
	static const float X = 1.0f;
	static const float SUM = N * X * pow(VOL,N);
	typedef CreateAdapter< VolumeAdapterX<NUMERATOR, DENOMINATOR> > Adapter;
	typedef CreateParallel< CreateSeries< Adapter, N >, N> Creator;
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>create
	// 
	Graph::Janitor::Ptr janitor = graph->getJanitor(); // !! avoid graph update after every create iteration
	Creator complex( graph, graph->getStartNode(), graph->getEndNode() );
	janitor.reset();
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> check creation
	CPPUNIT_ASSERT_EQUAL ( (size_t)( N * N * 4 + 2 ), graph->getNumNodes() );
	CPPUNIT_ASSERT_EQUAL ( (size_t)Creator::NUM_CREATED_ADAPTER, graph->getNumAdapter() );
	CPPUNIT_ASSERT ( graph->isActive() );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> process
	Frames inFrame( blockSize );
	Frames outFrame( blockSize );
	fillFrame ( &inFrame, X,  -X );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>process graph. expect frame num copied = N - 1
	Frames::num_copyintos = 0; // reset copy_counter
	graph->pushAndCopy ( &inFrame, blockSize );
	graph->processGraph( outFrame.getData(), blockSize  );
	CPPUNIT_ASSERT_EQUAL ( (size_t)N - 1, Frames::num_copyintos );
	CPPUNIT_ASSERT_EQUAL ( SUM, isFilledWith<float>( outFrame[0], outFrame.getSize(), SUM ) );
	CPPUNIT_ASSERT_EQUAL ( -SUM, isFilledWith<float>( outFrame[1], outFrame.getSize(), -SUM ) );
}	
//=============================================================================
// check with extra static "num_copyintos" variable in Frames. Which only exists
// when _FORX_TESTSUITE #defined.
void GraphTest::testGraphComplex2() { 
//=============================================================================
	/*
			X                     X = 1
		   /|\ ...N... \          O = 0.5
		  O O O        O
	      \ | /       /
		    |
		 ...N...
		   /|\ ...N... \          
		  O O O        O          X * (0.5*N) * (0.5*N) * ... N 
	      \ | /       /         = X * (0.5*N) ^ N
		    |
			=
	*/
	using namespace std;
	using namespace com;
	using namespace processing;
	int blockSize = 512;
	enum { N=5 };
	Graph::Ptr graph = createGraph( blockSize, 44100.0f );
	enum { NUMERATOR = 1, DENOMINATOR = 2 };
	static const float VOL = NUMERATOR / (float)DENOMINATOR;
	static const float X = 1.0f;
	static const float SUM = X * pow(VOL*N, N);
	typedef CreateAdapter< VolumeAdapterX<NUMERATOR, DENOMINATOR> > Adapter;
	typedef CreateSeries< CreateParallel< Adapter, N >, N> Creator;
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>create
	// 
	Graph::Janitor::Ptr janitor = graph->getJanitor(); // !! avoid graph update after every create iteration
	Creator ( graph, graph->getStartNode(), graph->getEndNode() );
	janitor.reset();
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> check creation
	CPPUNIT_ASSERT_EQUAL ( (size_t)( N * N * 3 + N + 2 ), graph->getNumNodes() );
	CPPUNIT_ASSERT_EQUAL ( (size_t)Creator::NUM_CREATED_ADAPTER, graph->getNumAdapter() );
	CPPUNIT_ASSERT ( graph->isActive() );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> process
	Frames inFrame( blockSize );
	Frames outFrame( blockSize );
	fillFrame ( &inFrame, X,  -X );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>process graph. expect frame num copied = (N-1) * N
	Frames::num_copyintos = 0; // reset copy_counter
	graph->pushAndCopy ( &inFrame, blockSize );
	graph->processGraph( outFrame.getData(), blockSize  );
	CPPUNIT_ASSERT_EQUAL ( (size_t)(N-1)*N, Frames::num_copyintos );
	CPPUNIT_ASSERT_EQUAL ( SUM, isFilledWith<float>( outFrame[0], outFrame.getSize(), SUM ) );
	CPPUNIT_ASSERT_EQUAL ( -SUM, isFilledWith<float>( outFrame[1], outFrame.getSize(), -SUM ) );
}	
//=============================================================================
// check with extra static "num_copyintos" variable in Frames. Which only exists
// when _FORX_TESTSUITE #defined.
void GraphTest::testGraphComplex3() { 
//=============================================================================
	/*
			X                     X = 1
		   / \                    O = 0.5
		  O   O        
		 /|   |\
	    O O   O O
		... N ...         
		    
	*/
	using namespace std;
	using namespace com;
	using namespace processing;
	int blockSize = 512;
	enum { N=5 };
	Graph::Ptr graph = createGraph( blockSize, 44100.0f );
	enum { NUMERATOR = 1, DENOMINATOR = 2 }; // bei gewissen bruechen kann es zu assertion kommen
											 // obwohl die fehler ausgabe(soll,ist) identisch ist. 
											 // ( wahrsch. rundungsfehler ) 
	static const float VOL = NUMERATOR / (float)DENOMINATOR;
	static const float X = 1.0f;
	static const float SUM = pow(VOL, N) * (float)(1<<N);
	typedef CreateAdapter< VolumeAdapterX<NUMERATOR, DENOMINATOR> > Adapter;
	typedef CreateBinaryTree< Adapter, N > Creator;
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>create
	// 
	Graph::Janitor::Ptr janitor = graph->getJanitor(); // !! avoid graph update after every create iteration
	Creator ( graph, graph->getStartNode(), graph->getEndNode() );
	janitor.reset();
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> check creation
	CPPUNIT_ASSERT_EQUAL ( (size_t)Creator::NUM_CREATED_ADAPTER, graph->getNumAdapter() );
	static const size_t NUM_BINARY = (1 << N) - 1;
	static const size_t NUM_NODES = Creator::NUM_CREATED_ADAPTER * 3 + NUM_BINARY + 1/*(NUM_BINARY includes startNode)*/;
	CPPUNIT_ASSERT_EQUAL ( NUM_NODES, graph->getNumNodes() );
	CPPUNIT_ASSERT ( graph->isActive() );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> process
	Frames inFrame( blockSize );
	Frames outFrame( blockSize );
	fillFrame ( &inFrame, X,  -X );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>process graph. expect frame num copied = NUM_BINARY - 1
	Frames::num_copyintos = 0; // reset copy_counter
	graph->pushAndCopy ( &inFrame, blockSize );
	graph->processGraph( outFrame.getData(), blockSize  );
	CPPUNIT_ASSERT_EQUAL ( (size_t)Creator::NUM_CREATED_ADAPTER/2, Frames::num_copyintos );
	CPPUNIT_ASSERT_EQUAL ( SUM, isFilledWith<float>( outFrame[0], outFrame.getSize(), SUM ) );
	CPPUNIT_ASSERT_EQUAL ( -SUM, isFilledWith<float>( outFrame[1], outFrame.getSize(), -SUM ) );
}	


//=============================================================================
template < typename A >
void register_types( A &ar ){
	using namespace processing;
	using namespace parameter;
	//graph
	ar.template register_type<parameter::Parameter>();
	//ar.template register_type<parameter::InverseConnection>();
	//ar.template register_type<parameter::ExpConnection>();
	//ar.template register_type<parameter::LogConnection>();
	ar.template register_type<NOPNode>();
	ar.template register_type<ProcessAdapter::OutputNode>();
	ar.template register_type<ProcessAdapter::InputNode>();
	ar.template register_type<StartNode>();
	ar.template register_type<EndNode>();
	ar.template register_type<ProcessAdapterNode>();
	ar.template register_type<Volume>();
	//ar.template register_type<VSTPlugin>();
	ar.template register_type<Pan>();
	ar.template register_type<OutputStep>();
	ar.template register_type<InputStep>();
	ar.template register_type<OutputSwitch>();
	ar.template register_type<InputSwitch>();
	ar.template register_type<PeakTracker>();
	ar.template register_type<ADSRTrigger>();
	ar.template register_type <FadeValue>();
	//                 * 
	// test klassen
	ar.template register_type <HelperNode>();
}

//=============================================================================
void GraphTest::testSerialization() { 
//=============================================================================
	using namespace std;
	using namespace com;
	using namespace processing;
	int blockSize = 512;
	enum { GRAPH_DEPTH=1 };
	Graph::Ptr graph = createGraph( blockSize, 44100.0f );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>create GRAPH_DEPTH VolumeAdapters series:
	// 
	CreateSeries< CreateAdapter<Volume>, GRAPH_DEPTH > 
		series( graph, graph->getStartNode(), graph->getEndNode() );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> check creation
	//                              4 = input + adapter + output + helper ( created by CreateSeries )
	//                              |
	CPPUNIT_ASSERT_EQUAL ( (size_t)(4 * GRAPH_DEPTH + 2), graph->getNumNodes() );
	CPPUNIT_ASSERT ( graph->isActive() );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> process
	Frames inFrame( blockSize );
	Frames outFrame( blockSize );
	fillFrame ( &inFrame, 0.5f, -0.5f );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>process graph. 
	graph->pushAndCopy ( &inFrame, blockSize );
	graph->processGraph( outFrame.getData(), blockSize  );
	CPPUNIT_ASSERT_EQUAL ( (float) 0.5f, isFilledWith<float>( outFrame[0], outFrame.getSize(), 0.5  ) );
	CPPUNIT_ASSERT_EQUAL ( (float)-0.5f, isFilledWith<float>( outFrame[1], outFrame.getSize(), -0.5 ) );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> serialize
	{
		stringstream ss;
		oArchive oar(ss);
		register_types<oArchive>( oar );
		graph->save(oar);
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> clear graph
		graph.reset();
		outFrame.setZero( blockSize );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> de-serialize
		iArchive iar(ss);
		register_types<iArchive>( iar );
		graph = Graph::load( iar, dummyFX );
		CPPUNIT_ASSERT ( graph );
		CPPUNIT_ASSERT_EQUAL ( (size_t)(4 * GRAPH_DEPTH + 2), graph->getNumNodes() );
		CPPUNIT_ASSERT ( graph->isActive() );
	}
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>reset graph. 
	blockSize = 255;
	float sampleRate = 44800.0f;
	inFrame.setSize ( blockSize );
	outFrame.setSize ( blockSize );
	outFrame.setZero ( blockSize );
	fillFrame ( &inFrame, 0.5f, -0.5f );
	Graph::Janitor::Ptr jan = graph->getJanitor();
	dummyFX->setBlockSize(blockSize);
	dummyFX->setSampleRate(sampleRate);
	jan->hostInfoChanged();
	jan.reset();
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>process graph. 
	// volumeAdapter has fadein after de-serialize, so the first out-values grow up from zero 
	for ( int i=0; i<10; ++i ) { 
		fillFrame ( &inFrame, 0.5f, -0.5f );
		graph->pushAndCopy ( &inFrame, blockSize );
		graph->processGraph( outFrame.getData(), blockSize  );
	}
	CPPUNIT_ASSERT_EQUAL ( (float) 0.5f, isFilledWith<float>( outFrame[0], outFrame.getSize(), 0.5  ) );
	CPPUNIT_ASSERT_EQUAL ( (float)-0.5f, isFilledWith<float>( outFrame[1], outFrame.getSize(), -0.5 ) );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>add delayAdapter parallel
	jan = graph->getJanitor();
	enum { DELAY = 105 };
	DelayAdapter<DELAY>::Ptr delay = DelayAdapter<DELAY>::create( graph.get() );
	jan->add( delay );
	jan->connectNodes ( graph->getStartNode(), delay->getInputNode(0) );
	jan->connectNodes ( delay->getOutputNode(0), graph->getEndNode() );
	jan.reset();
	size_t newNumNodes = graph->getNumNodes();
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>test new graph
	CPPUNIT_ASSERT_EQUAL ( (size_t)DELAY, graph->getGraphDelay() );
	// set DCStreams zero
	for ( int i=0; i<100; ++i ) { 
		fillFrame ( &inFrame, 0.0f, 0.0f );
		graph->pushAndCopy ( &inFrame, blockSize );
		graph->processGraph( outFrame.getData(), blockSize  );
	}
	fillFrame ( &inFrame, 0.5f, 0.5f );
	graph->pushAndCopy ( &inFrame, blockSize );
	graph->processGraph( outFrame.getData(), blockSize  );
	TEST_PEAK ( outFrame, 1.0f, DELAY, blockSize );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> serialize
	{
		stringstream ss;
		oArchive oar(ss);
		register_types<oArchive>( oar );
		oar.register_type< DelayAdapter<DELAY> >();
		graph->save(oar);
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> clear graph
		graph.reset();
		outFrame.setZero( blockSize );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> de-serialize
		iArchive iar(ss);
		register_types<iArchive>( iar );
		iar.register_type< DelayAdapter<DELAY> >();
		graph = Graph::load( iar, dummyFX );
		CPPUNIT_ASSERT ( graph );
		CPPUNIT_ASSERT_EQUAL ( (size_t)newNumNodes, graph->getNumNodes() );
		CPPUNIT_ASSERT ( graph->isActive() );
	}
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> test new graph
	jan = graph->getJanitor();
	dummyFX->setBlockSize(blockSize);
	dummyFX->setSampleRate(sampleRate);
	jan->hostInfoChanged();
	jan.reset();

	// volumeAdapter has fadein after de-serialize, so the first out-values grow up from zero 
	for ( int i=0; i<10; ++i ) { 
		fillFrame ( &inFrame, 0.0f, 0.0f );
		graph->pushAndCopy ( &inFrame, blockSize );
		graph->processGraph( outFrame.getData(), blockSize  );
	}

	fillFrame ( &inFrame, 0.5f, 0.5f );
	graph->pushAndCopy ( &inFrame, blockSize );
	graph->processGraph( outFrame.getData(), blockSize  );
	TEST_PEAK ( outFrame, 1.0f, DELAY, blockSize );
}	
} // namespace tests
