/*
 * ===========================================================================================================
 * BglGraph.h
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#ifndef VSTFORX_BGL_GRAPH_H_
#define VSTFORX_BGL_GRAPH_H_

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
	//========================================================================================================
	// Boost Graph Library stuff
	//========================================================================================================
	/**
	 * Bgl Property-Tag (http://www.boost.org/doc/libs/1_48_0/libs/graph/doc/using_property_maps.html)
	 * jedem vertex wird ein processor_node zugeordnet
	 */
	struct processor_node_t { typedef boost::vertex_property_tag kind; };
	//--------------------------------------------------------------------------------------------------------
	typedef boost::property<processor_node_t, ProcessorNodePtr> own_property;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Benutzerdefinierter BGL::Graph Typ
	 */
	typedef boost::adjacency_list< 
		boost::listS, 
		boost::vecS, 
		boost::bidirectionalS, // zugriff auf in_edges; doppelter edge speicherbedarf 
		own_property
	> G;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * BGL Vertex-Typ
	 */
	typedef boost::graph_traits<G>::vertex_descriptor Vertex;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * BGL Edge-Typ
	 */
	typedef boost::graph_traits<G>::edge_descriptor Edge;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * BGL PropertyMap-Typ: Vertex nach ProcessorNode
	 */
	typedef boost::property_map<G, processor_node_t>::type VertexProcessorNode; // TODO: rename Vertex2ProcessorNode
	//--------------------------------------------------------------------------------------------------------
	/**
	 * BGL PropertyMap-Typ: Vertex nach ProcessorNode[const]
	 */
	typedef boost::property_map<G, processor_node_t>::const_type const_VertexProcessorNode; // TODO: rename Vertex2ProcessorNode
	//--------------------------------------------------------------------------------------------------------
	/**
	 * BGL Vertex-Iterator-Typ
	 */
	typedef boost::graph_traits<G>::vertex_iterator VertexIterator;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * BGL Edge-Iterator-Typ
	 */
	typedef boost::graph_traits<G>::edge_iterator EdgeIterator;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * BGL InEdge-Iterator-Typ (eingehende Kanten eines Vertex)
	 */
	typedef boost::graph_traits<G>::in_edge_iterator InEdgeIterator;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * BGL OutEdge-Iterator-Typ (ausgehende Kanten eines Vertex)
	 */
	typedef boost::graph_traits<G>::out_edge_iterator OutEdgeIterator;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * BGL Adjazenz-Iterator-Typ
	 */
	typedef boost::graph_traits<G>::adjacency_iterator AdjacencyIterator;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * BGL Inverse-Adjazenz-Iterator-Typ
	 */
	typedef G::inv_adjacency_iterator InvAdjacencyIterator;
}} // namespace
#endif
