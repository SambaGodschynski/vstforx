/*
 * NodeConnection.hpp
 *
 *  Created on: Thu Oct 11 06:20:59 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_CONNECTION_H
#define SAMBAG_CONNECTION_H

#include <boost/shared_ptr.hpp>
#include "processing.h"
#include "IConnection.hpp"

namespace frx { namespace processing {
//=============================================================================
/** 
  * @class NodeConnection.
  */
class NodeConnection : public IConnection {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<NodeConnection> Ptr;
	//-------------------------------------------------------------------------
protected:
	NodeConnection() {}
private:
	//-------------------------------------------------------------------------
	::processing::ProcessorNode::Ptr src;
	//-------------------------------------------------------------------------
	::processing::ProcessorNode::Ptr dst;
	//-------------------------------------------------------------------------
public:
	//-------------------------------------------------------------------------
	static Ptr create() {
		return Ptr(new NodeConnection());
	}
	//-------------------------------------------------------------------------
	virtual ModelObject::Ptr getSource() const {
		return src;
	}
	//-------------------------------------------------------------------------
	virtual ModelObject::Ptr getDestination() const {
		return dst;
	}
	//-------------------------------------------------------------------------
	::processing::ProcessorNode::Ptr getSourceNode() const {
		return src;
	}
	//-------------------------------------------------------------------------
	::processing::ProcessorNode::Ptr getDestinationNode() const {
		return dst;
	}
	//-------------------------------------------------------------------------
	void setSource(::processing::ProcessorNode::Ptr node);
	//-------------------------------------------------------------------------
	void setDestination(::processing::ProcessorNode::Ptr node);
}; // NodeConnection
}} // namespace(s)

#endif /* SAMBAG_CONNECTION_H */
