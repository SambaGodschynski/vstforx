#include <iostream>
#include "windows.h"
#include "com/One4All.h"
#include "com/settings.h"
#include "tests/GraphBuilder.hpp"
#include "processing/ConcreteProcessAdapter.h"
#include "tests/DummyFX.h"
#include "boost/timer.hpp"
#include "processing/Graph.h"

using namespace std;
using namespace processing;

inline void waitForKey() {
	char bff[255];
	scanf_s ( "%c", &bff );
}

DummyFX *dummyFX = NULL;

const string SEPERATOR1("===================================");
const string SEPERATOR2("-----------------------------------");

//-----------------------------------------------------------------------------
processing::Graph::Ptr createGraph( int blockSize, float samplerate ) 
{
//-----------------------------------------------------------------------------
	Graph::Ptr graph = Graph::create ( dummyFX, NULL );
	Graph::Janitor::Ptr janitor = graph->getJanitor();
	janitor->setSampleRate ( samplerate );
	janitor->setBlockSize ( blockSize );
	return graph;
}
//-----------------------------------------------------------------------------
// start timer
//-----------------------------------------------------------------------------
inline void startTimer( const string & message, boost::timer & timer ) {
	cout<<"start "<<message<<" ...";
	timer.restart();
}
//-----------------------------------------------------------------------------
// stop timer
//-----------------------------------------------------------------------------
inline void stopTimer( boost::timer & timer ) {
	double t = timer.elapsed();
	cout<<t<<" s"<<endl;
}
//-----------------------------------------------------------------------------
// creates complex graph and does process measurement with depth 1..N
//-----------------------------------------------------------------------------
template <typename Creator>
void buildAndProcessGraph() {
	buildAndProcessGraph< Creator::N_LESS_1 >();
	if ( Creator::INDEX == 0 ) return;
	boost::timer timer;
	int blockSize = 512;
	Graph::Ptr graph = createGraph ( blockSize, 44100.0f );
	Graph::Janitor::Ptr janitor = graph->getJanitor();
	cout<<"graph ( N = "<<Creator::INDEX<<", NUM_ADAPTER = " 
		<< Creator::NUM_CREATED_ADAPTER <<" )"<<endl;
	// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>build tree
	startTimer("build graph", timer);
	Creator ( graph, graph->getStartNode(), graph->getEndNode() );
	stopTimer(timer);
	startTimer("update graph", timer);
	janitor.reset();
	stopTimer(timer);
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> process
	Frame inFrame( blockSize );
	Frame outFrame( blockSize );
	graph->pushAndCopy ( &inFrame, blockSize );
	startTimer("process graph", timer);
	graph->processGraph( outFrame.getData(), blockSize  );
	stopTimer(timer);
	// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>test finished
	cout<<SEPERATOR2<<endl;	
}
template <>
void buildAndProcessGraph<NullType>() {
}
//-----------------------------------------------------------------------------
template < typename Creator >
void startTest ( const string & name ) {
	cout<<SEPERATOR1<<endl;
	cout<<name<<endl;
	cout<<SEPERATOR1<<endl;
	buildAndProcessGraph<Creator>();
	cout<<endl;
}
//-----------------------------------------------------------------------------
int main ( int argc, char ** argv ) {
	cout<<"* VSTForx Performance Measurement *"<<endl;
	cout<<"version: " << com::Settings::versionToString() << endl << endl;
	
	dummyFX = new DummyFX( NULL );
	typedef CreateAdapter<VolumeAdapter> Adapter;
	// runs test(s)
	//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	startTest< CreateParallel< CreateSeries < Adapter, 15 >, 15 > >("complex1");
	//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	startTest<CreateSeries< CreateParallel< Adapter, 15 >, 15 > > ("complex2");
	//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	startTest< CreateBinaryTree< Adapter, 15 > > ("binary_tree1");
	//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	
	// thats it
	cout<<"over and out..."<<endl;
	delete dummyFX; dummyFX = NULL;
	waitForKey();
}

//-----------------------------------------------------------------------------
std::string getHomeDirectory() {
//-----------------------------------------------------------------------------
	using namespace com;
	std::string home_dir;
	const size_t N = 512; 
	char _d[N];
	DWORD r = GetModuleFileName ( NULL, &_d[0], N );
	com::Filename f( _d  );
	if ( is_regular_file(f) ) home_dir = f.remove_filename().string();
	else home_dir = f.directory_string();
	return home_dir;
}