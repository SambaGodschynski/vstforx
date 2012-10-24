/*
 * Forward.hpp
 *
 *  Created on: Thu Oct 18 15:17:15 2012
 *      Author: Johannes Unger
 */

#ifndef FRX_PROCESSING_FORWARD_H
#define FRX_PROCESSING_FORWARD_H

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

namespace processing {
	class ProcessAdapter;
	typedef boost::shared_ptr<ProcessAdapter> ProcessAdapterPtr;
	typedef boost::weak_ptr<ProcessAdapter> ProcessAdapterWPtr;
	class ProcessorNode;
	typedef boost::shared_ptr<ProcessorNode> ProcessorNodePtr;
	typedef boost::weak_ptr<ProcessorNode> ProcessorNodeWPtr;
}

namespace frx { namespace processing {
	class ModelObject;
	typedef boost::shared_ptr<ModelObject> ModelObjectPtr;
	typedef boost::weak_ptr<ModelObject> ModelObjectWPtr;
	class ProcessorAdapter;
	typedef boost::shared_ptr<ProcessorAdapter> ProcessorAdapterPtr;
	typedef boost::weak_ptr<ProcessorAdapter> ProcessorAdapterWPtr;
	class NodeAdapter;
	typedef boost::shared_ptr<NodeAdapter> NodeAdapterPtr;
	typedef boost::weak_ptr<NodeAdapter> NodeAdapterWPtr;
	class ParameterAdapter;
	typedef boost::shared_ptr<ParameterAdapter> ParameterAdapterPtr;
	typedef boost::weak_ptr<ParameterAdapter> ParameterAdapterWPtr;
	class NodeConnection;
	typedef boost::shared_ptr<NodeConnection> NodeConnectionPtr;
	typedef boost::weak_ptr<NodeConnection> NodeConnectionWPtr;
	class ParameterConnection;
	typedef boost::shared_ptr<ParameterConnection> ParameterConnectionPtr;
	typedef boost::weak_ptr<ParameterConnection> ParameterConnectionWPtr;
}}

#endif /* FRX_PROCESSING_FORWARD_H */
