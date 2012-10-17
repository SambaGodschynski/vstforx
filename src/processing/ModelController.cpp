/*
 * ModelController.cpp
 *
 *  Created on: Wed Oct 10 12:06:47 2012
 *      Author: Johannes Unger
 */

#include "ModelController.hpp"
#include "processing.h"
#include "NodeConnection.hpp"
#include <boost/bind.hpp>
#include "concreteAdapter/Volume.h"

namespace frx { namespace processing {
//=============================================================================
//  Class ModelController
//=============================================================================
//-----------------------------------------------------------------------------
ModelController::Ptr ModelController::create() {
	Ptr res(new ModelController());
	res->self = res;
	return res;
}
//-----------------------------------------------------------------------------
void ModelController::setGraph(::processing::Graph::Ptr graph) {
	this->graph = graph;
}
//-----------------------------------------------------------------------------
::processing::Graph::Ptr ModelController::getGraph() const {
	return graph;
}
//-----------------------------------------------------------------------------
void ModelController::installListeners(IProcessor::Ptr pr) {
	pr->addRemoveRequestExecuter(
		boost::bind(
			&ModelController::excuteProcessorRemoveRequest,
			this,
			_1,
			boost::weak_ptr<IProcessor>(pr)
		),
		self
	);
}
//-----------------------------------------------------------------------------
IProcessor::Ptr ModelController::createVolumeProcessor() {
	namespace pr = ::processing;
	if (!graph)
		return IProcessor::Ptr();
	pr::Volume::Ptr res =  
		pr::Volume::create(graph->getHostInfo());
	if ( graph->getJanitor()->add(res) != pr::Graph::Janitor::SUCCEED )
		return IProcessor::Ptr();
	// register remove request excutor
	installListeners(res);
	return res;
}
//-----------------------------------------------------------------------------
IProcessor::Ptr ModelController::createPanProcessor() {
	if (!graph)
		return IProcessor::Ptr();
	return IProcessor::Ptr();
}
//-----------------------------------------------------------------------------
IProcessor::Ptr 
ModelController::createInStepProcessor(size_t numInputs) {
	if (!graph)
		return IProcessor::Ptr();
	return IProcessor::Ptr();
}
//-----------------------------------------------------------------------------
IProcessor::Ptr 
ModelController::createOutStepProcessor(size_t numOutputs) {
	if (!graph)
		return IProcessor::Ptr();
	return IProcessor::Ptr();
}
//-----------------------------------------------------------------------------
IProcessor::Ptr 
ModelController::createInSwitchProcessor(size_t numInputs) {
	if (!graph)
		return IProcessor::Ptr();
	return IProcessor::Ptr();
}
//-----------------------------------------------------------------------------
IProcessor::Ptr 
ModelController::createOutSwitchProcessor(size_t numOutputs) {
	if (!graph)
		return IProcessor::Ptr();
	return IProcessor::Ptr();
}
//-----------------------------------------------------------------------------
IProcessor::Ptr ModelController::createPeakTracker() {
	if (!graph)
		return IProcessor::Ptr();
	return IProcessor::Ptr();
}
//-----------------------------------------------------------------------------
IProcessor::Ptr ModelController::createADSRTransformer() {
	if (!graph)
		return IProcessor::Ptr();
	return IProcessor::Ptr();
}
//-----------------------------------------------------------------------------
IConnection::Ptr ModelController::connect(INode::Ptr out, INode::Ptr in) {
	if (!graph)
		return IConnection::Ptr();
	::processing::ProcessorNode::Ptr src =
		boost::shared_dynamic_cast<::processing::ProcessorNode>(out);
	::processing::ProcessorNode::Ptr dst =
		boost::shared_dynamic_cast<::processing::ProcessorNode>(in);
	if (!src || !dst)
		return IConnection::Ptr();
	typedef ::processing::Graph::Janitor Janitor; 
	Janitor::Ptr jan = graph->getJanitor();
	Janitor::State res = jan->connectNodes(src, dst);
	jan.reset();
	if (res!=Janitor::SUCCEED)
		return IConnection::Ptr();
	NodeConnection::Ptr cn = NodeConnection::create();
	cn->setSource(src);
	cn->setDestination(dst);
	// register remove request excutor
	cn->addRemoveRequestExecuter(
		boost::bind(
			&ModelController::excuteConnectionRemoveRequest,
			this,
			_1,
			boost::weak_ptr<NodeConnection>(cn)
		),
		self
	);
	return cn;
}
//-----------------------------------------------------------------------------
bool ModelController::removeConnection(IConnection::Ptr cn) {
	if (!graph)
		return false;
	NodeConnection::Ptr connection = 
		boost::shared_dynamic_cast<NodeConnection>(cn);
	SAMBAG_ASSERT(connection);
	typedef ::processing::Graph::Janitor Janitor; 
	Janitor::Ptr jan = graph->getJanitor();
	Janitor::State res = 
		jan->removeConnection(connection->getSourceNode(), 
			connection->getDestinationNode()
		);
	return res == Janitor::SUCCEED;
}
//-----------------------------------------------------------------------------
bool ModelController::removeProcessor(IProcessor::Ptr cn) {
	if (!graph)
		return false;
	::processing::ProcessAdapter::Ptr pr = 
		boost::shared_dynamic_cast<::processing::ProcessAdapter>(cn);
	SAMBAG_ASSERT(pr);
	typedef ::processing::Graph::Janitor Janitor; 
	Janitor::Ptr jan = graph->getJanitor();
	Janitor::State res = jan->remove(pr);
	return res == Janitor::SUCCEED;
}
//-----------------------------------------------------------------------------
INode::Ptr ModelController::getEntry() {
	if (!graph)
		return INode::Ptr();
	return boost::shared_dynamic_cast<::processing::ProcessorNode> (
		graph->getStartNode()
	);
}
//-----------------------------------------------------------------------------
INode::Ptr ModelController::getExit() {
	if (!graph)
		return INode::Ptr();
	return boost::shared_dynamic_cast<::processing::ProcessorNode> (
		graph->getEndNode()
	);
}
//-----------------------------------------------------------------------------
bool ModelController::
excuteConnectionRemoveRequest(ModelObject::Ptr obj, 
	boost::weak_ptr<IConnection> cn)
{
	return removeConnection(cn.lock());	
}
//-----------------------------------------------------------------------------
bool ModelController::excuteProcessorRemoveRequest(ModelObject::Ptr obj, 
	boost::weak_ptr<IProcessor> cn)
{
	return removeProcessor(cn.lock());	
}

}} // namespace(s)
