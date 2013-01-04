/*
 * ===========================================================================================================
 * processing.h
 *      Author: Johannes Unger
 * ===========================================================================================================
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
#include "IMidiEventProcessor.h"
#include "BglGraph.h"
#include "Frames.h"
#include "com/Events.h"
#include <sambag/com/events/PropertyChanged.hpp>

//============================================================================================================
//	Vorwaerts Deklarationen
//============================================================================================================
namespace processing {
namespace fp = frx::processing;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Die Klasse Graph
	 */
	class Graph;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Implementiert die processFrames Methode indem es die processFrames Methode
	 * des im Konstruktor uebergebenen Processor Objektes aufruft.
	 */
	class ProcessAdapterNode;
} // namespace processing

namespace processing {
//============================================================================================================
/**
 * @interface Processor.
 * Alle Unterklassen muessen processFrames() implementieren.
 */
class Processor {
//============================================================================================================
public:
	//--------------------------------------------------------------------------------------------------------
	typedef unsigned int Int; 
	//--------------------------------------------------------------------------------------------------------
	/**
	 * bearbeitet Frames-Objekt (Eingangs-Sampleblock)
	 * @param frames
	 * @param numSamples Anzahl der zu bearbeitenden Samples
	 */
	virtual void processFrames(Frames * frames, Int numSamples ) = 0;

}; //class Processor

//============================================================================================================
/**
 * @class ProcessorNode.
 * Oberklasse fuer alle Knoten die processFrames() implementieren.
 */
//============================================================================================================
class ProcessorNode : public Processor,
	public PObject
{
friend class Graph;
friend class DFSVisitor;
friend class boost::serialization::access;
friend class StartNode;
friend class EndNode;
BOOST_SERIALIZATION_SPLIT_MEMBER() // teilt boost::serialize in save() und load()
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<ProcessorNode> Ptr;
	//--------------------------------------------------------------------------------------------------------
	typedef vector<ProcessorNode*> Container;
	//--------------------------------------------------------------------------------------------------------
	typedef list<ProcessorNode*> Parents;
private:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * ist true wenn Node die Graph-ProzessKette beeinflusst.
	 */
	bool active;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Anzahl der aktiven Knoten Nachfolger wird in DFSVisitor ermittelt
	 */
	size_t activeChildren;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Signal-Verabeitungs-Verzoegerung im graph
	 */
	size_t delay;
protected:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * ist ergebnis Frames-Objekt von mixInputToFrames()
	 */
	Frames *tmpFrames;
	//--------------------------------------------------------------------------------------------------------
	virtual void setNodeDelay(size_t v) { delay = v; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Eltern ProcessorNode-Objekte. Werden in DFSVisitor ueber updateGraph() ermittelt
	 */
	Parents parents;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Fuegt hinzu/entfernt Frames-Objekt zum FramesContainer.
	 * Wird von Graph::updateProcessorNode() aufgerufen, nachdem eine neue Verbindung zum Knoten hinzugefuegt
	 * wurde.
	 * @param num Anzahl der Kind-ProcessorNode-Objekte
	 */
	void prepareFramesContainer( size_t numChildren );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * BGL::Vertex Zuordnung. Wird zur schnelleren ermittlung intrinistisch gespeichert
	 */
	bgl::Vertex bglVertex; // !! kann sich aendern
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Delay-Kompensations-Stream
	 */
	DCStream stream;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Serialisiert ProcessorNode-Objekt
	 * @param ar
	 * @param version
	 */
	template < typename Archive >
	void save ( Archive &ar, const unsigned int version ) const {
		ar << boost::serialization::base_object<PObject>(*this);
		// ar << parents;  wird in DFSVisitor bzw. ueber updateGraph ermittelt
		// ar << activeChildren; wird in DFSVisitor bzw. ueber updateGraph ermittelt
		size_t numChildFrames = frameContainer.size();
		ar << numChildFrames;
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	* Deserialisiert ProcessorNode-Objekt
	 * @param ar
	 * @param version
	 */
	template < typename Archive >
	void load ( Archive &ar, const unsigned int version ) {
		ar >> boost::serialization::base_object<PObject>(*this);
		// ar >> parents;
		// ar >> activeChildren; wird in DFSVisitor bzw. ueber updateGraph ermittelt
		size_t numChildFrames;
		ar >> numChildFrames;
		prepareFramesContainer( numChildFrames );
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * bestimmt ob ProcessorNode-Objekt aktiv/nicht aktiv. Wird von DFSVisitor aufgerufen.
	 * @param stat
	 */
	void setActive( bool stat ){ active = stat; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Ausgabe-Sampleblockmenge-Ergebniss-Stack.
	 */
	typedef stack<Frames*> FrameStack;
	FrameStack frameStack;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Kontainer fuer Ausgabe-Sampleblockmenge. Enthaelt die Frames-Objekte die waehrend der
	 * pushAndCopy()-Operation, in den FrameStack kopiert werden.
	 * (da FramesStack nur 'pure'-Zeiger enthaelt wird extra Kontainer benoetigt)
	 * Anzahl abhaengig von activeChildren. Wird ueber prepareFramesContainer initalisiert.
	 * TODO: list->vector
	 */
	typedef list<Frames::Ptr> FrameContainer;
	FrameContainer frameContainer;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Mischt alle Eingangs-Frames-Objekte zu einem Frames-Objekt.
	 * @param numSamples Anzahl der zu berarbeitenden Samples
	 * @return Frames-Objekt
	 */
	Frames * mixInputsToFrames( Processor::Int numSamples );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Bereitet Knoten vor um schliesslich processFrames() aufzurufen.
	 * @param numSamples Anzahl der zu berarbeitenden Samples
	 */
	virtual void processNode( Processor::Int numSamples ) {
		processFrames ( mixInputsToFrames(numSamples), numSamples );
	}
	//--------------------------------------------------------------------------------------------------------
	ProcessorNode ( const string &name="unnamed" );
	//--------------------------------------------------------------------------------------------------------
public:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Signal-Verabeitungs-Verzoegerung im Graph ( unter beachtung der Vorgaenger )
	 */
	size_t getNodeDelay() const { return delay; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return zum ProcessorNode-Objekt zugeordneter BGL-Vertex.
	 */
	const bgl::Vertex & getBglVertex() const { return bglVertex; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return true, wenn Eintritts-Knoten
	 */
	virtual bool isStartNode() const { return false; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return true, wenn Austritts-Knoten
	 */
	virtual bool isEndNode() const { return false; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Delay-Kompensations-Stream Objekt
	 */
	DCStream & getDCStream() { return stream; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return FramesContainer-Objekt
	 */
	const FrameContainer & getFramesContainer() const { return frameContainer; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return groesse des Frames-Stack
	 */
	int getFrameStackSize() const { return frameStack.size(); }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Signal-Verabeitungs-Verzoegerung des Knoten (ohne Beruecksichtigung der Vorgaenger)
	 */
	virtual size_t getProcessDelay() const { return 0; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Anzahl der Kinds-Knoten die aktiv sind (@see isActive())
	 */
	size_t getNumActiveChildren(){ return activeChildren; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return true, wenn aktiv. (wenn Node die Graph-ProzessKette beeinflusst)
	 */
	bool isActive(){ return active; }
	//--------------------------------------------------------------------------------------------------------
	virtual ~ProcessorNode();
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Frames-Objekt.
	 */
	Frames * popFrame() {
		if ( getFrameStackSize() <= 0 ) 
			throw com::ppiError::StackUnderflow(
			   "StackUnderflow", __FILE__, __LINE__ );
		Frames *fr = frameStack.top();
		frameStack.pop();
		return fr;
	}
	
	//--------------------------------------------------------------------------------------------------------
	/**
	 * fuegt Frames-Objekt den Ergebniss-Stack hinzu.
	 * hatt ein ProcessorNode-Objekt mehr als 1 Kind-Objekt wird Frames-Objekt kopiert
	 * und zusaetzlich den Stack hinzugefuegt.
	 * @param frames Frames-Objekt
	 * @param numSamples Anzahl der zu bearbeitenden Samples
	 */
	inline void pushAndCopy( Frames * frames, Processor::Int numSamples ) {
		if ( isEndNode() ){
			frameStack.push (frames);
			return;
		}
		if ( getNumActiveChildren() == 0 ) return;
		frameStack.push (frames);
		FrameContainer::iterator it = frameContainer.begin();
		for ( size_t i=0; i<getNumActiveChildren() - 1; i++ ) { // kopiere weitere frames in stack
			(*it)->copyIntoFrom ( *frames, numSamples );
			frameStack.push ( (*it++).get() );
		}
	}
};//class ProcessorNode

//============================================================================================================
/**
 * @interface: VariableOutputAdapter
 * Schnitstelle fuer alle ProcessAdapter-Klassen, die Ausgangs-Knoten dynamisch hinzufuegen koennen.
 */
class VariableOutputAdapter {
//============================================================================================================
public:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Fuegt ProcessAdapter Ausgangs-Knoten hinzu
	 * @return ProcessorNode-Objekt
	 */
	virtual ProcessorNode::Ptr addOutputNode() = 0;
};

//============================================================================================================
/**
 * @interface: VariableInputAdapter
 * Schnitstelle fuer alle ProcessAdapter-Klassen, die Eingangs-Knoten dynamisch hinzufuegen koennen.
 */
class VariableInputAdapter {
//============================================================================================================
public:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Fuegt ProcessAdapter Eingangs-Knoten hinzu
	 * @return ProcessorNode-Objekt
	 */
	virtual ProcessorNode::Ptr addInputNode() = 0;
};

//============================================================================================================
/**
 * @class: NOPNode. - No Operation Node -
 * (Ausgabe = Eingabe)
 */
class NOPNode : public ProcessorNode, public Serializable {
//============================================================================================================
friend class boost::serialization::access;
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<NOPNode> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * (De)Serialisiert NOPNode-Objekt
	 * @param ar Boost::Archiv-Objekt
	 * @param version
	 */
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
	/**
	 * erzeugt NOPNode-Objekt
	 * @param str Objektname
	 * @return neues NOPNode-Objekt
	 */
	static Ptr create ( const string &str ) {
		Ptr neu( new NOPNode(str) );
		neu->self = neu;
		return neu;
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Ausgabe-Sampleblockmenge = Eingabe-Sampleblockmenge
	 * @param fr
	 * @param numSamples
	 */
	void processFrames ( Frames *fr, Processor::Int numSamples );
	//--------------------------------------------------------------------------------------------------------
	virtual ~NOPNode(){}
};

//============================================================================================================
/**
 * @class StartNode
 * Repraesentiert Eingangs-Knoten des Graphen.
 */
class StartNode : public NOPNode {
//============================================================================================================
friend class boost::serialization::access;
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<StartNode> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * (De)Serialisiert StartNode-Objekt
	 * @param ar Boost::Archiv-Objekt
	 * @param version
	 */
	template < typename Archive >
	void serialize ( Archive &ar, const unsigned int version ){
		ar & boost::serialization::base_object< NOPNode > ( *this );
	}
	//--------------------------------------------------------------------------------------------------------
	StartNode() : NOPNode ("startNode") {}
public: 
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Erzeugt StartNode-Objekt
	 * @return neues StartNode-Objekt
	 */
	static Ptr create () {
		Ptr neu( new StartNode() );
		neu->self = neu;
		return neu;
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return true
	 */
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
	/**
	 * (De)Serialisiert EndNode-Objekt
	 * @param ar Boost::Archiv-Objekt
	 * @param version
	 */
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
	/**
	 * Erzeugt EndNode-Objekt
	 * @return neues EndNode-Objekt
	 */
	static Ptr create () {
		Ptr neu( new EndNode() );
		neu->self = neu;
		return neu;
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return true
	 */
	virtual bool isEndNode() const { return true; }
};
//============================================================================================================
/**
 * @class IOChangedEvent.
 * Event: wird aufgerufen wenn Adapter Eingangs-/Ausgangskonfiguration geaendert wurde.
 */
struct IOChangedEvent : public com::events::Event {
//============================================================================================================
};
namespace sce = sambag::com::events;
//============================================================================================================
/**
 * @class ProcessAdapter
 * Repraesentiert ein Komplexes Modul mit N Ein/Ausgaengen.
 * Enthaelt N Input/Output Objekte und einen AdapterNode Objekte.
 * Sind diese einem Graph hinzugefuegt, wird - wenn ProcessFrames() des AdapterNode
 * ausgefuehrt - an processAdapter() weitergeleitet.
 */
class ProcessAdapter : 
	public PObject, 
	public com::events::EventSender<IOChangedEvent>,
	public sce::EventSender<sce::PropertyChanged> // TODO: replace IOChangedEvent with PropertyChanged 
{
//Klasse: ProcessAdapter.
//    Input_Node0-O   O -  Input_Node1 ... Input_NodeN
//                 \ /
//                  O - AdapterNode
//                 / \
//   Output_Node0-O   O - Output_Node1 ... Output_NodeN
//============================================================================================================
friend class ProcessAdapterNode;
friend class boost::serialization::access;
BOOST_SERIALIZATION_SPLIT_MEMBER()
public:
	//--------------------------------------------------------------------------------------------------------
	static const std::string PROPERTY_SWITCH_STATE;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * SwitchState(isInput, num)
	 */
	typedef std::pair<bool, size_t> SwitchState;
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<ProcessAdapter> Ptr;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Vorwaertz-Deklaration von eingebetteter OutputNode-Klasse
	 */
	class OutputNode;
	typedef boost::shared_ptr<OutputNode> OutputNodePtr;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Vorwaertz-Deklaration von eingebetteter InputNode-Klasse
	 */
	class InputNode;
	typedef boost::shared_ptr<InputNode> InputNodePtr;
	//--------------------------------------------------------------------------------------------------------
	typedef vector<OutputNodePtr> OutputNodes;
	//--------------------------------------------------------------------------------------------------------
	typedef vector<InputNodePtr> InputNodes;
	//--------------------------------------------------------------------------------------------------------
	typedef vector<Frames*> InputFrames;
private:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Serialisiert ProcessAdapter-Objekt
	 * @param ar boost::Archive-Objekt
	 * @param version
	 */
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
	/**
	 * Deserialisiert ProcessAdapter-Objekt
	 * @param ar boost::Archive-Objekt
	 * @param version
	 */
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
	/**
	 * ProcessAdapterNode-Objekt
	 */
	boost::shared_ptr<ProcessAdapterNode> aNode;
	//--------------------------------------------------------------------------------------------------------
	frx::processing::IHostInfo::WPtr  hostInfo;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Signal-Verabeitungs-Verzoegerung des ProcessAdapter.
	 */
	virtual size_t getProcessDelay() const { return 0; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Anzahl der Ausgangs-Knoten die aktiv sind (@see ProcessorNode::isActive())
	 * @return
	 */
	size_t getNumActiveOutputNodes() const;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * fuegt neuen OutputNode hinzu.
	 * @param name Objektname
	 * @return neues OutputNode-Objekt
	 */
	OutputNodePtr createOutputNode( const string &name = "unnamed" );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * fuegt neuen InputNode hinzu.
	 * @param name Objektname
	 * @return neues InputNode-Objekt
	 */
	InputNodePtr createInputNode( const string &name = "unnamed" );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * entfernt InputNode
	 */
	void removeInputNode(InputNodePtr node);
	//--------------------------------------------------------------------------------------------------------
	/**
	 * entfernt OutputNode
	 */
	void removeOutputNode(OutputNodePtr node);
	//--------------------------------------------------------------------------------------------------------
	ProcessAdapter( frx::processing::IHostInfo::Ptr  hostInfo, size_t numInputNodes = 1, size_t numOutputNodes = 1 );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Wird von AdapterNode, durch AdapterNode::processNode(), aufgerufen.
	 * Bereitet Inputdatenmenge vor (falls notwendig) und leitet an processAdapter() weiter.
	 * @param numSamples Anzahl der zu verarbeitenden Samples
	 */
	void process(Processor::Int numSamples);
public:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * setted by adapternode. value is delay value of adapternode an represents the
	 * current signal delay in graph on adapter position. 
	 * @param
	 */
	void setDelay(size_t v);
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Wird von process() aufgerufen.
	 * Verarbeitet Samplemenge des Eingangsknoten und fuegt Ergebniss Ausgangsknoten hinzu.
	 * @param numSamples Anzahl der zu verarbeitenden Samples
	 */
	virtual void processAdapter ( Processor::Int numSamples ) = 0;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return IHostInfo-Objekt
	 */
	frx::processing::IHostInfo::Ptr  getHostInfo() const { return hostInfo.lock(); } 
	//--------------------------------------------------------------------------------------------------------
	/**
	 * setzt IHostInfo-Objekt
	 * @param hI
	 */
	void setHostInfo( frx::processing::IHostInfo::Ptr  hI ){ hostInfo = hI; } 
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Anzahl der enthaltenden OutputNode-Objekte
	 */
	size_t getNumOutputNodes () const { return outputNodes.size(); }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Anzahl der enthaltenden InputNode-Objekte
	 */
	size_t getNumInputNodes () const { return inputNodes.size(); }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param index
	 * @return OutputNode-Objekt zu index. Wirft ppiError::IndexOutOfBoundException
	 */
	OutputNodePtr getOutputNode( size_t index ) const { 
		if ( index >= getNumOutputNodes() ) 
			throw ppiError::IndexOutOfBoundException ( "OutOfBound-OutputNodes", __FILE__, __LINE__ );
		return outputNodes[index]; 
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return ProcessAdapterNode-Objekt
	 */
	boost::shared_ptr<ProcessAdapterNode> getAdapterNode() { return aNode; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param index
	 * @return InputNode-Objekt zu index. Wirft ppiError::IndexOutOfBoundException
	 */
	virtual InputNodePtr getInputNode( size_t index ) const { 
		if ( index >= getNumInputNodes() ) 
			throw ppiError::IndexOutOfBoundException ( "OutOfBound-InputNodes", __FILE__, __LINE__ );
		return inputNodes[index]; 
	}
	//--------------------------------------------------------------------------------------------------------
	virtual ~ProcessAdapter();
}; //class ProcessAdapter
//============================================================================================================
/**
 * @class ProcessAdapter::OutputNode.
 * ProcessAdapter OutputNode.
 */
//============================================================================================================
class ProcessAdapter::OutputNode : public NOPNode {
friend class boost::serialization::access;
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<OutputNode> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * (De)Serialisiert OutputNode.
	 * @param ar Boost::Archive-Objekt
	 * @param version
	 */
	template < typename Archive >
	void serialize ( Archive &ar, const unsigned int version ){
		ar & boost::serialization::base_object< NOPNode > ( *this );
		ar & parent;
	}
	//--------------------------------------------------------------------------------------------------------
	OutputNode() : NOPNode(""){}
	//--------------------------------------------------------------------------------------------------------
	ProcessAdapter *parent;
	//--------------------------------------------------------------------------------------------------------
	OutputNode( const string &name, ProcessAdapter* parent );
public:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Erzeugt neues OutputNode-Objekt
	 * @param name Objektname
	 * @param parent uebergeordnetes ProcessAdapter-Objekt
	 * @return
	 */
	static Ptr create( const string &name, ProcessAdapter *parent ) {
		Ptr neu( new OutputNode(name, parent) );
		neu->self = neu;
		return neu;
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Tut nix, da funktionalitaet von ProcessAdapter ausgefuehrt wird
	 * @param numSamples
	 */
	virtual void processNode( Processor::Int numSamples ) {};
	//--------------------------------------------------------------------------------------------------------
	virtual ~OutputNode(){}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return uebergeordnetes ProcessAdapter-Objekt
	 */
	ProcessAdapter::Ptr getProcessAdapter() { 
		ProcessAdapter::Ptr p = 
			boost::shared_dynamic_cast< ProcessAdapter, PObject >( parent->getPtr() );
		return p;
	}
};
//============================================================================================================
/**
 * @class ProcessAdapter::InputNode.
 * ProcessAdapter InputNode.
 */
//============================================================================================================
class ProcessAdapter::InputNode : public NOPNode {
friend class boost::serialization::access;
friend class ProcessAdapter;
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<InputNode> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * (De)Serialisiert InputNode.
	 * @param ar Boost::Archive-Objekt
	 * @param version
	 */
	template < typename Archive >
	void serialize ( Archive &ar, const unsigned int version ){
		ar & boost::serialization::base_object< NOPNode > ( *this );
		ar & parent;
	}
	//--------------------------------------------------------------------------------------------------------
	InputNode() : NOPNode("") {}
	//--------------------------------------------------------------------------------------------------------
	ProcessAdapter *parent;
	//--------------------------------------------------------------------------------------------------------
	virtual void processNode( Processor::Int numSamples ){}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * to handle issue#157, adapter calls _processNode with specific delay value before
	 * processAdapter. Original processNode does nothing.
	 * @param numSamples
	 * @param delay
	 */ 
	void _processNode( Processor::Int numSamples, size_t delay );
	//--------------------------------------------------------------------------------------------------------
	InputNode( const string &name, ProcessAdapter *parent );
public:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Erzeugt neues InputNode-Objekt
	 * @param name Objektname
	 * @param parent uebergeordnetes ProcessAdapter-Objekt
	 * @return
	 */
	static Ptr create( const string &name, ProcessAdapter *parent ) {
		Ptr neu( new InputNode(name, parent) );
		neu->self = neu;
		return neu;
	}
	//--------------------------------------------------------------------------------------------------------
	virtual ~InputNode(){}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return uebergeordnetes ProcessAdapter-Objekt
	 */
	ProcessAdapter::Ptr getProcessAdapter() { 
		ProcessAdapter::Ptr p = 
			boost::shared_dynamic_cast< ProcessAdapter, PObject >( parent->getPtr() );
		return p;
	}
};
//============================================================================================================
/**
 * @class ProcessAdapterNode.
 * Implementiert die processFrames Methode, indem es die processFrames() Methode,
 * des im Konstruktor uebergebenen ProcessAdapter-Objektes aufruft.
 */
class ProcessAdapterNode : public ProcessorNode, public Serializable {
//============================================================================================================
friend class boost::serialization::access;
friend class ProcessAdapter;
friend class ProcessAdapter::InputNode;
friend class ProcessAdapter::OutputNode;
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<ProcessAdapterNode> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * (De)Serialisiert ProcessAdapterNode-Objekt
	 * @param ar boost::Archive-Objekt
	 * @param version
	 */
	template < typename Archive >
	void serialize ( Archive &ar, const unsigned int version ){
		ar & boost::serialization::base_object< ProcessorNode > ( *this );
		ar & parent;
	}
	//--------------------------------------------------------------------------------------------------------
	ProcessAdapterNode() {}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Uebergerordneter ProcessAdapter
	 */
	ProcessAdapter *parent;
	//--------------------------------------------------------------------------------------------------------
	ProcessAdapterNode( ProcessAdapter *processAdapter );
public:
	//--------------------------------------------------------------------------------------------------------
	virtual void setNodeDelay(size_t v) {
		parent->setDelay(v);
		ProcessorNode::setNodeDelay(v);
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Erzeugt neues ProcessAdapterNode-Objekt
	 * @param processAdapter
	 * @return
	 */
	static Ptr create ( ProcessAdapter *processAdapter ) {
		Ptr neu( new ProcessAdapterNode( processAdapter ) );
		neu->self = neu;
		return neu;
	}
	//--------------------------------------------------------------------------------------------------------
	virtual ~ProcessAdapterNode();
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Signal-Verabeitungs-Verzoegerung des uebergeordneten ProcessAdapter
	 */
	virtual size_t getProcessDelay() const { return parent->getProcessDelay(); }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return uebergeordneter ProcessAdapter
	 */
	ProcessAdapter * getAdatper(){ return parent; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * keine Auswirkung, da ProcessAdapter Eingabe-Frames-Objekte verarbeitet
	 * @param
	 * @param numSamples
	 */
	virtual void processFrames ( Frames*, Processor::Int numSamples ) {}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * ruft ProcessAdapter::processAdapter() auf.
	 * @param numSamples Anzahl der zu bearbeitenden Samples.
	 */
	virtual void processNode( Processor::Int numSamples ) { parent->process(numSamples); }
}; //class ProcessAdapterNode
} // namespace Processing

#endif
