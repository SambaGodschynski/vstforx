/*
 * NodeConnection.cpp
 *
 *  Created on: Thu Oct 11 06:20:59 2012
 *      Author: Johannes Unger
 */

#include "NodeConnection.hpp"
#include <sambag/com/exceptions/IllegalArgumentException.hpp>

namespace frx { namespace processing {
//=============================================================================
//  Class NodeConnection
//=============================================================================
//-----------------------------------------------------------------------------
void NodeConnection::setSource(::processing::ProcessorNode::Ptr node) {
	if (!node) {
		SAMBAG_THROW(
			sambag::com::exceptions::IllegalArgumentException,
			"source node cannot be NULL"
		);
	}
	src = node;
}
//-----------------------------------------------------------------------------
void NodeConnection::setDestination(::processing::ProcessorNode::Ptr node) {
	if (!node) {
		SAMBAG_THROW(
			sambag::com::exceptions::IllegalArgumentException,
			"destintaion node cannot be NULL"
		);
	}
	dst = node;
}

}} // namespace(s)
