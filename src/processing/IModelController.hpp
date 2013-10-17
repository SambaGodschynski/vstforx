/*
 * IModelController.hpp
 *
 *  Created on: Fri Oct  5 13:40:06 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_IMODELCONTROLLER_H
#define SAMBAG_IMODELCONTROLLER_H

#include <boost/shared_ptr.hpp>
#include <gui/components/Forward.hpp>
#include "ModelObject.hpp"
#include "IProcessor.hpp"
#include "IParameter.hpp"
#include "IConnection.hpp"
#include "IPluginAdapter.hpp"
#include "INode.hpp"
#include <gui/ViewObject.hpp>
#include <vector>
#include "IHostInfo.h"
#include "PlugInfo.h"

namespace frx { namespace processing {
//=============================================================================
/** 
  * @class IModelController.
  */
class IModelController {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<IModelController> Ptr;
	//-------------------------------------------------------------------------
	/**
	 * @return ModelObject pointer which points on a VolumeProcessor object
	 */
	virtual IProcessor::Ptr createVolumeProcessor() = 0;
	//-------------------------------------------------------------------------
	/**
	 * @return ModelObject pointer which points on a PanProcessor object
	 */
	virtual IProcessor::Ptr createPanProcessor() = 0;
	//-------------------------------------------------------------------------
	virtual IProcessor::Ptr createPlugin(const ::processing::PluginInfo &pI) = 0;
	//-------------------------------------------------------------------------
	/**
	 * @param number of inputs
	 * @param out-container with i/o objects
	 * @return ModelObject pointer which points on a InStepProcessor object
	 */
	virtual IProcessor::Ptr 
	createInStepProcessor(size_t numInputs) = 0;
	//-------------------------------------------------------------------------
	/**
	 * @param number of outputs
	 * @param out-container with i/o objects
	 * @return ModelObject pointer which points on a InStepProcessor object
	 */
	virtual IProcessor::Ptr 
	createOutStepProcessor(size_t numOutputs) = 0;
	//-------------------------------------------------------------------------
	/**
	 * @param number of inputs
	 * @param out-container with i/o objects
	 * @return ModelObject pointer which points on a InStepProcessor object
	 */
	virtual IProcessor::Ptr 
	createInSwitchProcessor(size_t numInputs) = 0;
	//-------------------------------------------------------------------------
	/**
	 * @param number of outputs
	 * @param out-container with i/o objects
	 * @return ModelObject pointer which points on a InStepProcessor object
	 */
	virtual IProcessor::Ptr 
	createOutSwitchProcessor(size_t numOutputs) = 0;
	//-------------------------------------------------------------------------
	/**
	 * @return ModelObject pointer which points on a PeakTracker object
	 */
	virtual IProcessor::Ptr createPeakTracker() = 0;
	//-------------------------------------------------------------------------
	/**
	 * @return ModelObject pointer which points on a PeakTracker object
	 */
	virtual IProcessor::Ptr createADSRTransformer() = 0;
	//-------------------------------------------------------------------------
	/**
	 * @return ModelObject pointer which points on a MIDIReceiver object
	 */
	virtual IProcessor::Ptr createMIDIReceiver() = 0;
	//-------------------------------------------------------------------------
	virtual IProcessor::Ptr createRemoteChannelReceiver(const std::string &rcid) = 0;
	//-------------------------------------------------------------------------
	virtual IConnection::Ptr connect(INode::Ptr out, INode::Ptr in) = 0;
	//-------------------------------------------------------------------------
	virtual IConnection::Ptr connect(IParameter::Ptr a, IParameter::Ptr b) = 0;
	//-------------------------------------------------------------------------
	virtual bool removeConnection(IConnection::Ptr cn) = 0;
	//-------------------------------------------------------------------------
	virtual bool removeProcessor(IProcessor::Ptr pr) = 0;
	//-------------------------------------------------------------------------
	virtual INode::Ptr getEntry() = 0;
	//-------------------------------------------------------------------------
	virtual INode::Ptr getExit() = 0;
	//-------------------------------------------------------------------------
	virtual IHostInfo::Ptr getHostInfo() const = 0;
	//-------------------------------------------------------------------------
	virtual IParameter::Ptr createFreeParameter() = 0;
	//-------------------------------------------------------------------------
	virtual bool removeParameter(IParameter::Ptr p) = 0;
	//-------------------------------------------------------------------------
	virtual IParameter::Ptr getHostParameter(int id) = 0;
	//-------------------------------------------------------------------------
	virtual int getNumHostParameter() = 0;
	//-------------------------------------------------------------------------
	virtual INode::Ptr addInputTo(IProcessor::Ptr pr) = 0;
	//-------------------------------------------------------------------------
	virtual INode::Ptr addOutputTo(IProcessor::Ptr pr) = 0;
	//-------------------------------------------------------------------------
	virtual void getParameterCnOpTypeIds(ParameterCnOpTypeIds &out) const = 0;
	//-------------------------------------------------------------------------
	virtual void 
		addParameterCnOp(IConnection::Ptr cn, const ParameterCnOpTypeId &opId) = 0;
}; // IModelController
///////////////////////////////////////////////////////////////////////////////
extern IModelController::Ptr
getModelController(frx::gui::components::FrxCircuidViewPtr view);
}} // namespace(s)

#endif /* SAMBAG_IMODELCONTROLLER_H */
