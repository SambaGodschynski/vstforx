/*
 * ===========================================================================================================
 * Graph.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */


#include <boost/graph/reverse_graph.hpp> // occurs compiler error: 'declaration does not declare anything'
										 // when declared after '#include "Graph.h"' on XCode with gcc4.0
      // http://stackoverflow.com/questions/8173620/c-boost-1-48-type-traits-and-cocoa-inclusion-weirdness

#include "Graph.h"
#include "OS_Specific/OS_com.h"
#include "OS_Specific/OS_processing.h"

namespace processing {

using namespace parameter;
using namespace com; 
//============================================================================================================
// class DFSVisitor
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
DFSVisitor::DFSVisitor( Graph *graph ) : 
graph(graph),
endFinalized(false)
{
	graph->_hasCycle=false;
	graph->signalProcessPath.clear();
}
//------------------------------------------------------------------------------------------------------------
// aktualsiert parent activeChildren; setzt node->parents; ermittelt delay
void DFSVisitor::processNodeParents( ProcessorNode::Ptr node ) {
	bgl::InvAdjacencyIterator i, end;
	boost::tie( i, end ) = boost::inv_adjacent_vertices( node->getBglVertex(), graph->g );
	size_t delay = node->getProcessDelay();
	
	// anzahl der eingehenden kanten
	boost::graph_traits<bgl::G>::degree_size_type inDegree = 
		boost::in_degree( node->getBglVertex(), graph->g );

	for ( ; i!=end; ++i ) { // alle parents:
		ProcessorNode::Ptr n = graph->vertexProcessorNode[*i];
		++(n->activeChildren); // erhoehe activeChidren des vorgaenger, da node active
		if ( n->isActive() ) node->parents.push_back( n.get() );
		// nur ein parent sprich: serielle verknuepfung ?
		if ( inDegree == 1 ) { // ja => addiere delay wert
			delay += n->getNodeDelay();
		} else { // nein => uebernehme max. wert
			if ( n->getNodeDelay() > delay ) 
				delay = n->getNodeDelay();
		}
	}
	// setze delay
	node->setNodeDelay(delay);
	// TODO:
	// not every node needs a dc stream:
	//   - nodes with parent <= 1
	//     BUT: Adapter::InputNodes still needs 
	node->getDCStream().setSize ( graph->getBlockSize(), delay );
}
//------------------------------------------------------------------------------------------------------------
void DFSVisitor::addToSignalProcessPath( ProcessorNode::Ptr node ) {
	if ( !node->isActive() ) return;
	processNodeParents( node );
	graph->signalProcessPath.push_back( node.get() );
}
//============================================================================================================
// class Graph
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
const bgl::Vertex Graph::nullVertex = bgl::Vertex();
//------------------------------------------------------------------------------------------------------------
const bgl::Edge Graph::nullEdge = bgl::Edge();
//------------------------------------------------------------------------------------------------------------
Graph::Graph( IHostInfo *hostInfo ) : 
hostInfo( hostInfo ),
_hasCycle(false)
{
	initBglGraph();
	startNode = StartNode::create();
	endNode = EndNode::create();
	Janitor::Ptr j = getJanitor();
	j->add ( startNode );
	j->add ( endNode );
	initHostParameter();
}
//------------------------------------------------------------------------------------------------------------
ProcessorNode::Ptr Graph::getProcessorNode( const bgl::Vertex &vertex ) {
	return vertexProcessorNode[vertex];
}
//------------------------------------------------------------------------------------------------------------
size_t Graph::getNumAdapter() const {
	size_t n = 0;
	GraphObjectContainer::const_iterator it = graphObjects.begin();
	for ( ; it!=graphObjects.end(); ++it ) {
		if ( dynamic_cast<ProcessAdapter*> ( it->get() ) ) ++n;
	}
	return n;
}
//------------------------------------------------------------------------------------------------------------
bool Graph::isActive() const {
	return endNode->isActive();
}
//------------------------------------------------------------------------------------------------------------
bool Graph::add( PObject::Ptr obj){
	if ( !obj ) return false;
	if ( contains(obj) ) return false;
	if ( obj->isOwned() ) 
		throw ppiError::OwnerLock("Object owned by another Graph.", __FILE__, __LINE__ );
	obj->owned = true;
	graphObjects.push_back (obj);
	return true;
}
//------------------------------------------------------------------------------------------------------------
bool Graph::remove( PObject::Ptr obj){
	if ( !contains(obj) ) return false;
	obj->owned = false;
	graphObjects.remove (obj);
	return true;
}
//------------------------------------------------------------------------------------------------------------
bool Graph::contains ( PObject::Ptr obj ) const {
	if ( com::contains < GraphObjectContainer > ( graphObjects, obj ) ) return true;
	return false;
}
//------------------------------------------------------------------------------------------------------------
void Graph::processGraph( float **outputs, Processor::Int numSamples ){
	if ( endNode->isActive() ) {
		SignalProcessPath::iterator it = signalProcessPath.begin();
		for ( ; it!=signalProcessPath.end(); ++it ) { // process path
			(*it)->processNode( numSamples );
		}
		endNode->getDCStream().flush( numSamples, outputs );
	}
}
//------------------------------------------------------------------------------------------------------------
StartNode::Ptr Graph::getStartNode() {
	return startNode;
}
//------------------------------------------------------------------------------------------------------------
EndNode::Ptr Graph::getEndNode() {
	return endNode;
}
//------------------------------------------------------------------------------------------------------------
void Graph::pushAndCopy ( Frames *fr, Processor::Int numSamples ){
	if (!endNode->isActive()) return;
	startNode->pushAndCopy ( fr, numSamples );
}
//------------------------------------------------------------------------------------------------------------
void Graph::updateGraph() {
	DFSVisitor vis(this);
	boost::reverse_graph<bgl::G> rg(g);
	boost::depth_first_search( 
		rg, 
		boost::visitor(vis).	// !! http://www.boost.org/doc/libs/1_46_1/libs/graph/doc/bgl_named_params.html
		root_vertex( endNode->getBglVertex() ) 
	);
	/*
	EventSender<GraphChanged>::notifyEventListeners( this, GraphChanged( getGraphDelay() ) );*/
}
//------------------------------------------------------------------------------------------------------------
void Graph::initHostParameter(){
	for ( int i=0; i<com::Settings::PROGRAM_PARAMETER; ++i ){
		Parameter::Ptr p = parameter::Parameter::create (i);
		p->setName ( "host par. " + MyString(i + 1) );
		hostParameter.push_back (p);
		add( p );
	}
}
//------------------------------------------------------------------------------------------------------------
Graph::~Graph(){
	com::MethodMessage<Graph> methodMessage ("~Graph()");
}
//------------------------------------------------------------------------------------------------------------
size_t Graph::getGraphDelay() {
	return getEndNode()->getNodeDelay();
}
//------------------------------------------------------------------------------------------------------------
Graph::Ptr Graph::create( IHostInfo *hostInfo ) {
	Graph::Ptr neu( new Graph( hostInfo ) );
	neu->self = neu;
	return neu;
}
//------------------------------------------------------------------------------------------------------------
void Graph::save(oArchive &ar) const {
	com::MethodMessage<Graph> methodMessage ( "save()");
	Graph::Ptr _this = self.lock();
	ar << _this;
	ar << startNode;
	ar << endNode;
	ar << g;
	ar << graphObjects;
	// hostParameter
	ar<<hostParameter;
}
//------------------------------------------------------------------------------------------------------------
Graph::Ptr Graph::load( iArchive &ar, IHostInfo *hostInfo ) {
	com::MethodMessage<Graph> methodMessage ( "load()");
	Graph::Ptr graph;
	ar >> graph; 
	graph->self = graph;
	graph->hostInfo = hostInfo;
	
	ar >> graph->startNode;      
	ar >> graph->endNode;
	
	ar >> graph->g;
	ar >> graph->graphObjects;
	
	// hostParameter
	ar>>graph->hostParameter;
	
	graph->getJanitor()->updateProcessorNodeVertexRelations(); // impl. updateGraph()

	return graph;
}
//------------------------------------------------------------------------------------------------------------
void Graph::processEvents(VstEvents * events) {
	GraphObjectContainer::iterator it = graphObjects.begin();
	for ( ; it!=graphObjects.end(); ++it ){
		IVstEventProcessor *pr = dynamic_cast<IVstEventProcessor*> ( it->get() );
		if ( pr ) pr->processEvents( events );
	}
}
//------------------------------------------------------------------------------------------------------------
Graph::Janitor::Ptr Graph::getJanitor() {
	Janitor::Ptr up = updater.lock();
	if (up) return up;
	up = Janitor::Ptr ( new Janitor( this ) );
	updater = up;
	return up;
}
//------------------------------------------------------------------------------------------------------------
bgl::Edge Graph::findEdge( ProcessorNode::Ptr source, ProcessorNode::Ptr target ) const {
	bgl::EdgeIterator ei, end;
	boost::tie( ei, end ) = boost::edges( g );
	for ( ; ei!=end; ++ei ) {
		if ( 
			boost::source( *ei, g ) == source->getBglVertex() &&
			boost::target( *ei, g ) == target->getBglVertex()
		) return *ei;
	}
	return nullEdge;
}
//============================================================================================================
// Klasse Janitor
// Ermoeglicht hinzufuegen und entfernen von PObjects und Verbindungen.
// da nebenl. op. sperrt Janitor Obj. den Processing Vorgang solange obj. am leben.
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
Graph::Janitor::Janitor(processing::Graph *graph) : 
graph(graph), 
_hostInfoChanged(false)
{
	if ( !graph->self.lock() ) return;
	graph->getProcessingLock().lock(); // sperre processing
}
//------------------------------------------------------------------------------------------------------------
Graph::Janitor::~Janitor() {
	if ( !graph->self.lock() ) return;
	if ( _hostInfoChanged  ) { // samplerate oder blockisze geandert
		GraphObjectContainer::iterator it = graph->graphObjects.begin();
		for ( ; it!=graph->graphObjects.end(); ++it ){
			(*it)->hostInfoChanged();
		}
	}
	/*if ( graph->getBlockSize() > 0 )*/ graph->updateGraph();
	graph->getProcessingLock().unlock(); // entsperre processing
}
//------------------------------------------------------------------------------------------------------------
void Graph::Janitor::hostInfoChanged() {
	if ( graph->getBlockSize() <= 0 ) 
		throw ppiError::InvalidBlockSize
			("Invalid blockSize: " + MyString( graph->getBlockSize() ), __FILE__, __LINE__ ); 

	if ( graph->getSampleRate() <= 0 ) 
		throw ppiError::InvalidSampleRate
			("Invalid sampleRate: " + MyString( graph->getBlockSize() ), __FILE__, __LINE__ ); 
	
	_hostInfoChanged = true;
}
//------------------------------------------------------------------------------------------------------------
Graph::Janitor::State Graph::Janitor::connectNodes( ProcessorNode::Ptr parent, ProcessorNode::Ptr child ) {

	if ( !parent || !child ) return FAILED;
	if ( !com::contains<Graph::GraphObjectContainer>( graph->graphObjects, parent ) ) return FAILED;
	if ( !com::contains<Graph::GraphObjectContainer>( graph->graphObjects, child ) ) return FAILED;
	
	ProcessorNode::Ptr a = graph->vertexProcessorNode[parent->getBglVertex()];
	ProcessorNode::Ptr b = graph->vertexProcessorNode[child->getBglVertex()];
	
	if ( graph->findEdge( parent, child ) != Graph::nullEdge ) return FAILED;
	bgl::Edge e; 
	bool inserted = false;
	boost::tie(e, inserted) = boost::add_edge( parent->getBglVertex(), 
		                                       child->getBglVertex(), 
											   graph->g );
	if ( !inserted ) return FAILED;

	// updateGraph
	graph->updateGraph();
	
	if ( graph->hasCycle() ) {
		removeConnection( parent, child );
		return FAILED;
	}
	
	// prepare parent ProcessorNode
	size_t numCh = parent->getFramesContainer().size();
	graph->updateProcessorNode( parent, _numChildren = ++numCh );

	return SUCCEED;
}
//------------------------------------------------------------------------------------------------------------
Graph::Janitor::State Graph::Janitor::removeConnection( ProcessorNode::Ptr parent, ProcessorNode::Ptr child ) {
	if ( !parent || !child ) return FAILED;
	bgl::Edge e = graph->findEdge( parent, child );
	if ( e == Graph::nullEdge ) return FAILED;
	boost::remove_edge( e, graph->g );
	
	// prepare parent ProcessorNode
	size_t numCh = parent->getFramesContainer().size();
	graph->updateProcessorNode( parent, _numChildren = --numCh );
	
	return SUCCEED;
}
//------------------------------------------------------------------------------------------------------------
Graph::Janitor::State Graph::Janitor::addProcessorNode( ProcessorNode::Ptr obj ) {
	const bgl::Vertex &v = boost::add_vertex( graph->g );
	graph->vertexProcessorNode[v] = obj;
	graph->updateProcessorNode ( obj, _bglVertex = v );
	return SUCCEED;
}
//------------------------------------------------------------------------------------------------------------
void Graph::Janitor::removeAdjacencyEdges ( ProcessorNode::Ptr obj  ) {
	bgl::Vertex v = obj->getBglVertex();
	// entferne parent verbindungen
	bgl::InEdgeIterator iEnd, iNext;
	boost::tie(iNext, iEnd) = boost::in_edges ( v, graph->g );
	while ( iNext != iEnd ) {
		boost::remove_edge( *iNext, graph->g );
		boost::tie(iNext, iEnd) = boost::in_edges ( v, graph->g );
	}
	// entferne child verbindungen
	bgl::OutEdgeIterator oEnd, oNext;
	boost::tie(oNext, oEnd) = boost::out_edges ( v, graph->g );
	while ( oNext != oEnd ) {
		boost::remove_edge( *oNext, graph->g );
		boost::tie(oNext, oEnd) = boost::out_edges ( v, graph->g );
	}
}
//------------------------------------------------------------------------------------------------------------
void Graph::Janitor::updateProcessorNodeVertexRelations() {
	// update ProcessorNode nach Vertex 
	bgl::VertexIterator vi,end;
	boost::tie( vi, end ) = vertices( graph->g );
	for ( ; vi!=end; ++vi ) {
		graph->updateProcessorNode ( graph->vertexProcessorNode[*vi], _bglVertex = *vi );
	}
}
//------------------------------------------------------------------------------------------------------------
Graph::Janitor::State Graph::Janitor::removeProcessorNode( ProcessorNode::Ptr obj ) {
	bgl::Vertex v = obj->getBglVertex();

	removeAdjacencyEdges( obj );
	
	// entferne vertex
	boost::remove_vertex( v, graph->g );
	
	updateProcessorNodeVertexRelations();
	
	graph->updateGraph();
	return SUCCEED;
}
//------------------------------------------------------------------------------------------------------------
Graph::Janitor::State Graph::Janitor::add( ProcessAdapter::Ptr p ) {
	if ( !graph->add(p) ) return FAILED;

	// AdapterNode unbed. zuerst:
	graph->add( p->getAdapterNode() );
	addProcessorNode( p->getAdapterNode() ); // add ProcessAdapter::AdapterNode

	for ( size_t i=0; i<p->getNumInputNodes(); ++i ) {
		ProcessAdapter::InputNode::Ptr node = p->getInputNode(i);
		add( node ); // add ProcessAdapter::InputNode :-> addProcessorNode -> connect( input, adapter )
	}
	for ( size_t i=0; i<p->getNumOutputNodes(); ++i ) {
		ProcessAdapter::OutputNode::Ptr node = p->getOutputNode(i);
		add( node ); // add ProcessAdapter::OutputNode :-> addProcessorNode -> connect( adapter, output )
	}

	return SUCCEED;
}
//------------------------------------------------------------------------------------------------------------
Graph::Janitor::State Graph::Janitor::remove( ProcessAdapter::Ptr p ) {
	if ( !graph->remove(p) ) return FAILED;
	
	for ( size_t i=0; i<p->getNumInputNodes(); ++i ) {
		ProcessAdapter::InputNode::Ptr node = p->getInputNode(i);
		remove( node ); // remove ProcessAdapter::InputNode
	}
	for ( size_t i=0; i<p->getNumOutputNodes(); ++i ) {
		ProcessAdapter::OutputNode::Ptr node = p->getOutputNode(i);
		remove(node); // remove ProcessAdapter::OutputNode
	}

	removeProcessorNode( p->getAdapterNode() ); // remove ProcessAdapter::AdapterNode
	graph->remove( p->getAdapterNode() );

	return SUCCEED;
}
//------------------------------------------------------------------------------------------------------------
Graph::Janitor::State Graph::Janitor::add( ProcessAdapter::InputNode::Ptr p ) {
	if ( !graph->add(p) ) return FAILED;
	addProcessorNode( p );
	connectNodes( p, p->getProcessAdapter()->getAdapterNode() );
	return SUCCEED;
}
//------------------------------------------------------------------------------------------------------------
Graph::Janitor::State Graph::Janitor::add( ProcessAdapter::OutputNode::Ptr p ) {
	if ( !graph->add(p) ) return FAILED;
	addProcessorNode( p );
	connectNodes( p->getProcessAdapter()->getAdapterNode(), p );
	return SUCCEED;
}
//------------------------------------------------------------------------------------------------------------
Graph::Janitor::State Graph::Janitor::add( Parameter::Ptr p ) {
	if ( !graph->add(p) ) return FAILED;
	return SUCCEED;
}
//------------------------------------------------------------------------------------------------------------
Graph::Janitor::State Graph::Janitor::remove( Parameter::Ptr p ) {
	if ( !graph->remove(p) ) return FAILED;
	return SUCCEED;
}
} // namespace processing
