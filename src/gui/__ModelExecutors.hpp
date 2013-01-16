/*
 * __ModelExecutors.hpp
 *
 *  Created on: Sat Oct  6 14:20:49 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAGMODELEXEC_H
#define SAMBAGMODELEXEC_H

#include <processing/IModelController.hpp>
#include <processing/IProcessor.hpp>
#include "components/FrxConcreteProcessor.hpp"
#include "components/FrxConcreteConnections.hpp"
#include <gui/HandyNamespaces.hpp>

namespace frx { namespace gui {
using namespace components;
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
template <class ConcreteProcessor>
fp::IProcessor::Ptr 
createProcessorOnModel(fp::IModelController::Ptr ctrl, size_t numInputs, size_t numOutputs) 
{
	return fp::IProcessor::Ptr();
}
//-----------------------------------------------------------------------------
template <>
fp::IProcessor::Ptr 
inline createProcessorOnModel<FrxVolumeNode>(fp::IModelController::Ptr ctrl, 
						 size_t numInputs, size_t numOutputs) 
{
	return ctrl->createVolumeProcessor();
}
//-----------------------------------------------------------------------------
template <>
fp::IProcessor::Ptr 
inline createProcessorOnModel<FrxPanNode>(fp::IModelController::Ptr ctrl, 
						 size_t numInputs, size_t numOutputs) 
{
	return ctrl->createPanProcessor();
}
//-----------------------------------------------------------------------------
template <>
fp::IProcessor::Ptr 
inline createProcessorOnModel<FrxInStepNode>(fp::IModelController::Ptr ctrl, 
						 size_t numInputs, size_t numOutputs) 
{
	return ctrl->createInStepProcessor(numInputs);
}
//-----------------------------------------------------------------------------
template <>
fp::IProcessor::Ptr 
inline createProcessorOnModel<FrxOutStepNode>(fp::IModelController::Ptr ctrl, 
						 size_t numInputs, size_t numOutputs) 
{
	return ctrl->createOutStepProcessor(numOutputs);
}
//-----------------------------------------------------------------------------
template <>
fp::IProcessor::Ptr 
inline createProcessorOnModel<FrxInSwitchNode>(fp::IModelController::Ptr ctrl, 
						 size_t numInputs, size_t numOutputs) 
{
	return ctrl->createInSwitchProcessor(numInputs);
}
//-----------------------------------------------------------------------------
template <>
fp::IProcessor::Ptr 
inline createProcessorOnModel<FrxOutSwitchNode>(fp::IModelController::Ptr ctrl, 
						 size_t numInputs, size_t numOutputs) 
{
	return ctrl->createOutSwitchProcessor(numOutputs);
}
//-----------------------------------------------------------------------------
template <>
fp::IProcessor::Ptr 
inline createProcessorOnModel<FrxADSRNode>(fp::IModelController::Ptr ctrl, 
						 size_t numInputs, size_t numOutputs) 
{
	return ctrl->createADSRTransformer();
}
//-----------------------------------------------------------------------------
template <>
fp::IProcessor::Ptr 
inline createProcessorOnModel<FrxPeakTrackerNode>(fp::IModelController::Ptr ctrl, 
						 size_t numInputs, size_t numOutputs) 
{
	return ctrl->createPeakTracker();
}
//-----------------------------------------------------------------------------
template <>
fp::IProcessor::Ptr 
inline createProcessorOnModel<FrxMIDIReceiver>(fp::IModelController::Ptr ctrl, 
						 size_t numInputs, size_t numOutputs) 
{
	return ctrl->createMIDIReceiver();
}
///////////////////////////////////////////////////////////////////////////////
template <class ConnectionType>
fp::IConnection::Ptr 
connectModelObjects(fp::IModelController::Ptr ctrl, 
	fp::ModelObject::Ptr src,
	fp::ModelObject::Ptr dst) 
{
	return fp::IConnection::Ptr();
}
//-----------------------------------------------------------------------------
template <>
fp::IConnection::Ptr 
inline connectModelObjects<IOCn>(fp::IModelController::Ptr ctrl, 
	fp::ModelObject::Ptr src,
	fp::ModelObject::Ptr dst) 
{
	fp::INode::Ptr nsrc = boost::shared_dynamic_cast<fp::INode>(src);
	fp::INode::Ptr ndst = boost::shared_dynamic_cast<fp::INode>(dst);
	SAMBAG_ASSERT(nsrc && ndst);
	return ctrl->connect(nsrc, ndst);
}
//-----------------------------------------------------------------------------
template <>
fp::IConnection::Ptr 
inline connectModelObjects<ParameterCn>(fp::IModelController::Ptr ctrl, 
	fp::ModelObject::Ptr src,
	fp::ModelObject::Ptr dst) 
{
	fp::IParameter::Ptr nsrc = boost::shared_dynamic_cast<fp::IParameter>(src);
	fp::IParameter::Ptr ndst = boost::shared_dynamic_cast<fp::IParameter>(dst);
	SAMBAG_ASSERT(nsrc && ndst);
	return ctrl->connect(nsrc, ndst);
}
}} // namespace(s)

#endif /* SAMBAGMODELEXEC_H */
