/*
 * IProcessor.hpp
 *
 *  Created on: Sat Oct  6 13:43:59 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_IPROCESSOR_H
#define SAMBAG_IPROCESSOR_H

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include "ModelObject.hpp"
#include "INode.hpp"
#include "IParameter.hpp"

namespace frx { namespace processing {
//=============================================================================
/** 
  * @class IProcessor.
  */
class IProcessor : public ModelObject {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef ModelObject Super;
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<IProcessor> Ptr;
	//-------------------------------------------------------------------------
	typedef boost::weak_ptr<IProcessor> WPtr;
	//-------------------------------------------------------------------------
	virtual size_t getNumInputs() const = 0;
	//-------------------------------------------------------------------------
	virtual size_t getNumOutputs() const = 0;
	//-------------------------------------------------------------------------
	virtual INode::Ptr getInput(size_t nr) const = 0;
	//-------------------------------------------------------------------------
	virtual INode::Ptr getOutput(size_t nr) const = 0;
	//-------------------------------------------------------------------------
	/**
	 * @return true if processor is able to add/remove input
	 */
	virtual bool hasMultipleInputs() const = 0;
	//-------------------------------------------------------------------------
	/**
	 * @return true if processor is able to add/remove output
	 */
	virtual bool hasMultipleOutputs() const = 0;
	//-------------------------------------------------------------------------
	/**
	 * Creates output and adds to processor.
	 * @return created output or null when failed.
	 */
	virtual INode::Ptr addOutput() = 0;
	//-------------------------------------------------------------------------
	/**
	 * Creates input and adds to processor.
	 * @return created input or null when failed.
	 */
	virtual INode::Ptr addInput() = 0;
	//-------------------------------------------------------------------------
	virtual size_t getNumParameter() const = 0;
	//-------------------------------------------------------------------------
	virtual IParameter::Ptr getParameter(int nr) const = 0;
}; // IProcessor
}} // namespace(s)

#endif /* SAMBAG_IPROCESSOR_H */
