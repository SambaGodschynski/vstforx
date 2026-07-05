/*
 * Forward.hpp
 *
 *  Created on: Thu Oct 18 15:17:15 2012
 *      Author: Johannes Unger
 */

#ifndef FRX_PROCESSING_FORWARD_H
#define FRX_PROCESSING_FORWARD_H

#include <memory>

namespace processing {
	class ProcessAdapter;
	typedef std::shared_ptr<ProcessAdapter> ProcessAdapterPtr;
	typedef std::weak_ptr<ProcessAdapter> ProcessAdapterWPtr;
	class ProcessorNode;
	typedef std::shared_ptr<ProcessorNode> ProcessorNodePtr;
	typedef std::weak_ptr<ProcessorNode> ProcessorNodeWPtr;
}

namespace frx { namespace processing {
	class ModelObject;
	typedef std::shared_ptr<ModelObject> ModelObjectPtr;
	typedef std::weak_ptr<ModelObject> ModelObjectWPtr;
	class ProcessorAdapter;
	typedef std::shared_ptr<ProcessorAdapter> ProcessorAdapterPtr;
	typedef std::weak_ptr<ProcessorAdapter> ProcessorAdapterWPtr;
	class NodeAdapter;
	typedef std::shared_ptr<NodeAdapter> NodeAdapterPtr;
	typedef std::weak_ptr<NodeAdapter> NodeAdapterWPtr;
	class ParameterAdapter;
	typedef std::shared_ptr<ParameterAdapter> ParameterAdapterPtr;
	typedef std::weak_ptr<ParameterAdapter> ParameterAdapterWPtr;
	class PluginAdapter;
	typedef std::shared_ptr<PluginAdapter> PluginAdapterPtr;
	typedef std::weak_ptr<PluginAdapter> PluginAdapterWPtr;
	class NodeConnection;
	typedef std::shared_ptr<NodeConnection> NodeConnectionPtr;
	typedef std::weak_ptr<NodeConnection> NodeConnectionWPtr;
	class ParameterConnection;
	typedef std::shared_ptr<ParameterConnection> ParameterConnectionPtr;
	typedef std::weak_ptr<ParameterConnection> ParameterConnectionWPtr;
}}

#endif /* FRX_PROCESSING_FORWARD_H */
