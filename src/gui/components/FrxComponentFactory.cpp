/*
 * FrxComponentFactory.cpp
 *
 *  Created on: Thu Nov  1 10:17:13 2012
 *      Author: Johannes Unger
 */

#include <com/Settings.h>
#include <sambag/com/Exception.hpp>
#include <sambag/com/exceptions/IllegalStateException.hpp>
#include <processing/IModelController.hpp>
#include "FrxComponentFactory.hpp"
#include <gui/IFrxControl.hpp>
#include "FrxCircuidView.hpp"
#include "FrxConcreteProcessor.hpp"
#include "FrxConcreteParameter.hpp"
#include <boost/foreach.hpp>
#include <boost/assign/list_of.hpp>
#include <gui/components/FrxFlag.hpp>
#include <boost/bind.hpp>
#include <sambag/com/Exception.hpp>
#include <sambag/com/exceptions/IllegalStateException.hpp>
#include <boost/foreach.hpp>
#include <sambag/com/Exception.hpp>
#include <processing/IModelController.hpp>
#include <com/Settings.h>
#include <processing/interprocess/RemoteChannelManager.hpp>
#include <processing/ModelFactory.hpp>
#include <gui/ViewFactory.hpp>
#include <com/one4All.h>
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <processing/pluginTypes/PluginFactory.hpp>

