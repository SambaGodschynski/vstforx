/*
 * NodeConnection.hpp
 *
 *  Created on: Thu Oct 11 06:20:59 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_CONNECTION_H
#define SAMBAG_CONNECTION_H

#include <boost/shared_ptr.hpp>
#include "INodeConnection.hpp"
#include "processing.h"
#include "NodeAdapter.hpp"

namespace frx { namespace processing {
//=============================================================================
/** 
  * @class NodeConnection.
  */
class NodeConnection : public INodeConnection {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef INodeConnection Super;
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<NodeConnection> Ptr;
	//-------------------------------------------------------------------------
protected:
	NodeConnection() {}
private:
	//-------------------------------------------------------------------------
	NodeAdapter::Ptr src;
	//-------------------------------------------------------------------------
	NodeAdapter::Ptr dst;
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
		return src->getAdaptee();
	}
	//-------------------------------------------------------------------------
	::processing::ProcessorNode::Ptr getDestinationNode() const {
		return dst->getAdaptee();
	}
	//-------------------------------------------------------------------------
	void setSource(NodeAdapter::Ptr node);
	//-------------------------------------------------------------------------
	void setDestination(NodeAdapter::Ptr node);
}; // NodeConnection
}} // namespace(s)

#endif /* SAMBAG_CONNECTION_H */
