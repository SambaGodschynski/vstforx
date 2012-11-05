/*
 * ModelController.cpp
 *
 *  Created on: Wed Oct 10 12:06:47 2012
 *      Author: Johannes Unger
 */

#include "ModelController.hpp"
#include "processing.h"
#include "NodeConnection.hpp"
#include "ParameterConnection.hpp"
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include "concreteAdapter/Volume.h"
#include "concreteAdapter/Pan.h"
#include "concreteAdapter/InputStep.h"
#include "concreteAdapter/OutputStep.h"
#include "concreteAdapter/InputSwitch.h"
#include "concreteAdapter/OutputSwitch.h"
#include "concreteAdapter/PeakTracker.h"
#include "concreteAdapter/ADSRTrigger.h"
#include "IHostInfo.h"
#include <processing/ProcessorAdapter.hpp>
#include <processing/ParameterAdapter.hpp>
#include <processing/NodeAdapter.hpp>
#include <com/PluginCollection.h>
#include <processing/Plugin.h>
#include <processing/pluginTypes/VSTPlugin2x.h>
#include "PluginAdapter.hpp"

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
	if (!graph)
		return;
	tmpHostParameter.clear();
	tmpHostParameter.resize(graph->getNumHostParameter(), IParameter::Ptr());
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
void ModelController::installListeners(IParameter::Ptr pr)  {
	pr->addRemoveRequestExecuter(
		boost::bind(
			&ModelController::excuteParameterRemoveRequest,
			this,
			_1,
			boost::weak_ptr<IParameter>(pr)
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
	ProcessorAdapter::Ptr ad = ProcessorAdapter::create(res);
	// register remove request excutor
	installListeners(ad);
	return ad;
}
//-----------------------------------------------------------------------------
IProcessor::Ptr ModelController::createPanProcessor() {
	namespace pr = ::processing;
	if (!graph)
		return IProcessor::Ptr();
	pr::Pan::Ptr res =  
		pr::Pan::create(graph->getHostInfo());
	if ( graph->getJanitor()->add(res) != pr::Graph::Janitor::SUCCEED )
		return IProcessor::Ptr();
	ProcessorAdapter::Ptr ad = ProcessorAdapter::create(res);
	// register remove request excutor
	installListeners(ad);
	return ad;
}
//-----------------------------------------------------------------------------
IProcessor::Ptr 
ModelController::createInStepProcessor(size_t numInputs) {
	namespace pr = ::processing;
	if (!graph)
		return IProcessor::Ptr();
	pr::InputStep::Ptr res =  
		pr::InputStep::create(graph->getHostInfo(), numInputs);
	if ( graph->getJanitor()->add(res) != pr::Graph::Janitor::SUCCEED )
		return IProcessor::Ptr();
	ProcessorAdapter::Ptr ad = ProcessorAdapter::create(res);
	// register remove request excutor
	installListeners(ad);
	return ad;
}
//-----------------------------------------------------------------------------
IProcessor::Ptr 
ModelController::createOutStepProcessor(size_t numOutputs) {
	namespace pr = ::processing;
	if (!graph)
		return IProcessor::Ptr();
	pr::OutputStep::Ptr res =  
		pr::OutputStep::create(graph->getHostInfo(), numOutputs);
	if ( graph->getJanitor()->add(res) != pr::Graph::Janitor::SUCCEED )
		return IProcessor::Ptr();
	ProcessorAdapter::Ptr ad = ProcessorAdapter::create(res);
	// register remove request excutor
	installListeners(ad);
	return ad;
}
//-----------------------------------------------------------------------------
IProcessor::Ptr 
ModelController::createInSwitchProcessor(size_t numInputs) {
	namespace pr = ::processing;
	if (!graph)
		return IProcessor::Ptr();
	pr::InputSwitch::Ptr res =  
		pr::InputSwitch::create(graph->getHostInfo(), numInputs);
	if ( graph->getJanitor()->add(res) != pr::Graph::Janitor::SUCCEED )
		return IProcessor::Ptr();
	ProcessorAdapter::Ptr ad = ProcessorAdapter::create(res);
	// register remove request excutor
	installListeners(ad);
	return ad;
}
//-----------------------------------------------------------------------------
IProcessor::Ptr 
ModelController::createOutSwitchProcessor(size_t numOutputs) {
	namespace pr = ::processing;
	if (!graph)
		return IProcessor::Ptr();
	pr::OutputSwitch::Ptr res =  
		pr::OutputSwitch::create(graph->getHostInfo(), numOutputs);
	if ( graph->getJanitor()->add(res) != pr::Graph::Janitor::SUCCEED )
		return IProcessor::Ptr();
	ProcessorAdapter::Ptr ad = ProcessorAdapter::create(res);
	// register remove request excutor
	installListeners(ad);
	return ad;
}
//-----------------------------------------------------------------------------
IProcessor::Ptr ModelController::createPeakTracker() {
	namespace pr = ::processing;
	if (!graph)
		return IProcessor::Ptr();
	pr::PeakTracker::Ptr res =  
		pr::PeakTracker::create(graph->getHostInfo());
	if ( graph->getJanitor()->add(res) != pr::Graph::Janitor::SUCCEED )
		return IProcessor::Ptr();
	ProcessorAdapter::Ptr ad = ProcessorAdapter::create(res);
	// register remove request excutor
	installListeners(ad);
	return ad;
}
//-----------------------------------------------------------------------------
IProcessor::Ptr ModelController::createADSRTransformer() {
	namespace pr = ::processing;
	if (!graph)
		return IProcessor::Ptr();
	pr::ADSRTrigger::Ptr res =  
		pr::ADSRTrigger::create(graph->getHostInfo());
	if ( graph->getJanitor()->add(res) != pr::Graph::Janitor::SUCCEED )
		return IProcessor::Ptr();
	ProcessorAdapter::Ptr ad = ProcessorAdapter::create(res);
	// register remove request excutor
	installListeners(ad);
	return ad;
}
//-----------------------------------------------------------------------------
IConnection::Ptr ModelController::connect(INode::Ptr out, INode::Ptr in) {
	if (!graph)
		return IConnection::Ptr();
	NodeAdapter::Ptr src = boost::shared_dynamic_cast<NodeAdapter>(out);
	NodeAdapter::Ptr dst = boost::shared_dynamic_cast<NodeAdapter>(in);
	if (!src || !dst)
		return IConnection::Ptr();
	typedef ::processing::Graph::Janitor Janitor; 
	Janitor::Ptr jan = graph->getJanitor();
	Janitor::State res = jan->connectNodes(src->getAdaptee(), 
		dst->getAdaptee());
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
IConnection::Ptr ModelController::connect(IParameter::Ptr a, IParameter::Ptr b)
{
	return ParameterConnection::createConnection(a, b);
}
//-----------------------------------------------------------------------------
bool ModelController::removeConnection(NodeConnectionPtr connection) {
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
bool ModelController::removeConnection(IConnection::Ptr cn) {
	if (!graph)
		return false;
	NodeConnection::Ptr connection = 
		boost::shared_dynamic_cast<NodeConnection>(cn);
	if (connection)
		return removeConnection(connection);
	return false;
}
//-----------------------------------------------------------------------------
bool ModelController::removeProcessor(IProcessor::Ptr cn) {
	if (!graph)
		return false;
	ProcessorAdapter::Ptr pr = 
		boost::shared_dynamic_cast<ProcessorAdapter>(cn);
	SAMBAG_ASSERT(pr);
	typedef ::processing::Graph::Janitor Janitor; 
	Janitor::Ptr jan = graph->getJanitor();
	Janitor::State res = jan->remove(pr->getAdaptee());
	return res == Janitor::SUCCEED;
}
//-----------------------------------------------------------------------------
INode::Ptr ModelController::getEntry() {
	if (!graph)
		return INode::Ptr();
	if (!entry) {
		::processing::ProcessorNode::Ptr o = graph->getStartNode();
		entry = NodeAdapter::create(o);
	}
	return entry;
}
//-----------------------------------------------------------------------------
INode::Ptr ModelController::getExit() {
	if (!graph)
		return INode::Ptr();
	if (!exit) {
		::processing::ProcessorNode::Ptr o = graph->getEndNode();
		exit = NodeAdapter::create(o);
	}
	return exit;
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
//-----------------------------------------------------------------------------
bool ModelController::excuteParameterRemoveRequest(ModelObject::Ptr obj, 
	boost::weak_ptr<IParameter> cn)
{
	return removeFreeParameter(cn.lock());	
}
//-----------------------------------------------------------------------------
IHostInfo::Ptr ModelController::getHostInfo() const {
	return graph->getHostInfo();
}
//-----------------------------------------------------------------------------
IProcessor::Ptr ModelController::createPlugin(const ::processing::PluginInfo &pI)
{
	
	::processing::Plugin::Ptr plugin;
	::com::PluginCollection &pC = ::com::getPluginCollection();
	::processing::PluginInfo pluginInfo = pI;
	try {
		plugin = pC.restorePlugNode( getHostInfo(), pluginInfo ); 
	} catch(const ::processing::VSTPlugin::ShellPluginException &ex) {
		// TODO:
		return IProcessor::Ptr();
	} catch(...) {
		return IProcessor::Ptr(); 
	}
		
	if ( graph->getJanitor()->add(plugin) != ::processing::Graph::Janitor::SUCCEED )
		return IProcessor::Ptr();
	PluginAdapter::Ptr adapter = PluginAdapter::create();
	adapter->setAdaptee(plugin);
	// register remove request excutor
	installListeners(adapter);
	return adapter;
}
//-----------------------------------------------------------------------------
IParameter::Ptr ModelController::createFreeParameter() {
	using ::processing::parameter::Parameter;
	ParameterAdapter::Ptr res = ParameterAdapter::create();
	Parameter::Ptr p = Parameter::create();
	if ( graph->getJanitor()->add(p) != ::processing::Graph::Janitor::SUCCEED )
		return IParameter::Ptr();
	res->setAdaptee(p);
	installListeners(res);
	return res;
}
//-----------------------------------------------------------------------------
bool ModelController::removeFreeParameter(IParameter::Ptr p) {
	ParameterAdapter::Ptr ada = boost::shared_dynamic_cast<ParameterAdapter>(p);
	SAMBAG_ASSERT(ada);
	return 
		graph->getJanitor()->remove(ada->getAdaptee()) 
			== ::processing::Graph::Janitor::SUCCEED;
}
//-----------------------------------------------------------------------------
IParameter::Ptr ModelController::getHostParameter(int id) {
	if (id>(int)tmpHostParameter.size())
		return IParameter::Ptr();
	if (!tmpHostParameter[id]) {
		ParameterAdapter::Ptr res = ParameterAdapter::create();
		res->setAdaptee( graph->getHostParameter((size_t)id) );
		tmpHostParameter[id] = res;
	}
	return tmpHostParameter[id];
}
//-----------------------------------------------------------------------------
int ModelController::getNumHostParameter() {
	return (int)graph->getNumHostParameter();
}
}} // namespace(s)
