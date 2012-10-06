/*
 * IProcessor.hpp
 *
 *  Created on: Sat Oct  6 13:43:59 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_IPROCESSOR_H
#define SAMBAG_IPROCESSOR_H

#include <boost/shared_ptr.hpp>
#include "ModelObject.hpp"
#include "IInput.hpp"
#include "IOutput.hpp"

namespace frx { namespace processing {
//=============================================================================
/** 
  * @class IProcessor.
  */
class IProcessor : public ModelObject {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<IProcessor> Ptr;
	//-------------------------------------------------------------------------
	virtual size_t getNumInputs() const = 0;
	//-------------------------------------------------------------------------
	virtual size_t getNumOutputs() const = 0;
	//-------------------------------------------------------------------------
	virtual IInput::Ptr getInput(size_t nr) const = 0;
	//-------------------------------------------------------------------------
	virtual IOutput::Ptr getOutput(size_t nr) const = 0;
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
	virtual IOutput::Ptr addOutput() = 0;
	//-------------------------------------------------------------------------
	/**
	 * Creates input and adds to processor.
	 * @return created input or null when failed.
	 */
	virtual IOutput::Ptr addInput() = 0;
}; // IProcessor
}} // namespace(s)

#endif /* SAMBAG_IPROCESSOR_H */
