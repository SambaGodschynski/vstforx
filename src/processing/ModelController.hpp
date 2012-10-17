/*
 * ModelController.hpp
 *
 *  Created on: Wed Oct 10 12:06:47 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_MODELCONTROLLER_H
#define SAMBAG_MODELCONTROLLER_H

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include "IModelController.hpp"
#include "graph.h"
#include <boost/function.hpp>

namespace frx { namespace processing {
//=============================================================================
/** 
  * @class ModelController.
  */
class ModelController : public IModelController {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<ModelController> Ptr;
	//-------------------------------------------------------------------------
	typedef boost::weak_ptr<ModelController> WPtr;
protected:
	//-------------------------------------------------------------------------
	WPtr self;
	//-------------------------------------------------------------------------
	ModelController() {}
private:
	//-------------------------------------------------------------------------
	::processing::Graph::Ptr graph;
	//-------------------------------------------------------------------------
	void installListeners(IProcessor::Ptr pr);
public:
	//-------------------------------------------------------------------------
	static Ptr create();
	//-------------------------------------------------------------------------
	void setGraph(::processing::Graph::Ptr graph);
	//-------------------------------------------------------------------------
	::processing::Graph::Ptr getGraph() const;
	///////////////////////////////////////////////////////////////////////////
	// IModelControllerImpl.
	//-------------------------------------------------------------------------
	/**
	 * @return ModelObject pointer which points on a VolumeProcessor object
	 */
	virtual IProcessor::Ptr createVolumeProcessor();
	//-------------------------------------------------------------------------
	/**
	 * @return ModelObject pointer which points on a PanProcessor object
	 */
	virtual IProcessor::Ptr createPanProcessor();
	//-------------------------------------------------------------------------
	/**
	 * @param number of inputs
	 * @param out-container with i/o objects
	 * @return ModelObject pointer which points on a InStepProcessor object
	 */
	virtual IProcessor::Ptr 
	createInStepProcessor(size_t numInputs);
	//-------------------------------------------------------------------------
	/**
	 * @param number of outputs
	 * @param out-container with i/o objects
	 * @return ModelObject pointer which points on a InStepProcessor object
	 */
	virtual IProcessor::Ptr 
	createOutStepProcessor(size_t numOutputs);
	//-------------------------------------------------------------------------
	/**
	 * @param number of inputs
	 * @param out-container with i/o objects
	 * @return ModelObject pointer which points on a InStepProcessor object
	 */
	virtual IProcessor::Ptr 
	createInSwitchProcessor(size_t numInputs);
	//-------------------------------------------------------------------------
	/**
	 * @param number of outputs
	 * @param out-container with i/o objects
	 * @return ModelObject pointer which points on a InStepProcessor object
	 */
	virtual IProcessor::Ptr 
	createOutSwitchProcessor(size_t numOutputs);
	//-------------------------------------------------------------------------
	/**
	 * @return ModelObject pointer which points on a PeakTracker object
	 */
	virtual IProcessor::Ptr createPeakTracker();
	//-------------------------------------------------------------------------
	/**
	 * @return ModelObject pointer which points on a PeakTracker object
	 */
	virtual IProcessor::Ptr createADSRTransformer();
	//-------------------------------------------------------------------------
	virtual IConnection::Ptr connect(INode::Ptr out, INode::Ptr in);
	//-------------------------------------------------------------------------
	virtual bool removeConnection(IConnection::Ptr cn);
	//-------------------------------------------------------------------------
	virtual bool removeProcessor(IProcessor::Ptr pr);
	//-------------------------------------------------------------------------
	virtual INode::Ptr getEntry();
	//-------------------------------------------------------------------------
	virtual INode::Ptr getExit();
	///////////////////////////////////////////////////////////////////////////
	// specific impl.
	//-------------------------------------------------------------------------
	bool excuteConnectionRemoveRequest(ModelObject::Ptr obj, 
		boost::weak_ptr<IConnection> cn);
	//-------------------------------------------------------------------------
	bool excuteProcessorRemoveRequest(ModelObject::Ptr obj, 
		boost::weak_ptr<IProcessor> cn);
}; // ModelController
}} // namespace(s)

#endif /* SAMBAG_MODELCONTROLLER_H */
