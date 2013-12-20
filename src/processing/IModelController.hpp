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
     * @param the id of the processor to create
     * @param whether the first output should be connected to the exit
     * (for processors which have a invisble output e.g. Peaktracker etc.)
	 * @return ModelObject fitting to id.
	 */
	virtual IProcessor::Ptr createProcessor(const std::string &id,
        bool autoConnectOutput = false) = 0;
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
    //-------------------------------------------------------------------------
    /**
     * @return the graph delay in samples
     */
     virtual int getGraphDelay() const = 0;
    //-------------------------------------------------------------------------
    /**
     * graph needs to be updated. e.g. after processors property changed
     * (process delay)
     */
    virtual void updateGraph() = 0;
}; // IModelController
///////////////////////////////////////////////////////////////////////////////
extern IModelController::Ptr
getModelController(frx::gui::components::FrxCircuidViewPtr view);
}} // namespace(s)

#endif /* SAMBAG_IMODELCONTROLLER_H */
