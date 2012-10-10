/*
 * __ModelExecutors.hpp
 *
 *  Created on: Sat Oct  6 14:20:49 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAGMODELEXEC_H
#define SAMBAGMODELEXEC_H

#include <processing/IModelController.hpp>
#include "components/FrxConcreteProcessor.hpp"
namespace frx { namespace gui {
namespace fp = frx::processing;
using namespace components;
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
template <class ConcreteProcessor>
fp::IProcessor::Ptr 
createProcessorOnModel(fp::IModelController::Ptr ctrl, size_t numInputs, size_t numOutputs) 
{
	return IProcessor::Ptr();
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
}} // namespace(s)

#endif /* SAMBAGMODELEXEC_H */
