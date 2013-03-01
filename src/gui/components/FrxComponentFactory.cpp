/*
 * FrxComponentFactory.cpp
 *
 *  Created on: Thu Nov  1 10:17:13 2012
 *      Author: Johannes Unger
 */

#include "FrxComponentFactory.hpp"
#include <gui/IFrxControl.hpp>
#include "FrxCircuidView.hpp"
#include "FrxConcreteProcessor.hpp"
#include "FrxConcreteParameter.hpp"
#include <gui/__ModelExecutors.hpp>
#include <boost/assign/list_of.hpp>
#include <gui/components/FrxFlag.hpp>
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
			"tried to create processor with FrxCircuidViewPtr == NULL");
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
	// flag
	/*FrxFlag::Ptr flag = FrxFlag::create();
	flag->setTarget(viewObj);
	circ->add(flag, FrxCircuidView::Z_Flags, true);*/
	// register
	registerProcessor(map, viewObj, mObj);
	return viewObj;
}
//-----------------------------------------------------------------------------
FrxProcessorNodePtr createPlugin(FrxCircuidViewPtr circ, ::processing::PluginInfo pI) 
{
	if (!circ) {
		SAMBAG_THROW(sambag::com::exceptions::IllegalStateException, 
			"tried to create processor with FrxCircuidViewPtr == NULL");
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
	// flag
	FrxFlag::Ptr flag = FrxFlag::create();
	flag->setTarget(viewObj);
	circ->add(flag, FrxCircuidView::Z_Flags, true);
	frx::processing::IPluginAdapter::Ptr plAd = 
		boost::shared_dynamic_cast<frx::processing::IPluginAdapter>(mObj);
	if (plAd) {
		viewObj->setName(plAd->getName());
		viewObj->setUpperFlagText(plAd->getName());
		viewObj->setLowerFlagText(plAd->getStatusMessage());
		viewObj->isSynth( plAd->isSynth() );
	}
	viewObj->configIO(mObj->getNumInputs(), mObj->getNumOutputs());
	registerProcessor(map, viewObj, mObj);
	return viewObj;
}
//-----------------------------------------------------------------------------
FrxParameterPtr createFreeParameter(FrxCircuidViewPtr circ) {
	if (!circ) {
		SAMBAG_THROW(sambag::com::exceptions::IllegalStateException, 
			"tried to create parameter with FrxCircuidViewPtr == NULL");
	}
	// create model obj.
	frx::processing::IModelController::Ptr ctrl;
	IViewModelMap::Ptr map;
	boost::tie(ctrl, map) = getControllerAndMap(circ);

	frx::processing::IParameter::Ptr mObj = ctrl->createFreeParameter();
	if (!mObj) {
		SAMBAG_THROW(sambag::com::exceptions::IllegalStateException, 
			"could'nt create parameter object.");
	}
	// create view obj.
	FrxStdKnob::Ptr viewObj = FrxStdKnob::create();
	if (!viewObj) {
		return FrxParameterPtr();
	}
	// flag
	FrxFlag::Ptr flag = FrxFlag::create();
	flag->setTarget(viewObj);
	circ->add(flag, FrxCircuidView::Z_Flags, true);
	viewObj->setUpperFlagText(mObj->getName());
	viewObj->setLowerFlagText(mObj->getDisplay());
	viewObj->setName( mObj->getName() );
	map->registerObjects(viewObj, mObj);
	return viewObj;
}
//-----------------------------------------------------------------------------
FrxParameterPtr createHostParameter(FrxCircuidViewPtr circ, int id) {
	if (!circ) {
		SAMBAG_THROW(sambag::com::exceptions::IllegalStateException, 
			"tried to create parameter with FrxCircuidViewPtr == NULL");
	}
	// create model obj.
	frx::processing::IModelController::Ptr ctrl;
	IViewModelMap::Ptr map;
	boost::tie(ctrl, map) = getControllerAndMap(circ);

	frx::processing::IParameter::Ptr mObj = ctrl->getHostParameter(id);
	if (!mObj) {
		SAMBAG_THROW(sambag::com::exceptions::IllegalStateException, 
			"could'nt create parameter object.");
	}
	// create view obj.
	FrxStdKnob::Ptr viewObj = FrxStdKnob::create();
	if (!viewObj) {
		return FrxParameterPtr();
	}
	// flag
	FrxFlag::Ptr flag = FrxFlag::create();
	flag->setTarget(viewObj);
	circ->add(flag, FrxCircuidView::Z_Flags, true);
	viewObj->setUpperFlagText(mObj->getName());
	viewObj->setLowerFlagText(mObj->getDisplay());
	viewObj->setName( mObj->getName() );
	if (!map->registerObjects(viewObj, mObj)) {
		return FrxParameterPtr();
	}
	return viewObj;
}
//-----------------------------------------------------------------------------
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
	(getProcessorName<FrxADSRNode::ProcessorType>(),               getCreator<FrxADSRNode>(1, 0))
	(getProcessorName<FrxPeakTrackerNode::ProcessorType>(), getCreator<FrxPeakTrackerNode>(1, 0))
	(getProcessorName<FrxMIDIReceiver::ProcessorType>(), getCreator<FrxMIDIReceiver>(0, 0));
}//-----------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------
FrxComponentFactory::FreeParameterCreator 
FrxComponentFactory::getFreeParameterCreator() const 
{
	return FreeParameterCreator(&createFreeParameter);
}
//-----------------------------------------------------------------------------
FrxComponentFactory::HostParameterCreator 
FrxComponentFactory::getHostParameterCreator() const 
{
	return HostParameterCreator(&createHostParameter);
}
///////////////////////////////////////////////////////////////////////////////
IFrxComponentFactory & getComponentFactory(FrxCircuidViewPtr view) {
	typedef Loki::SingletonHolder<FrxComponentFactory> FactoryHolder;
	return FactoryHolder::Instance();
}
}}} // namespace(s)
