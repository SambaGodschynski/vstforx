/*
 * ===========================================================================================================
 * Graph.h
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#ifndef GRAPH_H_
#define GRAPH_H_

#include "com/one4All.h"
#include <string>
#include <vector>
#include <list>
#include "com/Events.h"
#include "com/Serialization.h"
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
	/**
	 * Die Klasse Graph
	 */
	class Graph;
	/**
	 * 	BOOST_NAMED_PARAMETER
	 * 	http://www.boost.org/doc/libs/1_48_0/libs/parameter/doc/html/index.html
	 */
	BOOST_PARAMETER_NAME(node)    // Note: no semicolon
	BOOST_PARAMETER_NAME(bglVertex)
	BOOST_PARAMETER_NAME(numChildren)

} // namespace

//============================================================================================================
//	Deklarationen
//============================================================================================================
namespace processing {
using namespace events;
using namespace com;
using namespace boost::parameter;
//============================================================================================================
// Klasse GraphChanged
//============================================================================================================
/**
 * @class GraphChanged Event-Klasse.
 */
struct GraphChanged : public Event {
	size_t delay;
	GraphChanged ( size_t delay ) : delay(delay) {}
};
//============================================================================================================
/**
 * @class Graph beinhaltet nicht nur die Graph-Implementation (BGL) und dessen Objekte, sondern dient auch
 * als Kontainer, für die Prozesslogik Objekte (PObject), die nur indirekt Graphangehoerig sind
 * (zb. Parameter)
 */
class Graph : 
	public EventSender<GraphChanged>, 
	public IVstEventProcessor,
	public IHostInfo
//============================================================================================================
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
	static const bgl::Vertex nullVertex;
	//--------------------------------------------------------------------------------------------------------
	static const bgl::Edge nullEdge;
	//--------------------------------------------------------------------------------------------------------
	typedef NodeList SignalProcessPath;
private:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Boost Named Parameter Methode. Bietet Operationen um ProcessorNode-Informationen zu
	 * aktualiseren.
	 * Aufruf zb.: updateProcessorNode(node, _bglVertex=xxx)
	 */
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
		if ( numChildren >= 0 ) node->prepareFramesContainer( numChildren );
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * enthaelt topologisch sortierte ProcessNode-Objekte
	 */
	SignalProcessPath signalProcessPath;
	//--------------------------------------------------------------------------------------------------------
	bool _hasCycle;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * BGL-Objekt
	 */
	bgl::G g;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Vertex nach ProcessorNode Abbildung.
	 */
	bgl::VertexProcessorNode vertexProcessorNode; // TODO: rename vertex2ProcessorNode
	//--------------------------------------------------------------------------------------------------------
	/**
	 * initalisiert BGL-Graph
	 */
	void initBglGraph() { 
		// hole vertex nach ProcessorNode map
		vertexProcessorNode = get( bgl::processor_node_t(), g ); 
	}
	//--------------------------------------------------------------------------------------------------------
	boost::weak_ptr<Janitor> updater;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Fuegt PObject hinzu. Wird durch Janitor-Objekt aufgerufen.
 	 * @param obj
	 * @return true, wenn erfolgt
	 */
	bool add ( PObject::Ptr obj );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Entfernt PObject. Wird durch Janitor-Objekt aufgerufen.
 	 * @param obj
	 * @return true, wenn erfolgt
	 */
	bool remove ( PObject::Ptr obj );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * blockiert processGraph() gegen Janitor-Objekt
	 */
	com::Mutex processingLock;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * De/Serialisiert Graph-Objekt.
	 * PObject-Objekte werden ueber die Methoden: save() bzw. load()[statisch] De/Serialisiert.
	 * @param ar Archive-Objekt
	 * @param version
	 */
	template < typename Archive >
	void serialize ( Archive &ar, const unsigned int version ){
		ar & boost::serialization::base_object<IHostInfo> ( *this );
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
	/**
	 * initalisiert Hostparameter
	 */
	void initHostParameter();
	//--------------------------------------------------------------------------------------------------------
	/**
	 * smart_pointer this ersatz
	 */
	boost::weak_ptr<Graph> self;
	//--------------------------------------------------------------------------------------------------------
	Graph( IHostInfo *hostInfo );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * erstellt SignalProcessPath
	 */
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
	/**
	 * @return SignalProcessPath
	 */
	const SignalProcessPath & getSignalProcessPath() const { return signalProcessPath; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 *
	 * @return true, wenn graph zyklisch
	 */
	bool hasCycle() const { return _hasCycle; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param source
	 * @param target
	 * @return BGL Kanten Objekt zur ProcessorNode verbindung source->traget, falls exsistent.
	 * 		   liefert Ansonsten nullEdge Objekt.
	 */
	bgl::Edge findEdge( ProcessorNode::Ptr source, ProcessorNode::Ptr target ) const;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return die Anzahl aller Verbindungen
	 */
	size_t getNumEdges() const { return boost::num_edges(g); }
	//--------------------------------------------------------------------------------------------------------
	/**
	 *
	 * @return BGL-Graph Objekt
	 */
	const bgl::G & getBglGraph() const { return g; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param vertex
	 * @return BGL-Vertex zugehöriges ProcessorNode Objekt. Wird nicht auf existenz geprüft(TODO)!
	 */
	ProcessorNode::Ptr getProcessorNode( const bgl::Vertex &vertex );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Anzahl aller ProcessAdapter
	 */
	size_t getNumAdapter() const;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return aktuele Samplerate
	 */
	float getSampleRate() const { return hostInfo->getSampleRate(); }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return aktuelle Sampleblock groesse
	 */
	int getBlockSize() const { return hostInfo->getBlockSize(); }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return true, wenn anzahl aktiver ProcessNode-Objekte in SignalProcessPath > 0
	 */
	bool isActive() const;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param hostInfo Objekt
	 * @return Graph Objekt
	 */
	static Ptr create( IHostInfo *hostInfo );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Processing Mutex
	 */
	com::Mutex & getProcessingLock() { return processingLock; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return AudioMasterCallback Funktionszeiger (kommunikation plugin->host)
	 */
	AudioMasterCallback getAudioMasterCallback() { return hostInfo->getAudioMasterCallback(); }
	//--------------------------------------------------------------------------------------------------------
	/**
	 *
	 * @return AudioEffectX-Objekt des Clients
	 */
	AudioEffectX * getAudioEffectX() { return hostInfo->getAudioEffectX(); }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param i
	 * @return i. Host Parameter-Objekt (Vst-Parameter des Clients). Liefert NULL falls i > Anzahl.
	 */
	processing::parameter::Parameter::Ptr getHostParameter ( size_t i ) { 
		if ( i > hostParameter.size() ) return parameter::Parameter::Ptr();
		return hostParameter[i]; 
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Fuellt outContainer mit Kind-ProcessorNode Objekten, eines ProcessorNode Objektes.
	 * @param node
	 * @param outContainer
	 */
	template < typename Container >
	void getChildNodes ( ProcessorNode::Ptr node, Container &outContainer );
	//--------------------------------------------------------------------------------------------------------
	/**
	* Fuellt outContainer mit Eltern-ProcessorNode Objekten, eines ProcessorNode Objektes.
	 * @param node
	 * @param outContainer
	 */
	template < typename Container >
	void getParentNodes ( ProcessorNode::Ptr node, Container &outContainer );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Anzahl der enthaltenden Host-Parameter Objekte. (Vst-Parameter des Clients)
	 */
	size_t getNumHostParameter () { return hostParameter.size(); }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param filter
	 * @return VstTimeInfo Objekt des Hosts. (siehe VST-SDK)
	 */
	VstTimeInfo * getVstTimeInfo ( VstInt32 filter) { return hostInfo->getVstTimeInfo(filter); }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Eintritts-ProcessorNode Objekt.
	 */
	StartNode::Ptr getStartNode();
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Austritts-ProcessorNode Objekt.
	 */
	EndNode::Ptr getEndNode();
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Verarbeitet VstEvents (zb. MIDI events) (siehe VST-SDK)
	 * @param events
	 */
	virtual void processEvents(VstEvents * events);
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Wurde zuvor mittels pushAndCopy() ein Frames(=Eingabe-Samplemenge) Objekt uebergeben,
	 * kann ueber processGraph() eine verarbeitung erfolgen. Wirft andernfalls com::ppiError::StackUnderflow.
	 * @param outputs Zieldatenstruktur fuer Samplebloecke gemaess VST-SDK (siehe processReplacing)
	 * @param numSamples anzahl der zu verarbeitenden Samples
	 */
	void processGraph( float **outputs, Processor::Int numSamples );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Uebergibt Frames Objekt(=Eingabe-Samplemenge) an Graph.
	 * @param fr Frames-Objekt
	 * @param numSamples anzhal der zu Berechnenden Samples
	 */
	void pushAndCopy( Frames *fr, Processor::Int numSamples );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param obj
	 * @return true, falls obj in Graph enthalten
	 */
	bool contains(PObject::Ptr obj) const;
	//--------------------------------------------------------------------------------------------------------
	virtual ~Graph();
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Anzahl aller BGL-Vertices
 	 */
	size_t getNumNodes() { return boost::num_vertices(g); }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Persitiert Graph
	 * @param ar
	 */
	void save ( oArchive &ar );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Rekonstruiert Graph-Objekt aus Boost::Archive Strom
	 * @param ar Boost::Archive Objekt
	 * @param hostInfo Hostinfo Objekt
	 * @return
	 */
	static Graph::Ptr load ( iArchive &ar, IHostInfo *hostInfo );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return gesamt Latenz des Graph in Ms.
	 */
	size_t getGraphDelay();
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Janitor-Objekt
	 */
	boost::shared_ptr<Janitor> getJanitor();
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
/**
 * @class Graph::Janitor
 * Ermoeglicht hinzufuegen und entfernen von PObjects und Verbindungen.
 * Solange Janitor existent, ist Graph::processGraph() ueber Mutex gesperrt.
 */
class Graph::Janitor {
//============================================================================================================
friend class Graph;
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<Janitor> Ptr;
	//--------------------------------------------------------------------------------------------------------
	enum State { SUCCEED, FAILED };
private:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Eltern Graphobjekt
	 */
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
	/**
	 * aktualisiert ProcessorNode->bglVertex zurodnung, ist notwendig nachdem ein Knoten-Objekt aus dem
	 * BGL-Graph entfernt wurde.
	 */
	void updateProcessorNodeVertexRelations();
public:
	//--------------------------------------------------------------------------------------------------------
	virtual ~Janitor ();
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Fuegt NOPNode-Objekt hinzu
	 * @param obj
	 * @return SUCCEED / FAILED
	 */
	State add ( NOPNode::Ptr obj ) { 
		if ( !graph->add(obj) ) return FAILED;
		return addProcessorNode(obj); 
	} 
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Fuegt ProcessAdapter-Objekt hinzu
	 * @param obj
	 * @return SUCCEED / FAILED
	 */
	State add ( ProcessAdapter::Ptr obj );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Fuegt InputNode-Objekt hinzu
	 * @param obj
	 * @return SUCCEED / FAILED
	 */
	State add ( ProcessAdapter::InputNode::Ptr obj );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Fuegt OutputNode-Objekt hinzu
	 * @param obj
	 * @return SUCCEED / FAILED
	 */
	State add ( ProcessAdapter::OutputNode::Ptr obj );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Fuegt Parameter-Objekt hinzu
	 * @param obj
	 * @return SUCCEED / FAILED
	 */
	State add ( parameter::Parameter::Ptr obj );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Entfernt ProcessAdapter-Objekt
	 * @param obj
	 * @return SUCCEED / FAILED
	 */
	State remove ( ProcessAdapter::Ptr obj );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Entfernt InputNode-Objekt
	 * @param obj
	 * @return SUCCEED / FAILED
	 */
	State remove ( ProcessAdapter::InputNode::Ptr obj ) {
		if ( !graph->remove(obj) ) return FAILED;
		return removeProcessorNode( obj );
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Entfernt OutputNode-Objekt
	 * @param obj
	 * @return SUCCEED / FAILED
	 */
	State remove ( ProcessAdapter::OutputNode::Ptr obj ) {
		if ( !graph->remove(obj) ) return FAILED;
		return removeProcessorNode( obj );
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Entfernt Parameter-Objekt
	 * @param obj
	 * @return SUCCEED / FAILED
	 */
	State remove ( parameter::Parameter::Ptr obj );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Entfernt NOPNode-Objekt
	 * @param obj
	 * @return SUCCEED / FAILED
	 */
	State remove ( NOPNode::Ptr obj ) { 
		if ( !graph->remove(obj) ) return FAILED;
		return removeProcessorNode(obj); 
	} 
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Muss aufgerufen werden, nachdem im HostInfo Objekt(Client), die Samplrate oder die Sampleblockgroesse
	 * geaendert wurde. Wirft ppiError::InvalidBlockSize bzw. ppiError::InvalidSampleRate.
	 */
	void hostInfoChanged();
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @deprecated
	 * TODO: irgendwann kann die * variante mal der ::Ptr variante weichen
	 * @param parent
	 * @param child
	 * @return SUCCEED / FAILED
	 */
	State connectNodes( ProcessorNode *parent, ProcessorNode *child) {
		ProcessorNode::Ptr p = boost::shared_dynamic_cast< ProcessorNode, PObject > ( parent->getPtr() );
		ProcessorNode::Ptr c = boost::shared_dynamic_cast< ProcessorNode, PObject > ( child->getPtr() );
		if ( !p || !c ) return FAILED;
		return connectNodes( p, c );
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Erzeugt die gerichtete verbindung : output->input.
	 * Prueft ob graph valid (keine zykel).
	 * Wenn nicht wird verbindung wieder aufgehoben und FAILED zurueckgegeben.
	 * Ansonsten: SUCCEED.
	 * @param parent
	 * @param child
	 * @return SUCCEED / FAILED
	 */
	State connectNodes( ProcessorNode::Ptr parent, ProcessorNode::Ptr child );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * TODO: irgendwann kann die * variante mal der ::Ptr variante weichen
	 * @param parent
	 * @param child
	 * @return SUCCEED / FAILED
	 */
	State removeConnection( ProcessorNode *parent, ProcessorNode *child) {
		ProcessorNode::Ptr p = boost::shared_dynamic_cast< ProcessorNode, PObject > ( parent->getPtr() );
		ProcessorNode::Ptr c = boost::shared_dynamic_cast< ProcessorNode, PObject > ( child->getPtr() );
		if ( !p || !c ) return FAILED;
		return removeConnection( p, c );
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * 	Erzeugt die gerichtete verbindung : output->input.
	 * 	Prueft ob graph valid (keine zykel).
	 * 	Wenn nicht wird verbindung wieder aufgehoben und FAILED zurueckgegeben.
	 * 	Ansonsten: SUCCEED.
	 * @param parent
	 * @param child
	 * @return SUCCEED / FAILED
	 */
	State removeConnection( ProcessorNode::Ptr parent, ProcessorNode::Ptr child );
}; // Janitor

//============================================================================================================
/**
 * @class: DFSVisitor.
 * Implementierung eine BGL::DFSVisitor-Klasse für BGL::depth_first_search Algorithmus.
 * (http://www.boost.org/doc/libs/1_48_0/libs/graph/doc/DFSVisitor.html)
 * Erstellt SignalProcessPath.
 */
class DFSVisitor : public boost::dfs_visitor<> {
//============================================================================================================
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