namespace frx { namespace gui { namespace components {
namespace {
typedef boost::weak_ptr<void> AnyWPtr;
typedef boost::shared_ptr<void> AnyPtr;
//-----------------------------------------------------------------------------
/**
 * register components in ModelMap
 */
void registerComponent(IViewModelMap::Ptr map,
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
FrxProcessorNodePtr createProcessor(FrxCircuidViewPtr circ, std::string id)
{
	if (!circ) {
		SAMBAG_THROW(sambag::com::exceptions::IllegalStateException, 
			"tried to create processor with FrxCircuidViewPtr == NULL");
	}
    
    ViewFactory &vfac = ViewFactory::instance();
    
    com::IdParser pid = "frx.gui."+id;
    
	// create view obj
	FrxProcessorNode::Ptr viewObj = vfac.create(pid.toString());
	globAddProcessor(viewObj);
	if (!viewObj) {
		return FrxProcessorNodePtr();
	}
    
	// create model obj.
	frx::processing::IModelController::Ptr ctrl;
	IViewModelMap::Ptr map;
	boost::tie(ctrl, map) = getControllerAndMap(circ);
    
    bool invisibleOuts = pid.name() == "ADSRTrigger" ||
                       pid.name() == "PeakTracker";
    
	frx::processing::IProcessor::Ptr mObj = ctrl->createProcessor(
        pid.namespace_("processing").toString(),
        invisibleOuts
    );
	if (!mObj) {
		SAMBAG_THROW(sambag::com::exceptions::IllegalStateException, 
			"could'nt create processor object.");
	}
	// flag
	//FrxFlag::Ptr flag = FrxFlag::create();
	//flag->setTarget(viewObj);
	//circ->add(flag, FrxCircuidView::Z_Flags, true);
	// register
	
    viewObj->configIO(mObj->getNumInputs(),
        invisibleOuts ? 0 : mObj->getNumOutputs()
    );
    registerComponent(map, viewObj, mObj);
	return viewObj;
}
//-----------------------------------------------------------------------------
FrxProcessorNodePtr createPlugin(FrxCircuidViewPtr circ, ::processing::PluginInfo pI) 
{
	if (!circ) {
		SAMBAG_THROW(sambag::com::exceptions::IllegalStateException, 
			"tried to create processor with FrxCircuidViewPtr == NULL");
	}
    
    std::string id = pI.getFactoryId();
    // remove frx.processing
    boost::algorithm::erase_first(id, "frx.processing.");
    SAMBAG_LOG_TRACE<<id;
    FrxPluginNode::Ptr viewObj = boost::dynamic_pointer_cast<FrxPluginNode>(
        createProcessor(circ, id)
    );
    
	// create model obj.
	frx::processing::IProcessor::Ptr mObj =
        boost::dynamic_pointer_cast<frx::processing::IProcessor>(
            getViewModelMap(circ)->getModelObject(viewObj)
        );

    if (!mObj) {
		SAMBAG_THROW(sambag::com::exceptions::IllegalStateException, 
			"modelObject == NULL");
	}

    // flag
	FrxFlag::Ptr flag = FrxFlag::create();
	flag->setTarget(viewObj);
	circ->add(flag, FrxCircuidView::Z_Flags, true);
	frx::processing::IPluginAdapter::Ptr plAd = 
		boost::dynamic_pointer_cast<frx::processing::IPluginAdapter>(mObj);
	if (plAd) {
		viewObj->setName(plAd->getName());
		viewObj->setUpperFlagText(plAd->getName());
		viewObj->setLowerFlagText(mObj->getStatusMessage());
		viewObj->isSynth( plAd->isSynth() );
	}
	
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
FrxProcessorNodePtr createRemoteChannel(FrxCircuidViewPtr circ, std::string &rcId)
{
	if (!circ) {
		SAMBAG_THROW(sambag::com::exceptions::IllegalStateException, 
			"tried to create processor with FrxCircuidViewPtr == NULL");
	}
    
    std::string id = "internal.RemoteChReceiver";
    id+="('" + rcId + "')";
    
    FrxProcessorNode::Ptr viewObj = boost::dynamic_pointer_cast<FrxPluginNode>(
        createProcessor(circ, id)
    );
    
	// create model obj.
	frx::processing::IProcessor::Ptr mObj =
        boost::dynamic_pointer_cast<frx::processing::IProcessor>(
            getViewModelMap(circ)->getModelObject(viewObj)
        );

    if (!mObj) {
		SAMBAG_THROW(sambag::com::exceptions::IllegalStateException, 
			"modelObject == NULL");
	}

    // flag
	FrxFlag::Ptr flag = FrxFlag::create();
	flag->setTarget(viewObj);
	circ->add(flag, FrxCircuidView::Z_Flags, true);
	frx::processing::IPluginAdapter::Ptr plAd = 
		boost::dynamic_pointer_cast<frx::processing::IPluginAdapter>(mObj);
	if (plAd) {
		viewObj->setName(plAd->getName());
		viewObj->setUpperFlagText(rcId);
		viewObj->setLowerFlagText(mObj->getStatusMessage());
	}
	
    return viewObj;
    
}
} // namespace(s)
//=============================================================================
//  Class FrxComponentFactory
//=============================================================================
//-----------------------------------------------------------------------------
FrxComponentFactory::FrxComponentFactory() {
}
//-----------------------------------------------------------------------------
FrxComponentFactory::ProcessorCreator 
FrxComponentFactory::getProcessorCreator(const std::string &name) const
{
    return boost::bind(&createProcessor, _1, name);
}
//-----------------------------------------------------------------------------
FrxComponentFactory::PluginCreator FrxComponentFactory::getPluginCreator() const 
{
	return PluginCreator(&createPlugin);
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
//-----------------------------------------------------------------------------
FrxComponentFactory::ProcessorCreator
FrxComponentFactory::getRemoteChannelCreator(const std::string &rcId) const
{
    return ProcessorCreator(
        boost::bind(&createRemoteChannel, _1, std::string(rcId))
    );
}
//-----------------------------------------------------------------------------
void FrxComponentFactory::getComponentNames(std::list<std::string> &out,
    const std::string &filter) const
{
    typedef std::vector<std::string> Vector;
    // determine the intersection of registered model and view components
    // get data:
    Vector ms;
    processing::ModelFactory::instance().getRegisteredIds(ms, filter);
    Vector vs;
    ViewFactory::instance().getRegisteredIds(vs);
    // presort
    std::sort(ms.begin(), ms.end());
    std::sort(vs.begin(), vs.end());
    // intersect
    Vector tmp( std::max(ms.size(), vs.size()) );
    Vector::iterator end = std::set_intersection(ms.begin(), ms.end(),
        vs.begin(), vs.end(), tmp.begin());
    for ( Vector::const_iterator it = tmp.begin(); it!=end; ++it ) {
        out.push_back(*it);
    }
}
///////////////////////////////////////////////////////////////////////////////
IFrxComponentFactory & getComponentFactory(FrxCircuidViewPtr view) {
	typedef Loki::SingletonHolder<FrxComponentFactory> FactoryHolder;
	return FactoryHolder::Instance();
}
}}} // namespace(s)