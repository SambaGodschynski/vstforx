#ifndef GRAPH_BUILDER_H
#define GRAPH_BUILDER_H

#include "processing/Graph.h"
#include "processing/ConcreteProcessAdapter.h"


/*
	Klassen zum erstellen von Komplexen Graph Konstrukten.

	Creator						=> Basis fuer alle ersteller;
	CreateSeries<Creator, N>	=> erzeugt N seriell veknuepfungen aus dem vom creator erzeugten
	CreateParallel<Creator, N>	=> erzeugt N parallel veknuepfungen aus dem vom creator erzeugten
	CreateTree<Creator, N>		=> erzeugt baum veknuepfungen der Tiefe N aus dem vom creator erzeugten
	CreateAdapter<Adapter>		=> erzeugt ProcessAdapter
	
	bsp.:
	CreateSerial< CreateAdapter<Volume>, 3 > creator(graph, entryNode, exitNode);

*/

//================================================================================
// Template Meta Pow
//================================================================================
template < int N, int M >
struct POW {
	enum { RESULT = POW<N, M-1>::RESULT * N }; 
};
template < int N >
struct POW < N, 1 > {
	enum { RESULT = N };
};


namespace processing {
//--------------------------------------------------------------------------------
typedef ProcessorNode::Ptr EntryNode;
//--------------------------------------------------------------------------------
typedef ProcessorNode::Ptr ExitNode;
//--------------------------------------------------------------------------------
struct NullType {};
//================================================================================
class HelperNode : public NOPNode {
//================================================================================
friend class boost::serialization::access;
private:
	//----------------------------------------------------------------------------
	HelperNode() : NOPNode("Helper Node") {}
	//----------------------------------------------------------------------------
	template< typename Archive >
	void serialize ( Archive &ar, const unsigned int version ) {		
		ar & boost::serialization::base_object< NOPNode > ( *this );
	} 
public:
	//----------------------------------------------------------------------------
	typedef boost::shared_ptr<HelperNode> Ptr;
	//----------------------------------------------------------------------------
	static Ptr create() {
		Ptr neu( new HelperNode() );
		neu->self = neu;
		return neu;
	}
};
//================================================================================
class Creator {
//================================================================================
protected:
	//----------------------------------------------------------------------------
	Graph::Ptr graph;
public:
	//----------------------------------------------------------------------------
	enum { NUM_CREATED_ADAPTER = 0 };
	//----------------------------------------------------------------------------
	Creator ( Graph::Ptr graph ) 
		: graph(graph) {}
};
//================================================================================
template < class Adapter > 
class CreateAdapter : public Creator {
//================================================================================
public:
	//----------------------------------------------------------------------------
	enum { NUM_CREATED_ADAPTER = 1 };
	//----------------------------------------------------------------------------
	CreateAdapter ( Graph::Ptr graph, EntryNode entryNode, ExitNode exitNode );
};
//================================================================================
template < class _Creator, int N > 
class CreateSeries : public Creator {
//================================================================================
public:
	//----------------------------------------------------------------------------
	typedef CreateSeries<_Creator, N-1 > N_LESS_1;
	//----------------------------------------------------------------------------
	enum { INDEX = N };
	//----------------------------------------------------------------------------
	enum { NUM_CREATED_ADAPTER = _Creator::NUM_CREATED_ADAPTER * N };
	//----------------------------------------------------------------------------
	CreateSeries ( Graph::Ptr graph, EntryNode entryNode, ExitNode exitNode );
};
//================================================================================
template < typename _Creator > 
class CreateSeries< _Creator, 0 > : public Creator {
//================================================================================
public:
	//----------------------------------------------------------------------------
	typedef NullType N_LESS_1;
	//----------------------------------------------------------------------------
	enum { INDEX = 0 };
	//----------------------------------------------------------------------------
	enum { NUM_CREATED_ADAPTER = 0 };
	//----------------------------------------------------------------------------
	CreateSeries ( Graph::Ptr graph, EntryNode entryNode, ExitNode exitNode ) 
		: Creator(graph) 
	{
		graph->getJanitor()->connectNodes( entryNode.get(), exitNode.get() );
	}
};
//================================================================================
template < class _Creator, int N > 
class CreateParallel : public Creator {
//================================================================================
public:
	//----------------------------------------------------------------------------
	enum { INDEX = N };
	//----------------------------------------------------------------------------
	typedef CreateParallel<_Creator, N-1 > N_LESS_1;
	//----------------------------------------------------------------------------
	enum { NUM_CREATED_ADAPTER = _Creator::NUM_CREATED_ADAPTER * N  };
	//----------------------------------------------------------------------------
	CreateParallel ( Graph::Ptr graph, EntryNode entryNode, ExitNode exitNode );
};

//================================================================================
template < typename _Creator > 
class CreateParallel< _Creator, 0 > : public Creator {
//================================================================================
public:
	//----------------------------------------------------------------------------
	typedef NullType N_LESS_1;
	//----------------------------------------------------------------------------
	enum { INDEX = 0 };
	//----------------------------------------------------------------------------
	enum { NUM_CREATED_ADAPTER = 0 };
	//----------------------------------------------------------------------------
	CreateParallel ( Graph::Ptr graph, EntryNode entryNode, ExitNode exitNode ) : 
	  Creator(graph) {}
};
//================================================================================
template < class _Creator, int N > 
class CreateBinaryTree : public Creator {
//================================================================================
public:
	//----------------------------------------------------------------------------
	typedef CreateBinaryTree<_Creator, N-1 > N_LESS_1;
	//----------------------------------------------------------------------------
	enum { INDEX = N };
	//----------------------------------------------------------------------------
	enum { NUM_CREATED_ADAPTER = 
		       _Creator::NUM_CREATED_ADAPTER * POW<2,N>::RESULT - 2
	};
	//----------------------------------------------------------------------------
	CreateBinaryTree ( Graph::Ptr graph, EntryNode entryNode, ExitNode exitNode );
};
//================================================================================
template < typename _Creator > 
class CreateBinaryTree< _Creator, 1 > : public Creator {
//================================================================================
public:
	//----------------------------------------------------------------------------
	typedef NullType N_LESS_1;
	//----------------------------------------------------------------------------
	enum { INDEX = 1 };
	//----------------------------------------------------------------------------
	CreateBinaryTree ( Graph::Ptr graph, EntryNode entryNode, ExitNode exitNode ) 
		: Creator(graph) 
	{
		graph->getJanitor()->connectNodes( entryNode.get(), exitNode.get() );
	}
};
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//							  template funk defs.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//================================================================================
// class CreateAdapter 
//================================================================================
//--------------------------------------------------------------------------------
template < class Adapter > 
CreateAdapter<Adapter>::CreateAdapter( Graph::Ptr graph, 
									   EntryNode entryNode, 
									   ExitNode exitNode ) 
									   : Creator( graph ) 
{
	typename Adapter::Ptr neu( Adapter::create( graph.get() ) );
	Graph::Janitor::Ptr janitor = graph->getJanitor();
	janitor->add ( neu );
	janitor->connectNodes( entryNode.get(), neu->getInputNode(0).get() );
	janitor->connectNodes( neu->getOutputNode(0).get(), exitNode.get() );
}
//================================================================================
// class CreateParallel
//================================================================================
//--------------------------------------------------------------------------------
template < class _Creator, int N > 
CreateParallel<_Creator, N>::CreateParallel( Graph::Ptr graph, 
										    EntryNode entryNode, 
											ExitNode exitNode 
											) : Creator( graph ) 
{
	Graph::Janitor::Ptr jan = graph->getJanitor();
	_Creator ( graph, entryNode, exitNode );
	CreateParallel< _Creator, N-1 > ( graph, entryNode, exitNode );
}
//================================================================================
// class CreateSeries
//================================================================================
//--------------------------------------------------------------------------------
template < class _Creator, int N > 
CreateSeries<_Creator, N>::CreateSeries( Graph::Ptr graph, 
										 EntryNode entryNode, 
										 ExitNode exitNode 
										 ) : Creator( graph ) 
{
	/*
		O <- entryNode
		|
		O <- creation
		|
		O <- helperNode
		|
	   ...
	    |
	    O <- exitNode
		----------------
		N-1 bekommt helperNode als entry und exitNode als exit,
		ist N == 0 wird entry mit exit verbunden.
	*/
	HelperNode::Ptr hl = HelperNode::create();
	graph->getJanitor()->add( hl );
	_Creator ( graph, entryNode, hl );
	CreateSeries< _Creator, N-1 > ( graph, hl, exitNode );
}
//================================================================================
// class CreateBinaryTree
//================================================================================
//--------------------------------------------------------------------------------
template < class _Creator, int N > 
CreateBinaryTree<_Creator, N>::CreateBinaryTree( Graph::Ptr graph, 
												 EntryNode entryNode, 
												 ExitNode exitNode 
												 ) : Creator( graph ) 
{
	HelperNode::Ptr hl = HelperNode::create();
	HelperNode::Ptr hl2 = HelperNode::create();
	graph->getJanitor()->add( hl );
	graph->getJanitor()->add( hl2 );
	_Creator ( graph, entryNode, hl );
	_Creator ( graph, entryNode, hl2 );
	CreateBinaryTree< _Creator, N-1 > ( graph, hl, exitNode );
	CreateBinaryTree< _Creator, N-1 > ( graph, hl2, exitNode );
}

}

#endif