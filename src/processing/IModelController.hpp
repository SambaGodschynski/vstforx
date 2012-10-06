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
#include <gui/ViewObject.hpp>
#include <vector>
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
	virtual bool connect(IOutput::Ptr out, IInput::Ptr in) = 0;
	//-------------------------------------------------------------------------
	virtual bool removeConnection(IOutput::Ptr out, IInput::Ptr in) = 0;
	//-------------------------------------------------------------------------
	virtual bool remove(ModelObject::Ptr obj) = 0;
}; // IModelController
///////////////////////////////////////////////////////////////////////////////
extern IModelController *
getModelController(frx::gui::components::FrxCircuidViewPtr view);
}} // namespace(s)

#endif /* SAMBAG_IMODELCONTROLLER_H */
