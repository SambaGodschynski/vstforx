/*
 * ProcessorAdapter.hpp
 *
 *  Created on: Thu Oct 18 13:32:15 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_PROCESSORADAPTER_H
#define SAMBAG_PROCESSORADAPTER_H

#include <boost/shared_ptr.hpp>
#include "IProcessor.hpp"
#include <sambag/com/Exception.hpp>
#include <vector>
#include "processing.h"
#include "Forward.hpp"

namespace frx { namespace processing {
//=============================================================================
/** 
  * @class ProcessorAdapter.
  */
class ProcessorAdapter : public IProcessor {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<ProcessorAdapter> Ptr;
	//-------------------------------------------------------------------------
	typedef ::processing::ProcessAdapter Adaptee;
	//-------------------------------------------------------------------------
	typedef IProcessor Super;
protected:
	//-------------------------------------------------------------------------
	Adaptee::Ptr processor;
	//-------------------------------------------------------------------------
	ProcessorAdapter(){}
	//-------------------------------------------------------------------------
	mutable std::vector<NodeAdapterPtr> inputs; 
	//-------------------------------------------------------------------------
	mutable std::vector<NodeAdapterPtr> outputs; 
	//-------------------------------------------------------------------------
	mutable std::vector<ParameterAdapterPtr> parameters; 
public:
	//-------------------------------------------------------------------------
	static Ptr create(Adaptee::Ptr a = Adaptee::Ptr()) {
		Ptr res(new ProcessorAdapter());
		res->setAdaptee(a);
		return res;
	}
	//-------------------------------------------------------------------------
	void setAdaptee(Adaptee::Ptr p);
	//-------------------------------------------------------------------------
	Adaptee::Ptr getAdaptee() const;
	//-------------------------------------------------------------------------
	virtual size_t getNumInputs() const;
	//-------------------------------------------------------------------------
	virtual size_t getNumOutputs() const;
	//-------------------------------------------------------------------------
	virtual INode::Ptr getInput(size_t nr) const;
	//-------------------------------------------------------------------------
	virtual INode::Ptr getOutput(size_t nr) const;
	//-------------------------------------------------------------------------
	/**
	 * @return true if processor is able to add/remove input
	 */
	virtual bool hasMultipleInputs() const;
	//-------------------------------------------------------------------------
	/**
	 * @return true if processor is able to add/remove output
	 */
	virtual bool hasMultipleOutputs() const;
	//-------------------------------------------------------------------------
	/**
	 * Creates output and adds to processor.
	 * @return created output or null when failed.
	 */
	virtual INode::Ptr addOutput();
	//-------------------------------------------------------------------------
	/**
	 * Creates input and adds to processor.
	 * @return created input or null when failed.
	 */
	virtual INode::Ptr addInput();
	//-------------------------------------------------------------------------
	virtual size_t getNumParameter() const;
	//-------------------------------------------------------------------------
	virtual IParameter::Ptr getParameter(int nr) const;
	//-------------------------------------------------------------------------
	virtual bool requestRemove(ModelObject::Ptr obj);
protected:
private:
public:
}; // ProcessorAdapter
}} // namespace(s)

#endif /* SAMBAG_PROCESSORADAPTER_H */
