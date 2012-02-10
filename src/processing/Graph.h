/*
 ppi.processing
 Signal Processing:
 */
#ifndef GRAPH_H_
#define GRAPH_H_

#include "com/one4All.h"
#include <string>
#include <vector>
#include <list>
#include "com/Events.h"
#include "com/Serialization.h"
#include "audioeffectx.h"
#include "IHostInfo.h"
#include "PObject.h"
#include "parameter/Parameter.h"
#include "processing.h"
#include "IVstEventProcessor.h"
#include "BglGraph.h"
#include <boost/parameter/name.hpp>
#include <boost/parameter/keyword.hpp>
#include <boost/parameter/preprocessor.hpp>


//============================================================================================================
//	Vorwaerts Deklarationen
//============================================================================================================
namespace processing {
	//--------------------------------------------------------------------------------------------------------
	// Die Klasse Graph
	class Graph;
	// ..................................................BOOST_PARAMETER
	BOOST_PARAMETER_NAME(node)    // Note: no semicolon
	BOOST_PARAMETER_NAME(bglVertex)
	BOOST_PARAMETER_NAME(numChildren)

} // namespace

//============================================================================================================
//	Deklerationen
//============================================================================================================
namespace processing {
using namespace events;
using namespace com;
using namespace boost::parameter;
//============================================================================================================
// Klasse GraphChanged
//============================================================================================================
struct GraphChanged : public Event {
	size_t delay;
	GraphChanged ( size_t delay ) : delay(delay) {}
};
//============================================================================================================
//	Klasse: Graph.
//============================================================================================================
class Graph : 
	public EventSender<GraphChanged>, 
	public IVstEventProcessor,
	public IHostInfo
{
friend class boost::serialization::access;
friend class DFSVisitor;
public:
	//--------------------------------------------------------------------------------------------------------
	typedef list<ProcessorNode*> NodeList;
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<Graph> Ptr;
	//--------------------------------------------------------------------------------------------------------
	class Janitor;
	//--------------------------------------------------------------------------------------------------------
	boost::shared_ptr<Janitor> getJanitor();
	//--------------------------------------------------------------------------------------------------------
	static const bgl::Vertex nullVertex;
	//--------------------------------------------------------------------------------------------------------
	static const bgl::Edge nullEdge;
	//--------------------------------------------------------------------------------------------------------
	typedef NodeList SignalProcessPath;
private:
	//--------------------------------------------------------------------------------------------------------
	BOOST_PARAMETER_CONST_MEMBER_FUNCTION (
		  (void),                 // 1. parenthesized return type
		  updateProcessorNode,    // 2. name of the function template

		  tag,                    // 3. namespace of tag types

		  (required (node, (ProcessorNode::Ptr)) )  // 4. one required parameter, and

		  (optional              //  optional parameters, with defaults
		  (bglVertex,           (bgl::Vertex),  nullVertex )
		  (numChildren,         (int),  -1 )
		  )
	  )
	{
		if ( bglVertex != nullVertex ) node->bglVertex = bglVertex;	
		if ( numChildren >= 0 ) node->prepareFrameContainer( numChildren );
	}
	//--------------------------------------------------------------------------------------------------------
	SignalProcessPath signalProcessPath;
	//--------------------------------------------------------------------------------------------------------
	bool _hasCycle;
	//--------------------------------------------------------------------------------------------------------
	bgl::G g;
	//--------------------------------------------------------------------------------------------------------
	bgl::VertexProcessorNode vertexProcessorNode;
	//--------------------------------------------------------------------------------------------------------
	void initBglGraph() { 
		vertexProcessorNode = get( bgl::processor_node_t(), g ); 
	}
	//--------------------------------------------------------------------------------------------------------
	boost::weak_ptr<Janitor> updater;
	//--------------------------------------------------------------------------------------------------------
	bool add ( PObject::Ptr obj );
	//--------------------------------------------------------------------------------------------------------
	bool remove ( PObject::Ptr obj );
	//--------------------------------------------------------------------------------------------------------
	// blockiert processing fuer operationen die graph veraendern
	com::Mutex processingLock;
	//--------------------------------------------------------------------------------------------------------
	template < typename Archive >
	void serialize ( Archive &ar, const unsigned int version ){
		ar & boost::serialization::base_object<IHostInfo> ( *this );
		//ar >> g->startNode;      
		//ar >> g->endNode;
		// muss extern ( nicht in serialize methode ) erfolgen :
		// da entspechende knoten evntl. parentEffect benoetigen.
		// dies ist aber waehrend 'ar >> g;' noch nicht vorhanden.
	}
	//--------------------------------------------------------------------------------------------------------
	Graph () : _hasCycle(false), hostInfo(NULL) { initBglGraph(); }
	//--------------------------------------------------------------------------------------------------------
	typedef list <PObject::Ptr> GraphObjectContainer;
	//--------------------------------------------------------------------------------------------------------
	GraphObjectContainer graphObjects;
	//--------------------------------------------------------------------------------------------------------
	typedef vector<processing::parameter::Parameter::Ptr> ParameterContainer;
	//--------------------------------------------------------------------------------------------------------
	ParameterContainer hostParameter;
	//--------------------------------------------------------------------------------------------------------
	void initHostParameter();
	//--------------------------------------------------------------------------------------------------------
	boost::weak_ptr<Graph> self;
	//--------------------------------------------------------------------------------------------------------
	Graph( IHostInfo *hostInfo );
	//--------------------------------------------------------------------------------------------------------
	// (Re-)Initalisiert Graph:
	// Setzt acitivity flag und anzahl der aktiven childs pro Node. 
	void updateGraph();
protected:
	//--------------------------------------------------------------------------------------------------------
	IHostInfo *hostInfo;
	//--------------------------------------------------------------------------------------------------------
	//Start Knoten
	StartNode::Ptr startNode;
	//--------------------------------------------------------------------------------------------------------
	//End Knoten
	EndNode::Ptr endNode;
public:
	//--------------------------------------------------------------------------------------------------------
	const SignalProcessPath & getSignalProcessPath() const { return signalProcessPath; }
	//--------------------------------------------------------------------------------------------------------
	bool hasCycle() const { return _hasCycle; }
	//--------------------------------------------------------------------------------------------------------
	bgl::Edge findEdge( ProcessorNode::Ptr source, ProcessorNode::Ptr target ) const;
	//--------------------------------------------------------------------------------------------------------
	size_t getNumEdges() const { return boost::num_edges(g); }
	//--------------------------------------------------------------------------------------------------------
	const bgl::G & getBglGraph() const { return g; }
	//--------------------------------------------------------------------------------------------------------
	ProcessorNode::Ptr getProcessorNode( const bgl::Vertex &vertex );
	//--------------------------------------------------------------------------------------------------------
	size_t getNumAdapter() const;
	//--------------------------------------------------------------------------------------------------------
	float getSampleRate() const { return hostInfo->getSampleRate(); }
	//--------------------------------------------------------------------------------------------------------
	int getBlockSize() const { return hostInfo->getBlockSize(); }
	//--------------------------------------------------------------------------------------------------------
	bool isActive() const;
	//--------------------------------------------------------------------------------------------------------
	static Ptr create( IHostInfo *hostInfo );
	//--------------------------------------------------------------------------------------------------------
	com::Mutex & getProcessingLock() { return processingLock; }
	//--------------------------------------------------------------------------------------------------------
	AudioMasterCallback getAudioMasterCallback() { return hostInfo->getAudioMasterCallback(); }
	//--------------------------------------------------------------------------------------------------------
	AudioEffectX * getAudioEffectX() { return hostInfo->getAudioEffectX(); }
	//--------------------------------------------------------------------------------------------------------
	processing::parameter::Parameter::Ptr getHostParameter ( size_t i ) { 
		if ( i > hostParameter.size() ) return parameter::Parameter::Ptr();
		return hostParameter[i]; 
	}
	//--------------------------------------------------------------------------------------------------------
	// fuellt outContainer mit child Nodes von node.
	template < typename Container >
	void getChildNodes ( ProcessorNode::Ptr node, Container &outContainer );
	//--------------------------------------------------------------------------------------------------------
	// fuellt outContainer mit parent Nodes von node.
	template < typename Container >
	void getParentNodes ( ProcessorNode::Ptr node, Container &outContainer );
	//--------------------------------------------------------------------------------------------------------
	size_t getNumHostParameter () { return hostParameter.size(); }
	//--------------------------------------------------------------------------------------------------------
	VstTimeInfo * getVstTimeInfo ( VstInt32 filter) { return hostInfo->getVstTimeInfo(filter); }
	//--------------------------------------------------------------------------------------------------------
	StartNode::Ptr getStartNode();
	//--------------------------------------------------------------------------------------------------------
	EndNode::Ptr getEndNode();
	//--------------------------------------------------------------------------------------------------------
	virtual void processEvents(VstEvents * events);
	//--------------------------------------------------------------------------------------------------------
	// Liefert das Ergebniss Frames des Endknoten.
	// Und Startet damit indirekt die Prozess Ablaufkette. 
	void processGraph( float **outputs, Processor::Int numSamples );
	//--------------------------------------------------------------------------------------------------------
	void pushAndCopy( Frames *fr, Processor::Int numSamples );
	//--------------------------------------------------------------------------------------------------------
	bool contains(PObject::Ptr obj) const;
	//--------------------------------------------------------------------------------------------------------
	~Graph();
	//--------------------------------------------------------------------------------------------------------
	size_t getNumNodes() { return boost::num_vertices(g); }
	//--------------------------------------------------------------------------------------------------------
	void save ( oArchive &ar );
	//--------------------------------------------------------------------------------------------------------
	static Graph::Ptr load ( iArchive &ar, IHostInfo *hostInfo );
	//--------------------------------------------------------------------------------------------------------
	size_t getGraphDelay();
}; // class Graph
//============================================================================================================
// Graph Template Methoden
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
template < typename Container >
void Graph::getChildNodes ( ProcessorNode::Ptr node, Container &outContainer ) {
	bgl::AdjacencyIterator ai, end;
	boost::tie( ai, end ) = boost::adjacent_vertices ( node->getBglVertex(), g );
	for ( ; ai!=end; ++ai ) {
		ProcessorNode::Ptr n = vertexProcessorNode[*ai];
		outContainer.push_back(n);
	}
}
//------------------------------------------------------------------------------------------------------------
template < typename Container >
void Graph::getParentNodes ( ProcessorNode::Ptr node, Container &outContainer ) {
	bgl::InvAdjacencyIterator ai, end;
	boost::tie( ai, end ) = boost::inv_adjacent_vertices ( node->getBglVertex(), g );
	for ( ; ai!=end; ++ai ) {
		ProcessorNode::Ptr n = vertexProcessorNode[*ai];
		outContainer.push_back(n);
	}
}
//============================================================================================================
// Klasse Graph::Janitor
// Ermoeglicht hinzufuegen und entfernen von PObjects und Verbindungen.
// da nebenl. op. sperrt Janitor Obj. den Processing Vorgang solange obj. am leben.
//============================================================================================================
class Graph::Janitor {
friend class Graph;
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<Janitor> Ptr;
	//--------------------------------------------------------------------------------------------------------
	enum State { SUCCEED, FAILED };
private:
	//--------------------------------------------------------------------------------------------------------
	Graph *graph;
	//--------------------------------------------------------------------------------------------------------
	Janitor ( Graph *graph );
	//--------------------------------------------------------------------------------------------------------
	bool _hostInfoChanged;
	//--------------------------------------------------------------------------------------------------------
	State addProcessorNode( ProcessorNode::Ptr obj );
	//--------------------------------------------------------------------------------------------------------
	State removeProcessorNode ( ProcessorNode::Ptr obj );
	//--------------------------------------------------------------------------------------------------------
	void removeAdjacencyEdges( ProcessorNode::Ptr obj );
	//--------------------------------------------------------------------------------------------------------
	void updateProcessorNodeVertexRelations();
public:
	//--------------------------------------------------------------------------------------------------------
	virtual ~Janitor ();
	//--------------------------------------------------------------------------------------------------------
	State add ( NOPNode::Ptr obj ) { 
		if ( !graph->add(obj) ) return FAILED;
		return addProcessorNode(obj); 
	} 
	//--------------------------------------------------------------------------------------------------------
	State add ( ProcessAdapter::Ptr obj );
	//--------------------------------------------------------------------------------------------------------
	State add ( ProcessAdapter::InputNode::Ptr obj );
	//--------------------------------------------------------------------------------------------------------
	State add ( ProcessAdapter::OutputNode::Ptr obj );
	//--------------------------------------------------------------------------------------------------------
	State add ( parameter::Parameter::Ptr obj );
	//--------------------------------------------------------------------------------------------------------
	State remove ( ProcessAdapter::Ptr obj );
	//--------------------------------------------------------------------------------------------------------
	State remove ( ProcessAdapter::InputNode::Ptr obj ) {
		if ( !graph->remove(obj) ) return FAILED;
		return removeProcessorNode( obj );
	}
	//--------------------------------------------------------------------------------------------------------
	State remove ( ProcessAdapter::OutputNode::Ptr obj ) {
		if ( !graph->remove(obj) ) return FAILED;
		return removeProcessorNode( obj );
	}
	//--------------------------------------------------------------------------------------------------------
	State remove ( parameter::Parameter::Ptr obj );
	//--------------------------------------------------------------------------------------------------------
	State remove ( NOPNode::Ptr obj ) { 
		if ( !graph->remove(obj) ) return FAILED;
		return removeProcessorNode(obj); 
	} 
	//--------------------------------------------------------------------------------------------------------
	void hostInfoChanged();
	//--------------------------------------------------------------------------------------------------------
	// TODO: irgendwann kann die * variante mal der ::Ptr variante weichen
	State connectNodes( ProcessorNode *parent, ProcessorNode *child) {
		ProcessorNode::Ptr p = boost::shared_dynamic_cast< ProcessorNode, PObject > ( parent->getPtr() );
		ProcessorNode::Ptr c = boost::shared_dynamic_cast< ProcessorNode, PObject > ( child->getPtr() );
		if ( !p || !c ) return FAILED;
		return connectNodes( p, c );
	}
	//--------------------------------------------------------------------------------------------------------
	// Erzeugt die gerichtete verbindung : output->input.
	// Prueft ob graph valid (keine zykel).
	// Wenn nicht wird verbindung wieder aufgehoben und FAILED zurueckgegeben.
	// Ansonsten: SUCCEED.
	State connectNodes( ProcessorNode::Ptr parent, ProcessorNode::Ptr child );
	//--------------------------------------------------------------------------------------------------------
	// TODO: irgendwann kann die * variante mal der ::Ptr variante weichen
	State removeConnection( ProcessorNode *parent, ProcessorNode *child) {
		ProcessorNode::Ptr p = boost::shared_dynamic_cast< ProcessorNode, PObject > ( parent->getPtr() );
		ProcessorNode::Ptr c = boost::shared_dynamic_cast< ProcessorNode, PObject > ( child->getPtr() );
		if ( !p || !c ) return FAILED;
		return removeConnection( p, c );
	}
	//--------------------------------------------------------------------------------------------------------
	// Erzeugt die gerichtete verbindung : output->input.
	// Prueft ob graph valid (keine zykel).
	// Wenn nicht wird verbindung wieder aufgehoben und FAILED zurueckgegeben.
	// Ansonsten: SUCCEED.
	State removeConnection( ProcessorNode::Ptr parent, ProcessorNode::Ptr child );
}; // Janitor

//============================================================================================================
// Klasse: DFSVisitor.
//============================================================================================================
class DFSVisitor : public boost::dfs_visitor<> {
private:
	//--------------------------------------------------------------------------------------------------------
	bool endFinalized;
	//--------------------------------------------------------------------------------------------------------
	Graph *graph;
public:
	//--------------------------------------------------------------------------------------------------------
	DFSVisitor( Graph *graph );
	//--------------------------------------------------------------------------------------------------------
	template <class Edge, class Graph>
	void back_edge(Edge, Graph&) { graph->_hasCycle = true; }
	//--------------------------------------------------------------------------------------------------------
	template <class Vertex, class Graph>
	void discover_vertex(const Vertex &v, Graph&) {
		ProcessorNode::Ptr n = graph->vertexProcessorNode[v];
	}
	//--------------------------------------------------------------------------------------------------------
	void addToSignalProcessPath ( ProcessorNode::Ptr n );
	//--------------------------------------------------------------------------------------------------------
	void processNodeParents ( ProcessorNode::Ptr n );
	//--------------------------------------------------------------------------------------------------------
	template <class Vertex, class Graph>
	void finish_vertex(const Vertex &v, Graph&) {
		ProcessorNode::Ptr n = graph->vertexProcessorNode[v];
		if ( n == graph->getStartNode() ) {
			n->setActive( true );
			return;
		}
		if ( n == graph->getEndNode() ) {
			endFinalized = true;
			if ( boost::in_degree( v, graph->g ) == 0 ) return;
			n->setActive( true );
			addToSignalProcessPath( n );
			return;
		}
		// alles nach endFinalized ist inaktiv
		if ( endFinalized ) return;
		n->setActive( true );
		addToSignalProcessPath( n );
	}
	//--------------------------------------------------------------------------------------------------------
	template <class Vertex, class Graph>
	void initialize_vertex(const Vertex &v, Graph&) {
		ProcessorNode::Ptr n = graph->vertexProcessorNode[v];
		n->setActive( false );
		n->parents.clear();
		n->activeChildren = 0;
		n->tmpFrames->setSize( graph->getBlockSize() );
	}
};
} // namespace Processing

#endif