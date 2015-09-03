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
#include "ConcreteProcessAdapter.h"
#include "interprocess/RemoteChReceiver.hpp"
#include "IHostInfo.h"
#include <processing/ProcessorAdapter.hpp>
#include <processing/ParameterAdapter.hpp>
#include <processing/NodeAdapter.hpp>
#include <com/one4All.h>
#include <processing/Plugin.h>
#include <processing/pluginTypes/VST2xImpl.h>
#include "PluginAdapter.hpp"
#include <processing/ModelFactory.hpp>
#include <processing/pluginTypes/VstShellPlugin.hpp>

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
IProcessor::Ptr ModelController::createProcessor(const std::string &idStr,
    bool autoConnectOutput)
{
    
    try {
        namespace pr = ::processing;
        if (!graph) {
            return IProcessor::Ptr();
        }
        ::com::IdParser id(idStr);
        if (id.name() == "Plugin") {
            return createPlugin(idStr);
        }
    
        pr::Graph::Janitor::Ptr jan = graph->getJanitor();
	
        pr::ProcessAdapter::Ptr res =
            ModelFactory::instance().create(idStr, graph->getHostInfo());
        jan->add(res);
        if (autoConnectOutput) {
            // create invisible connection
            jan->connectNodes(res->getOutputNode(0), graph->getEndNode());
        }
        ProcessorAdapter::Ptr ad = ProcessorAdapter::create(res);
        // register remove request excutor
        installListeners(ad);
        return ad;
    } catch (const ::processing::ShellPluginException &ex) {
        throw;
    } catch (const std::exception &ex) {
        throw std::runtime_error("creating " + idStr + " failed: " + ex.what());
    } catch (...) {
        throw std::runtime_error("creating " + idStr + " failed: unknown error.");
    }
}
//-----------------------------------------------------------------------------
IProcessor::Ptr ModelController::createPlugin(const std::string &id)
{
    Plugin::Ptr plugin;
    plugin = ModelFactory::instance().create<Plugin>(id, graph->getHostInfo());
		
	if ( graph->getJanitor()->add(plugin) != ::processing::Graph::Janitor::SUCCEED )
		return IProcessor::Ptr();
	PluginAdapter::Ptr adapter = PluginAdapter::create();
	adapter->setAdaptee(plugin);
	// register remove request excutor
	installListeners(adapter);
	return adapter;
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
INode::Ptr ModelController::getEntry(int idx) {
	if (!graph)
		return INode::Ptr();
    if (idx>=entries.size()) {
        entries.resize(idx+1);
    }
	if (!entries[idx]) {
		::processing::ProcessorNode::Ptr o = graph->getStartNode(idx);
		entries[idx] = NodeAdapter::create(o);
	}
	return entries[idx];
}
//-----------------------------------------------------------------------------
INode::Ptr ModelController::getExit(int idx) {
	if (!graph)
		return INode::Ptr();
    if (idx>=exits.size()) {
        exits.resize(idx+1);
    }
	if (!exits[idx]) {
		::processing::ProcessorNode::Ptr o = graph->getEndNode(idx);
		exits[idx] = NodeAdapter::create(o);
	}
	return exits[idx];
}
//-------------------------------------------------------------------------
size_t ModelController::getNumEntries() const {
    if (!graph) {
        return 0;
    }
    return graph->getNumStartNodes();
}
//-------------------------------------------------------------------------
size_t ModelController::getNumExits() const {
    if (!graph) {
        return 0;
    }
    return graph->getNumEndNodes();
}
//-----------------------------------------------------------------------------
IHostInfo::Ptr ModelController::getHostInfo() const {
	return graph->getHostInfo();
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
//-----------------------------------------------------------------------------
void ModelController::getParameterCnOpTypeIds(ParameterCnOpTypeIds &out) const {
	ParameterConnection::getParameterCnOpTypeIds(out);
}
//-----------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------
int ModelController::getGraphDelay() const {
    return (int)graph->getGraphDelay();
}
//-----------------------------------------------------------------------------
void ModelController::updateGraph() {
    graph->getJanitor()->updateGraph();
}
}} // namespace(s)
