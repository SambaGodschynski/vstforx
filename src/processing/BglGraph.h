#ifndef BGL_GRAPH_H_
#define BGL_GRAPH_H_

#include <boost/utility.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/visitors.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/graph/adj_list_serialize.hpp>

namespace processing {
class ProcessorNode;
typedef boost::shared_ptr<ProcessorNode> ProcessorNodePtr;
namespace bgl {
	//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	// Boost Graph Library stuff
	//........................................................................................................
	// jedem vertex wird ein processor_node zugeordnet
	struct processor_node_t { typedef boost::vertex_property_tag kind; };
	//........................................................................................................
	typedef boost::property<processor_node_t, ProcessorNodePtr> own_property;
	//........................................................................................................
	// bgl graph
	typedef boost::adjacency_list< 
		boost::listS, 
		boost::vecS, 
		boost::bidirectionalS, // zugriff auf in_edges; doppelter edge speicherbedarf 
		own_property
	> G;
	//........................................................................................................
	typedef boost::graph_traits<G>::vertex_descriptor Vertex;
	//........................................................................................................
	typedef boost::graph_traits<G>::edge_descriptor Edge;
	//........................................................................................................
	typedef boost::property_map<G, processor_node_t>::type VertexProcessorNode;
	//........................................................................................................
	typedef boost::property_map<G, processor_node_t>::const_type const_VertexProcessorNode;
	//........................................................................................................
	typedef boost::graph_traits<G>::vertex_iterator VertexIterator;
	//........................................................................................................
	typedef boost::graph_traits<G>::edge_iterator EdgeIterator;
	//........................................................................................................
	typedef boost::graph_traits<G>::in_edge_iterator InEdgeIterator;
	//........................................................................................................
	typedef boost::graph_traits<G>::out_edge_iterator OutEdgeIterator;
	//........................................................................................................
	typedef boost::graph_traits<G>::adjacency_iterator AdjacencyIterator;
	//........................................................................................................
	typedef G::inv_adjacency_iterator InvAdjacencyIterator;
	//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
} // namespace 
} // namespace
#endif