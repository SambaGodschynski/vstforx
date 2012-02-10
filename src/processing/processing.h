/*
 ppi.processing
 Signal Processing:
 P ist ein gerichteter Graph. Jeder Plugin-Knoten hatt Vorgaenger und Nachfolger.
 Die bearbeitung des graphen erfolgt uber einen SignalProcessPath. Dies ist eine
 Liste in der die ProcessorNodes nach abhaengigkeit geordnet sind.
 Der SignalProcessPath wird nur nach jeder Graph aenderung berechnet.
 */
#ifndef PROCESSING_H_
#define PROCESSING_H_

#include "com/one4All.h"
#include <string>
#include <vector>
#include <map>
#include <list>
#include <stack>
#include "com/Serialization.h"
#include "IHostInfo.h"
#include "PObject.h"
#include "IVstEventProcessor.h"
#include "BglGraph.h"
#include "Frames.h"

//============================================================================================================
//	Vorwaerts Deklarationen
//============================================================================================================
namespace processing {
	//--------------------------------------------------------------------------------------------------------
	// Die Klasse Graph
	class Graph;
	//--------------------------------------------------------------------------------------------------------
	//Implementiert die processFrames Methode indem es die processFrames Methode
	//des im Konstruktor uebergebenen Processor Objektes aufruft.
	class ProcessAdapterNode;
} // namespace processing

