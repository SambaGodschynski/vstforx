/*
 * FrxComponentFactory.cpp
 *
 *  Created on: Thu Nov  1 10:17:13 2012
 *      Author: Johannes Unger
 */

#include "FrxComponentFactory.hpp"
#include <gui/FrxControl.hpp>
#include "FrxCircuidView.hpp"
#include "FrxConcreteProcessor.hpp"
#include <gui/__ModelExecutors.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/bind.hpp>
#include <sambag/com/Exception.hpp>
#include <sambag/com/exceptions/IllegalStateException.hpp>
#include <boost/foreach.hpp>
#include <processing/IModelController.hpp>

namespace frx { namespace gui { namespace components {
namespace {
//-----------------------------------------------------------------------------
void registerProcessor(IViewModelMap::Ptr map,
					   FrxProcessorNode::Ptr v,
					   frx::processing::IProcessor::Ptr m) 
{
	map->registerObjects(v, m);
	const FrxProcessorNode::IOContainer &ins = v->getInputs();
	int num = std::min(ins.size(), m->getNumInputs());
	for (int i=0; i<num; ++i) {
		frx::processing::INode::Ptr node = m->getInput(i);
		map->registerObjects(ins[i], node);
	}
	const FrxProcessorNode::IOContainer &outs = v->getOutputs();
	num = std::min(outs.size(), m->getNumOutputs());
	for (int i=0; i<num; ++i) {
		frx::processing::INode::Ptr node = m->getOutput(i);
		map->registerObjects(outs[i], node);
	}
}
//-----------------------------------------------------------------------------
template <class ConcreteProcessor>
FrxProcessorNodePtr createProcessor(FrxCircuidViewPtr circ, int numInputs, int numOutputs) 
{
	if (!circ) {
		SAMBAG_THROW(sambag::com::exceptions::IllegalStateException, 
			"tried to add processor with FrxCircuidViewPtr == NULL");
	}
	// create view obj
	typename ConcreteProcessor::Ptr viewObj = ConcreteProcessor::create();
	if (!viewObj) {
		return FrxProcessorNodePtr();
	}
	viewObj->configIO(numInputs, numOutputs);
	// create model obj.
	frx::processing::IModelController::Ptr ctrl;
	IViewModelMap::Ptr map;
	boost::tie(ctrl, map) = getControllerAndMap(circ);

	frx::processing::IProcessor::Ptr mObj = 
		createProcessorOnModel<ConcreteProcessor>(ctrl, numInputs, numOutputs);
	if (!mObj) {
		SAMBAG_THROW(sambag::com::exceptions::IllegalStateException, 
			"could'nt create processor object.");
	}
	// register
	registerProcessor(map, viewObj, mObj);
	return viewObj;
}
//-----------------------------------------------------------------------------
FrxProcessorNodePtr createPlugin(FrxCircuidViewPtr circ, ::processing::PluginInfo pI) 
{
	if (!circ) {
		SAMBAG_THROW(sambag::com::exceptions::IllegalStateException, 
			"tried to add processor with FrxCircuidViewPtr == NULL");
	}
	// create model obj.
	frx::processing::IModelController::Ptr ctrl;
	IViewModelMap::Ptr map;
	boost::tie(ctrl, map) = getControllerAndMap(circ);

	frx::processing::IProcessor::Ptr mObj = ctrl->createPlugin(pI);
	if (!mObj) {
		SAMBAG_THROW(sambag::com::exceptions::IllegalStateException, 
			"could'nt create processor object.");
	}
	// create view obj.
	FrxPluginNode::Ptr viewObj = FrxPluginNode::create();
	if (!viewObj) {
		return FrxProcessorNodePtr();
	} 
	viewObj->configIO(mObj->getNumInputs(), mObj->getNumOutputs());
	registerProcessor(map, viewObj, mObj);
	return viewObj;
}
template <class ConcreteProcessor>
FrxComponentFactory::ProcessorCreator getCreator(int numIns, int numOuts) 
{
	return 
		boost::bind(&createProcessor<ConcreteProcessor>, _1, numIns, numOuts);
}
} // namespace(s)
//=============================================================================
//  Class FrxComponentFactory
//=============================================================================
//-----------------------------------------------------------------------------
void FrxComponentFactory::initMap() {
	processorMap = boost::assign::map_list_of
	(getProcessorName<FrxVolumeNode::ProcessorType>(),           getCreator<FrxVolumeNode>(1, 1))
	(getProcessorName<FrxPanNode::ProcessorType>(),                 getCreator<FrxPanNode>(1, 1))
	(getProcessorName<FrxInStepNode::ProcessorType>(),           getCreator<FrxInStepNode>(2, 1))
	(getProcessorName<FrxOutStepNode::ProcessorType>(),         getCreator<FrxOutStepNode>(1, 2))
	(getProcessorName<FrxInSwitchNode::ProcessorType>(),       getCreator<FrxInSwitchNode>(2, 1))
	(getProcessorName<FrxOutSwitchNode::ProcessorType>(),     getCreator<FrxOutSwitchNode>(1, 2))
	(getProcessorName<FrxADSRNode::ProcessorType>(),               getCreator<FrxADSRNode>(0, 1))
	(getProcessorName<FrxPeakTrackerNode::ProcessorType>(), getCreator<FrxPeakTrackerNode>(0, 1));
}
//-----------------------------------------------------------------------------
FrxComponentFactory::FrxComponentFactory() {
	initMap();
}
//-----------------------------------------------------------------------------
FrxComponentFactory::ProcessorCreator 
FrxComponentFactory::getProcessorCreator(const std::string &name) const
{
	ProcessorMap::const_iterator it = processorMap.find(name);
	if (it==processorMap.end()) {
		SAMBAG_THROW(sambag::com::exceptions::IllegalStateException, 
			"tried get an unkown processor creator.");
	}
	return it->second;
}
//-----------------------------------------------------------------------------
FrxComponentFactory::PluginCreator FrxComponentFactory::getPluginCreator() const 
{
	return PluginCreator(&createPlugin);
}
//-----------------------------------------------------------------------------
void FrxComponentFactory::getProcessorNames(std::list<std::string> &out) const
{
	BOOST_FOREACH(const ProcessorMap::value_type &v, processorMap) {
		out.push_back(v.first);
	}
}
///////////////////////////////////////////////////////////////////////////////
IFrxComponentFactory & getComponentFactory(FrxCircuidViewPtr view) {
	typedef Loki::SingletonHolder<FrxComponentFactory> FactoryHolder;
	return FactoryHolder::Instance();
}
}}} // namespace(s)
