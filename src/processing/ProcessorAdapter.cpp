/*
 * ProcessorAdapter.cpp
 *
 *  Created on: Thu Oct 18 13:32:15 2012
 *      Author: Johannes Unger
 */

#include "ProcessorAdapter.hpp"
#include "NodeAdapter.hpp"
#include "ParameterAdapter.hpp"
#include <boost/unordered_map.hpp>
#include <boost/foreach.hpp>
namespace frx { namespace processing {
//=============================================================================
//  Class ProcessorAdapter
//=============================================================================
//-----------------------------------------------------------------------------
void ProcessorAdapter::setAdaptee(ProcessorAdapter::Adaptee::Ptr p) {
	processor = p;	
}
//-----------------------------------------------------------------------------
ProcessorAdapter::Adaptee::Ptr ProcessorAdapter::getAdaptee() const {
	return processor;
}
//-----------------------------------------------------------------------------
size_t ProcessorAdapter::getNumInputs() const {
	SAMBAG_ASSERT(processor);
	return processor->getNumInputNodes();
}
//-----------------------------------------------------------------------------
size_t ProcessorAdapter::getNumOutputs() const {
	SAMBAG_ASSERT(processor);
	return processor->getNumOutputNodes();
}
//-----------------------------------------------------------------------------
INode::Ptr ProcessorAdapter::getInput(size_t nr) const {
	SAMBAG_ASSERT(processor);
	size_t num = getNumInputs();
	if (inputs.size() != num) {
		inputs.resize(num);
	}
	if (!inputs[nr]) {
		::processing::ProcessorNode::Ptr n = processor->getInputNode(nr);
		inputs[nr] = NodeAdapter::create(n);
	}
	return inputs[nr];
}
//-----------------------------------------------------------------------------
INode::Ptr ProcessorAdapter::getOutput(size_t nr) const {
	SAMBAG_ASSERT(processor);
	size_t num = getNumOutputs();
	if (outputs.size() != num) {
		outputs.resize(num);
	}
	if (!outputs[nr]) {
		::processing::ProcessorNode::Ptr n = processor->getOutputNode(nr);
		outputs[nr] = NodeAdapter::create(n);
	}
	return outputs[nr];
}
//-----------------------------------------------------------------------------
bool ProcessorAdapter::hasMultipleInputs() const {
	return false;
}
//-----------------------------------------------------------------------------
bool ProcessorAdapter::hasMultipleOutputs() const {
	return false;
}
//-----------------------------------------------------------------------------
INode::Ptr ProcessorAdapter::addOutput() {
	return INode::Ptr();
}
//-----------------------------------------------------------------------------
INode::Ptr ProcessorAdapter::addInput() {
	return INode::Ptr();
}
//-----------------------------------------------------------------------------
size_t ProcessorAdapter::getNumParameter() const {
	using ::processing::parameter::HasParameter;
	HasParameter::Ptr hp =
		boost::shared_dynamic_cast<HasParameter>(processor);
	if (!hp)
		return 0;
	return hp->getNumParameter();
}
//-----------------------------------------------------------------------------
IParameter::Ptr ProcessorAdapter::getParameter(int nr) const {
	using ::processing::parameter::HasParameter;
	HasParameter::Ptr hp =
		boost::shared_dynamic_cast<HasParameter>(processor);
	if (!hp)
		return IParameter::Ptr();
	size_t num = hp->getNumParameter();
	if (parameters.size() != num) {
		parameters.resize(num);
	}
	if (!parameters[nr]) {
		::processing::parameter::Parameter::Ptr p = hp->getParameter(nr);
		parameters[nr] = ParameterAdapter::create(p);
	}
	return parameters[nr];
}
//-----------------------------------------------------------------------------
bool ProcessorAdapter::requestRemove(ModelObject::Ptr obj) {
	bool res = true;
	BOOST_FOREACH(ModelObject::Ptr m, inputs) {
		res &= m->requestRemove(m);
	}
	BOOST_FOREACH(ModelObject::Ptr m, outputs) {
		res &= m->requestRemove(m);
	}
	BOOST_FOREACH(ModelObject::Ptr m, parameters) {
		res &= m->requestRemove(m);
	}
	return res && Super::requestRemove(obj);
}
}} // namespace(s)
