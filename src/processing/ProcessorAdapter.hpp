/*
 * ProcessorAdapter.hpp
 *
 *  Created on: Thu Oct 18 13:32:15 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_PROCESSORADAPTER_H
#define SAMBAG_PROCESSORADAPTER_H

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include "IProcessor.hpp"
#include <sambag/com/Exception.hpp>
#include <vector>
#include <map>
#include "processing.h"
#include "Forward.hpp"

namespace frx { namespace processing {
//=============================================================================
/** 
  * @class ProcessorAdapter.
  */
class ProcessorAdapter : 
	public IProcessor,
	public IProcessor::IOChangedEventSender
{
//=============================================================================
private:
	///////////////////////////////////////////////////////////////////////////
	// Archive:
	//-------------------------------------------------------------------------
	friend class boost::serialization::access;
	//-------------------------------------------------------------------------
	template <typename Archive> 
	void serialize(Archive &ar, const unsigned int version) {
		ar & boost::serialization::base_object<IProcessor> ( *this );
		ar & processor;
		ar & inputs;
		ar & outputs;
		ar & parameters;
	}
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<ProcessorAdapter> Ptr;
	//-------------------------------------------------------------------------
	typedef boost::weak_ptr<ProcessorAdapter> WPtr;
	//-------------------------------------------------------------------------
	typedef ::processing::ProcessAdapter Adaptee;
	//-------------------------------------------------------------------------
	typedef IProcessor Super;
protected:
	//-------------------------------------------------------------------------
	WPtr self;
	//-------------------------------------------------------------------------
	Adaptee::Ptr processor;
	//-------------------------------------------------------------------------
	ProcessorAdapter(){}
	//-------------------------------------------------------------------------
	mutable std::vector<NodeAdapterPtr> inputs; 
	//-------------------------------------------------------------------------
	mutable std::vector<NodeAdapterPtr> outputs; 
	//-------------------------------------------------------------------------
	typedef std::multimap<ParameterGroupKey, ParameterAdapterPtr> ParameterGroupMap;
	ParameterGroupMap parameters; 
	//-------------------------------------------------------------------------
	void initParameter();
	//-------------------------------------------------------------------------
public:
	//-------------------------------------------------------------------------
	virtual IProcessor::Ptr getPtr() const {
		return self.lock();
	}
	//-------------------------------------------------------------------------
	static Ptr create(Adaptee::Ptr a = Adaptee::Ptr()) {
		Ptr res(new ProcessorAdapter());
		res->setAdaptee(a);
		res->self = res;
		return res;
	}
	//-------------------------------------------------------------------------
	virtual void setAdaptee(Adaptee::Ptr p);
	//-------------------------------------------------------------------------
	virtual Adaptee::Ptr getAdaptee() const;
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
	virtual void getParameterGroupKeys(ParameterGroupKeys &out) const;
	//-------------------------------------------------------------------------
	/**
	 * @return parameter by group key. if key == "*" all parameter
	 * will be returned.
	 */
	virtual void 
	getParameters(const ParameterGroupKey &key, Parameters &out) const;
	//-------------------------------------------------------------------------
	virtual bool requestRemove(ModelObject::Ptr obj);
	///////////////////////////////////////////////////////////////////////////
	// Events
	//-------------------------------------------------------------------------
	virtual IOChangedEventSender::Connection 
	addIOChangedListener(const IOChangedEventSender::EventFunction &);
	//-------------------------------------------------------------------------
	virtual IOChangedEventSender::Connection 
	addTrackedIOChangedListener(const IOChangedEventSender::EventFunction &, 
		AnyWPtr holder);
protected:
private:
public:
}; // ProcessorAdapter
}} // namespace(s)

#endif /* SAMBAG_PROCESSORADAPTER_H */