namespace processing {
//============================================================================================================
// Schnittstelle: Processor.
// Alle Unterklassen muessen processFrames implementieren.
// Diese Methode bearbeitet Die Audio Daten die im Frames gelagert sind.
//============================================================================================================
class Processor {
public:
	//--------------------------------------------------------------------------------------------------------
	typedef unsigned int Int; 
	//--------------------------------------------------------------------------------------------------------
	virtual void processFrames(Frames * frames, Int numSamples ) = 0;

}; //class Processor

//============================================================================================================
//Klasse: ProcessorNode.
//Oberklasse fuer alle Knoten die processFrames() implementieren.
//Kann n Node Vorgaenger und m Node Nachfolger aufnehmen.
//============================================================================================================
class ProcessorNode : public Processor, public PObject {
friend class Graph;
friend class DFSVisitor;
friend class boost::serialization::access;
friend class StartNode;
friend class EndNode;
BOOST_SERIALIZATION_SPLIT_MEMBER()
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<ProcessorNode> Ptr;
	//--------------------------------------------------------------------------------------------------------
	typedef vector<ProcessorNode*> Container;
	//--------------------------------------------------------------------------------------------------------
	typedef list<ProcessorNode*> Parents;   // wird in DFSVisitor bzw. �ber updateGraph ermittelt
private:
	//--------------------------------------------------------------------------------------------------------
	Parents parents;
	//--------------------------------------------------------------------------------------------------------
	// erzeugt oder entfernt tmpFrames
	void prepareFrameContainer( size_t num ); 
	//--------------------------------------------------------------------------------------------------------
	bgl::Vertex bglVertex; // !! kann sich aendern
	//--------------------------------------------------------------------------------------------------------
	DCStream stream;
	//--------------------------------------------------------------------------------------------------------
	template < typename Archive >
	void save ( Archive &ar, const unsigned int version ) const {
		ar << boost::serialization::base_object<PObject>(*this);
		// ar << parents;  wird in DFSVisitor bzw. �ber updateGraph ermittelt
		// ar << activeChildren; wird in DFSVisitor bzw. �ber updateGraph ermittelt
		size_t numChildFrames = frameContainer.size();
		ar << numChildFrames;
	}
	//--------------------------------------------------------------------------------------------------------
	template < typename Archive >
	void load ( Archive &ar, const unsigned int version ) {
		ar >> boost::serialization::base_object<PObject>(*this);
		// ar >> parents;
		// ar >> activeChildren; wird in DFSVisitor bzw. �ber updateGraph ermittelt
		size_t numChildFrames;
		ar >> numChildFrames;
		prepareFrameContainer( numChildFrames );
	}
	//--------------------------------------------------------------------------------------------------------
	bool active; // ist true wenn Node die Graph-ProzessKette beiinflusst.
	//--------------------------------------------------------------------------------------------------------
	bool onCall; // wird benoet. um feedback schleifen zu ermitteln. siehe findCycles
	//--------------------------------------------------------------------------------------------------------
	size_t activeChildren; //anzahl der activen Knoten Nachfolger wird in DFSVisitor bzw. ermittelt
	//--------------------------------------------------------------------------------------------------------
	Frames *tmpFrames; // benutzt in processNode
	//--------------------------------------------------------------------------------------------------------
	size_t delay;
	//--------------------------------------------------------------------------------------------------------
	// liefert signal verabeitungs verzoegerung im graph ( unter beachtung der vorgaenger )
	size_t getNodeDelay() const { return delay; }
protected:
	//--------------------------------------------------------------------------------------------------------
	//Jeder Knoten hatt ein ergebniss Stack. Die Adresse eines errechneten Frames wird hier gelagert.
	//Hatt ein Knoten mehr als ein Nachfolger so muss das errechnete Frames kopiert und auf den Stack gelegt werden.
	//Wird ein Knoten aufgefordert die Adresse eines Frames zu liefern so wird diese vom Stack geholt.
	typedef stack<Frames*> FrameStack;
	FrameStack frameStack; //< hier lagern frames zum austausch
	//--------------------------------------------------------------------------------------------------------
	// Hier lagern alle Frames die vom ProcesserNode 
	// erzeugt werden. Dies geschieht wenn eine neue
	// abgehende Verbindung (child) hinzugefuegt wird
	// und mehr als eine verbindung existiert.
	typedef list<Frames::Ptr> FrameContainer;
	FrameContainer frameContainer;	//< ProcessorNode besitzt diese frames
	//--------------------------------------------------------------------------------------------------------
	Frames * mixInputsToFrames( Processor::Int numSamples ) {
		if ( parents.empty() ) {
			tmpFrames->setZero( numSamples );
			return tmpFrames;
		}
		ProcessorNode *n;
		if ( parents.size() == 1 ) { // sonderzug nach pankow:
			n = parents.front();
			assert ( n->isActive() );
			return n->popFrame();
		}
		Parents::iterator it = parents.begin();
		for ( ; it!=parents.end(); ++it ) {
			n = *it;
			if ( !n->isActive() ) continue;
			stream.addFrame ( n->popFrame(), numSamples, getNodeDelay() - n->getNodeDelay() );
		}
		stream.flush ( numSamples, tmpFrames->getData() );
		return tmpFrames;
	}
	//--------------------------------------------------------------------------------------------------------
	virtual void processNode( Processor::Int numSamples );
	//--------------------------------------------------------------------------------------------------------
	ProcessorNode ( const string &name="unnamed" );
public:
	//--------------------------------------------------------------------------------------------------------
	const bgl::Vertex & getBglVertex() const { return bglVertex; }
	//--------------------------------------------------------------------------------------------------------
	virtual bool isStartNode() const { return false; }
	//--------------------------------------------------------------------------------------------------------
	virtual bool isEndNode() const { return false; }
	//--------------------------------------------------------------------------------------------------------
	DCStream & getDCStream() { return stream; }
	//--------------------------------------------------------------------------------------------------------
	size_t getNumPreparedChildren() { return frameContainer.size(); }
	//--------------------------------------------------------------------------------------------------------
	virtual void processFrames(Frames * frames, Processor::Int numSamples) = 0;
	//--------------------------------------------------------------------------------------------------------
	int getFrameStackSize() const { return frameStack.size(); }
	//--------------------------------------------------------------------------------------------------------
	// liefert signal verabeitungs verzoegerung des process ( VSTPlugin::processReplacing() )
	virtual size_t getProcessDelay() const { return 0; }
	//--------------------------------------------------------------------------------------------------------
	size_t getNumActiveChildren(){ return activeChildren; }
	//--------------------------------------------------------------------------------------------------------
	bool isActive(){ return active; }
	//--------------------------------------------------------------------------------------------------------
	void setActive( bool stat ){ active = stat; }
	//--------------------------------------------------------------------------------------------------------
	virtual ~ProcessorNode();
	//--------------------------------------------------------------------------------------------------------
	Frames * popFrame() {
		if ( getFrameStackSize() <= 0 ) 
			throw com::ppiError::StackUnderflow(
			   "StackUnderflow", __FILE__, __LINE__ );
		Frames *fr = frameStack.top();
		frameStack.pop();
		return fr;
	}
	
