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
#include "concreteAdapter/MidiProcessor.h"
#include "interprocess/RemoteChReceiver.hpp"
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
}
//-----------------------------------------------------------------------------
void ModelController::installListeners(IParameter::Ptr pr)  {
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
	pr::Graph::Janitor::Ptr jan = graph->getJanitor();
	// add processor to graph
	if ( jan->add(res) != pr::Graph::Janitor::SUCCEED ) {
		return IProcessor::Ptr();
	}
	// create invisible connection
	if ( jan->connectNodes( res->getOutputNode(0), graph->getEndNode() )
		!= pr::Graph::Janitor::SUCCEED )
	{
		return IProcessor::Ptr(); 
	}
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
	pr::Graph::Janitor::Ptr jan = graph->getJanitor();
	pr::ADSRTrigger::Ptr res =  
		pr::ADSRTrigger::create(graph->getHostInfo());
	if ( jan->add(res) != pr::Graph::Janitor::SUCCEED ) {
		return IProcessor::Ptr();
	}
	// create invisible connection
	if ( jan->connectNodes( res->getOutputNode(0), graph->getEndNode() )
		!= pr::Graph::Janitor::SUCCEED )
	{
		return IProcessor::Ptr(); 
	}
	ProcessorAdapter::Ptr ad = ProcessorAdapter::create(res);
	// register remove request excutor
	installListeners(ad);
	return ad;
}
//-----------------------------------------------------------------------------
IProcessor::Ptr ModelController::createMIDIReceiver() {
	namespace pr = ::processing;
	if (!graph)
		return IProcessor::Ptr();
	pr::Graph::Janitor::Ptr jan = graph->getJanitor();
	pr::MidiProcessor::Ptr res =  
		pr::MidiProcessor::create(graph->getHostInfo());
	if ( jan->add(res) != pr::Graph::Janitor::SUCCEED ) {
		return IProcessor::Ptr();
	}
	ProcessorAdapter::Ptr ad = ProcessorAdapter::create(res);
	// register remove request excutor
	installListeners(ad);
	return ad;
}
//-----------------------------------------------------------------------------
IProcessor::Ptr
ModelController::createRemoteChannelReceiver(const std::string &rcId)
{
    namespace pr = ::processing;
    namespace fpi = frx::processing::interprocess;
	if (!graph)
		return IProcessor::Ptr();
	pr::Graph::Janitor::Ptr jan = graph->getJanitor();
	fpi::RemoteChReceiver::Ptr res =
		fpi::RemoteChReceiver::create(graph->getHostInfo(), rcId);
	if ( jan->add(res) != pr::Graph::Janitor::SUCCEED ) {
		return IProcessor::Ptr();
	}
	ProcessorAdapter::Ptr ad = ProcessorAdapter::create(res);
	// register remove request excutor
	installListeners(ad);
	return ad;
}
//-----------------------------------------------------------------------------
IConnection::Ptr ModelController::connect(INode::Ptr out, INode::Ptr in) {
	if (!graph)
		return IConnection::Ptr();
	NodeAdapter::Ptr src = boost::dynamic_pointer_cast<NodeAdapter>(out);
	NodeAdapter::Ptr dst = boost::dynamic_pointer_cast<NodeAdapter>(in);
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
		boost::dynamic_pointer_cast<NodeConnection>(cn);
	if (connection)
		return removeConnection(connection);
	return false;
}
//-----------------------------------------------------------------------------
bool ModelController::removeProcessor(IProcessor::Ptr cn) {
	if (!graph)
		return false;
	ProcessorAdapter::Ptr pr = 
		boost::dynamic_pointer_cast<ProcessorAdapter>(cn);
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
IHostInfo::Ptr ModelController::getHostInfo() const {
	return graph->getHostInfo();
}
//-----------------------------------------------------------------------------
IProcessor::Ptr ModelController::createPlugin(const ::processing::PluginInfo &pI)
{
	
	::processing::Plugin::Ptr plugin;
	::com::PluginCollection::Ptr pC;
	try {
		pC = ::com::getPluginCollection();
	} catch (...) {
		return IProcessor::Ptr();
	}
	::processing::PluginInfo pluginInfo = pI;
	try {
		plugin = pC->restorePlugNode( getHostInfo(), pluginInfo ); 
	} catch(const ::processing::ShellPluginException) {
		throw;
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
bool ModelController::removeParameter(IParameter::Ptr p) {
	ParameterAdapter::Ptr ada = boost::dynamic_pointer_cast<ParameterAdapter>(p);
	if (ada->isHostParameter()) {
		return true;
	}
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
		res->setIsHostParameter(true);
		tmpHostParameter[id] = res;
	}
	return tmpHostParameter[id];
}
//-----------------------------------------------------------------------------
int ModelController::getNumHostParameter() {
	return (int)graph->getNumHostParameter();
}
//-----------------------------------------------------------------------------
INode::Ptr ModelController::addInputTo(IProcessor::Ptr pr) {
	typedef ::processing::Graph::Janitor Janitor; 
	// get janitor respectively lock graph
	Janitor::Ptr jan = graph->getJanitor();
	// add i/o to processor
	INode::Ptr res = pr->addInput();
	NodeAdapter::Ptr src =
		boost::dynamic_pointer_cast<NodeAdapter>(res);
	if (!src)
		return INode::Ptr();
	
	// get concrete node (adaptee)
	::processing::ProcessAdapter::InputNode::Ptr atom = 
		boost::dynamic_pointer_cast< ::processing::ProcessAdapter::InputNode >(
			src->getAdaptee()
		);
	// add concrete node to graph
	if (jan->add(atom)!=Janitor::SUCCEED) {
		return INode::Ptr();
	}
	return res;
}
//-----------------------------------------------------------------------------
INode::Ptr ModelController::addOutputTo(IProcessor::Ptr pr) {
	typedef ::processing::Graph::Janitor Janitor; 
	// get janitor respectively lock graph
	Janitor::Ptr jan = graph->getJanitor();
	// add i/o to processor
	INode::Ptr res = pr->addOutput();
	NodeAdapter::Ptr src =
		boost::dynamic_pointer_cast<NodeAdapter>(res);
	if (!src)
		return INode::Ptr();
	
	// get concrete node (adaptee)
	typedef ::processing::Graph::Janitor Janitor; 
	::processing::ProcessAdapter::OutputNode::Ptr atom = 
		boost::dynamic_pointer_cast< ::processing::ProcessAdapter::OutputNode >(
			src->getAdaptee()
		);
	// add concrete node to graph
	if (jan->add(atom)!=Janitor::SUCCEED) {
		return INode::Ptr();
	}
	return res;
}
//-------------------------------------------------------------------------
void ModelController::getParameterCnOpTypeIds(ParameterCnOpTypeIds &out) const {
	ParameterConnection::getParameterCnOpTypeIds(out);
}
//-------------------------------------------------------------------------
void ModelController::
addParameterCnOp(IConnection::Ptr cn, const ParameterCnOpTypeId &opId) 
{
	ParameterConnection::Ptr pcn =
		boost::dynamic_pointer_cast<ParameterConnection>(cn);
	if (!pcn) {
		return;
	}
	pcn->addParameterCnOp(opId);
}
}} // namespace(s)
