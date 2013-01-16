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
#include "Forward.hpp"
#include <vector>

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
	NodeAdapterPtr entry, exit;
	//-------------------------------------------------------------------------
	void installListeners(IProcessor::Ptr pr);
	//-------------------------------------------------------------------------
	void installListeners(IParameter::Ptr pr);
	//-------------------------------------------------------------------------
	bool removeConnection(NodeConnectionPtr cn);
	//-------------------------------------------------------------------------
	typedef std::vector<IParameter::Ptr> TmpHostParameter;
	//-------------------------------------------------------------------------
	TmpHostParameter tmpHostParameter;
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
	/**
	 * @return ModelObject pointer which points on a MIDIReceiver object
	 */
	virtual IProcessor::Ptr createMIDIReceiver();
	//-------------------------------------------------------------------------
	virtual IConnection::Ptr connect(INode::Ptr out, INode::Ptr in);
	//-------------------------------------------------------------------------
	virtual IConnection::Ptr connect(IParameter::Ptr a, IParameter::Ptr b);
	//-------------------------------------------------------------------------
	virtual bool removeConnection(IConnection::Ptr cn);
	//-------------------------------------------------------------------------
	virtual bool removeProcessor(IProcessor::Ptr pr);
	//-------------------------------------------------------------------------
	virtual INode::Ptr getEntry();
	//-------------------------------------------------------------------------
	virtual INode::Ptr getExit();
	//-------------------------------------------------------------------------
	virtual IProcessor::Ptr createPlugin(const ::processing::PluginInfo &pI);
	//-------------------------------------------------------------------------
	virtual IParameter::Ptr createFreeParameter();
	//-------------------------------------------------------------------------
	virtual IParameter::Ptr getHostParameter(int id);
	//-------------------------------------------------------------------------
	virtual bool removeFreeParameter(IParameter::Ptr p);
	//-------------------------------------------------------------------------
	virtual int getNumHostParameter();
	//-------------------------------------------------------------------------
	virtual IHostInfo::Ptr getHostInfo() const;
	//-------------------------------------------------------------------------
	virtual INode::Ptr addInputTo(IProcessor::Ptr pr);
	//-------------------------------------------------------------------------
	virtual INode::Ptr addOutputTo(IProcessor::Ptr pr);
	//-------------------------------------------------------------------------
	virtual void getParameterCnOpTypeIds(ParameterCnOpTypeIds &out) const;
	//-------------------------------------------------------------------------
	virtual void addParameterCnOp(IConnection::Ptr cn, const ParameterCnOpTypeId &opId);
	///////////////////////////////////////////////////////////////////////////
	// specific impl.
	//-------------------------------------------------------------------------
	bool excuteConnectionRemoveRequest(ModelObject::Ptr obj, 
		boost::weak_ptr<IConnection> cn);
	//-------------------------------------------------------------------------
	bool excuteProcessorRemoveRequest(ModelObject::Ptr obj, 
		boost::weak_ptr<IProcessor> cn);
	//-------------------------------------------------------------------------
	bool excuteParameterRemoveRequest(ModelObject::Ptr obj, 
		boost::weak_ptr<IParameter> cn);
}; // ModelController
}} // namespace(s)

#endif /* SAMBAG_MODELCONTROLLER_H */