	//--------------------------------------------------------------------------------------------------------
	// lagert frames in den Ergebniss Stack.
	// hatt ein Knoten mehr als 1 Kind-Knoten werden Frames kopiert
	// und in den Stack gelagert.
	inline void pushAndCopy( Frames * frames, Processor::Int numSamples ) {
		if ( isEndNode() ){
			frameStack.push (frames);
			return;
		}
		if ( getNumActiveChildren() == 0 ) return;
		frameStack.push (frames);
		FrameContainer::iterator it = frameContainer.begin();
		for ( int i=0; i<getNumActiveChildren() - 1; i++ ) { // kopiere weitere frames in stack
			(*it)->copyIntoFrom ( *frames, numSamples );
			frameStack.push ( (*it++).get() );
		}
	}
};//class ProcessorNode

//============================================================================================================
//	Klasse: VariableOutputAdapter
//============================================================================================================
class VariableOutputAdapter {
public:
	//--------------------------------------------------------------------------------------------------------
	virtual ProcessorNode::Ptr addOutputNode() = 0;
};

//============================================================================================================
//	Klasse: VariableInputAdapter
//============================================================================================================
class VariableInputAdapter {
public:
	//--------------------------------------------------------------------------------------------------------
	virtual ProcessorNode::Ptr addInputNode() = 0;
};

//============================================================================================================
//	Klasse: NOPNode. - No Operation Node -
//  Ein ProcessorNode der nichts tut ausser zu existieren. ( fuer Start-, EndKnoten )
//============================================================================================================
class NOPNode : public ProcessorNode, public Serializable {
friend class boost::serialization::access;
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<NOPNode> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	template < typename Archive >
	void serialize ( Archive &ar, const unsigned int version ){
		ar & boost::serialization::base_object< ProcessorNode > ( *this );
	}
	//--------------------------------------------------------------------------------------------------------
	NOPNode() {}
protected:
	//--------------------------------------------------------------------------------------------------------
	NOPNode( const string& );
public:
	//--------------------------------------------------------------------------------------------------------
	static Ptr create ( const string &str ) {
		Ptr neu( new NOPNode(str) );
		neu->self = neu;
		return neu;
	}
	//--------------------------------------------------------------------------------------------------------
	void processFrames ( Frames *fr, Processor::Int numSamples );
	//--------------------------------------------------------------------------------------------------------
	~NOPNode(){}
};

//============================================================================================================
//	Klasse: StartNode
//============================================================================================================
class StartNode : public NOPNode {
friend class boost::serialization::access;
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<StartNode> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	template < typename Archive >
	void serialize ( Archive &ar, const unsigned int version ){
		ar & boost::serialization::base_object< NOPNode > ( *this );
	}
	//--------------------------------------------------------------------------------------------------------
	StartNode() : NOPNode ("startNode") {}
public: 
	//--------------------------------------------------------------------------------------------------------
	static Ptr create () {
		Ptr neu( new StartNode() );
		neu->self = neu;
		return neu;
	}
	//--------------------------------------------------------------------------------------------------------
	virtual bool isStartNode() const { return true; }
};
//============================================================================================================
//	Klasse: EndNode
//  Fuellt Stream Objekt so dass latenzen kompensiert werden.
//============================================================================================================
class EndNode : public NOPNode {
friend class boost::serialization::access;
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<EndNode> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	template < typename Archive >
	void serialize ( Archive &ar, const unsigned int version ){
		ar & boost::serialization::base_object< NOPNode > ( *this );
	}
	//--------------------------------------------------------------------------------------------------------
	virtual void processNode( Processor::Int numSamples);
	//--------------------------------------------------------------------------------------------------------
	EndNode() : NOPNode ("endNode") {}
public:
	//--------------------------------------------------------------------------------------------------------
	static Ptr create () {
		Ptr neu( new EndNode() );
		neu->self = neu;
		return neu;
	}
	//--------------------------------------------------------------------------------------------------------
	virtual bool isEndNode() const { return true; }
};

//============================================================================================================
//Klasse: ProcessAdapter.
//    Input_Node0-O   O -  Input_Node1 ... Input_NodeN
//                 \ /
//                  O - AdapterNode
//                 / \
//   Output_Node0-O   O - Output_Node1 ... Output_NodeN
//============================================================================================================
class ProcessAdapter: public PObject {
friend class ProcessAdapterNode;
friend class boost::serialization::access;
BOOST_SERIALIZATION_SPLIT_MEMBER()
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<ProcessAdapter> Ptr;
	//--------------------------------------------------------------------------------------------------------
	class OutputNode : public NOPNode {
	friend class boost::serialization::access;
	public:
		typedef boost::shared_ptr<OutputNode> Ptr;
	private:
		template < typename Archive >
		void serialize ( Archive &ar, const unsigned int version ){
			ar & boost::serialization::base_object< NOPNode > ( *this );
			ar & parent;
		}
		OutputNode() : NOPNode(""){}
		ProcessAdapter *parent;
		OutputNode( const string &name, ProcessAdapter* parent );
	public:
		static Ptr create( const string &name, ProcessAdapter *parent ) {
			Ptr neu( new OutputNode(name, parent) );
			neu->self = neu;
			return neu;
		}
		// Outputnodes haben keiene processNode() meth. da sie die 
		// frames ueber den adapter erhalten.
		virtual void processNode( Processor::Int numSamples ) {};
		virtual ~OutputNode(){}
		ProcessAdapter::Ptr getProcessAdapter() { 
			ProcessAdapter::Ptr p = 
				boost::shared_dynamic_cast< ProcessAdapter, PObject >( parent->getPtr() );
			return p;
		}
	};
	//--------------------------------------------------------------------------------------------------------
	class InputNode : public NOPNode {
	friend class boost::serialization::access;
	public:
		typedef boost::shared_ptr<InputNode> Ptr;
	private:
		template < typename Archive >
		void serialize ( Archive &ar, const unsigned int version ){
			ar & boost::serialization::base_object< NOPNode > ( *this );
			ar & parent;
		}
		InputNode() : NOPNode("") {}
		ProcessAdapter *parent;
		InputNode( const string &name, ProcessAdapter *parent );
	public:
		static Ptr create( const string &name, ProcessAdapter *parent ) {
			Ptr neu( new InputNode(name, parent) );
			neu->self = neu;
			return neu;
		}
		virtual ~InputNode(){}
		ProcessAdapter::Ptr getProcessAdapter() { 
			ProcessAdapter::Ptr p = 
				boost::shared_dynamic_cast< ProcessAdapter, PObject >( parent->getPtr() );
			return p;
		}
	};
	//--------------------------------------------------------------------------------------------------------
	typedef vector<OutputNode::Ptr> OutputNodes;
	//--------------------------------------------------------------------------------------------------------
	typedef vector<InputNode::Ptr> InputNodes;
	//--------------------------------------------------------------------------------------------------------
	typedef vector<Frames*> InputFrames;
private:
	//--------------------------------------------------------------------------------------------------------
	template < typename Archive >
	void save ( Archive &ar, const unsigned int version ) const {
		com::MethodMessage<ProcessAdapter> methodMessage ( "save()", *this );
		ar << boost::serialization::base_object< PObject > ( *this );
		ar << hostInfo;
		ar << inputNodes;
		ar << outputNodes;
		ar << aNode;
	}
	//--------------------------------------------------------------------------------------------------------
	template < typename Archive >
	void load ( Archive &ar, const unsigned int version ) {
		com::MethodMessage<ProcessAdapter> methodMessage ( "load()", *this );
		ar >> boost::serialization::base_object< PObject > ( *this );
		ar >> hostInfo;
		ar >> inputNodes;
		ar >> outputNodes;
		ar >> aNode;
	}
protected:
	//--------------------------------------------------------------------------------------------------------
	ProcessAdapter (){}
	//--------------------------------------------------------------------------------------------------------
	OutputNodes outputNodes;
	//--------------------------------------------------------------------------------------------------------
	InputNodes inputNodes;
	//--------------------------------------------------------------------------------------------------------
	boost::shared_ptr<ProcessAdapterNode> aNode;
	//--------------------------------------------------------------------------------------------------------
	IHostInfo * hostInfo;
	//--------------------------------------------------------------------------------------------------------
	// liefert signal verabeitungs verzoegerung des process ( VSTPlugin::processReplacing() )
	virtual size_t getProcessDelay() const { return 0; }
	//--------------------------------------------------------------------------------------------------------
	size_t getNumActiveOutputNodes() const;
	//--------------------------------------------------------------------------------------------------------
	// fuegt neuen ProcessAdapterNode hinzu und liefert Zeiger.
	OutputNode::Ptr createOutputNode( const string &name = "unnamed" );
	//--------------------------------------------------------------------------------------------------------
	// fuegt neuen ProcessAdapterNode hinzu und liefert Zeiger.
	InputNode::Ptr createInputNode( const string &name = "unnamed" );
	//--------------------------------------------------------------------------------------------------------
	void processAdapter( Processor::Int numSamples ) {
		_processAdapter ( numSamples );
	}	
public:
	//--------------------------------------------------------------------------------------------------------
	virtual void _processAdapter ( Processor::Int numSamples ) = 0;
	//--------------------------------------------------------------------------------------------------------
	IHostInfo * getHostInfo() const { return hostInfo; } 
	//--------------------------------------------------------------------------------------------------------
	void setHostInfo( IHostInfo * hI ){ hostInfo = hI; } 
	//--------------------------------------------------------------------------------------------------------
	ProcessAdapter( IHostInfo * hostInfo, size_t numInputNodes = 1, size_t numOutputNodes = 1 );
	//--------------------------------------------------------------------------------------------------------
	size_t getNumOutputNodes () const { return outputNodes.size(); }
	//--------------------------------------------------------------------------------------------------------
	size_t getNumInputNodes () const { return inputNodes.size(); }
	//--------------------------------------------------------------------------------------------------------
	// Liefert ProcessAdapterNode zu index.
	OutputNode::Ptr getOutputNode( size_t index ){ 
		if ( index >= getNumOutputNodes() ) 
			throw ppiError::IndexOutOfBoundException ( "OutOfBound-OutputNodes", __FILE__, __LINE__ );
		return outputNodes[index]; 
	}
	//--------------------------------------------------------------------------------------------------------
	boost::shared_ptr<ProcessAdapterNode> getAdapterNode() { return aNode; }
	//--------------------------------------------------------------------------------------------------------
	// liefert Input-Node
	virtual InputNode::Ptr getInputNode( size_t index ) { 
		if ( index >= getNumInputNodes() ) 
			throw ppiError::IndexOutOfBoundException ( "OutOfBound-InputNodes", __FILE__, __LINE__ );
		return inputNodes[index]; 
	}
	//--------------------------------------------------------------------------------------------------------
	virtual ~ProcessAdapter();
}; //class ProcessAdapter
//============================================================================================================
//Klasse: ProcessAdapterNode.
//Implementiert die processFrames Methode indem es die processFrames Methode
//des im Konstruktor uebergebenen ProcessAdapter Objektes aufruft.
//============================================================================================================
class ProcessAdapterNode : public ProcessorNode, public Serializable {
friend class boost::serialization::access;
friend class ProcessAdapter;
friend class ProcessAdapter::InputNode;
friend class ProcessAdapter::OutputNode;
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<ProcessAdapterNode> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	template < typename Archive >
	void serialize ( Archive &ar, const unsigned int version ){
		ar & boost::serialization::base_object< ProcessorNode > ( *this );
		ar & adapter;
	}
	//--------------------------------------------------------------------------------------------------------
	ProcessAdapterNode() {}
	//--------------------------------------------------------------------------------------------------------
	ProcessAdapter *adapter;
	//--------------------------------------------------------------------------------------------------------
	ProcessAdapterNode( ProcessAdapter *processAdapter );
public:
	//--------------------------------------------------------------------------------------------------------
	static Ptr create ( ProcessAdapter *processAdapter ) {
		Ptr neu( new ProcessAdapterNode( processAdapter ) );
		neu->self = neu;
		return neu;
	}
	//--------------------------------------------------------------------------------------------------------
	virtual ~ProcessAdapterNode();
	//--------------------------------------------------------------------------------------------------------
	virtual size_t getProcessDelay() const { return adapter->getProcessDelay(); }
	//--------------------------------------------------------------------------------------------------------
	ProcessAdapter * getAdatper(){ return adapter; }
	//--------------------------------------------------------------------------------------------------------
	virtual void processFrames ( Frames*, Processor::Int numSamples ) {}
	//--------------------------------------------------------------------------------------------------------
	virtual void processNode( Processor::Int numSamples ) { adapter->processAdapter( numSamples ); }
}; //class ProcessAdapterNode
} // namespace Processing

#endif