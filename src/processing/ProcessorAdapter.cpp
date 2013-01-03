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
#include "MidiEventProcessor.h"
#include <algorithm>

namespace frx { namespace processing {
//=============================================================================
//  Class ProcessorAdapter
//=============================================================================
//-----------------------------------------------------------------------------
void ProcessorAdapter::setAdaptee(ProcessorAdapter::Adaptee::Ptr p) {
	processor = p;	
	initParameter();
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
	typedef ::processing::VariableInputAdapter VIA;
	VIA *va =
		dynamic_cast<VIA*>(getAdaptee().get());
	if (!va)
		return false;
	return true;
}
//-----------------------------------------------------------------------------
bool ProcessorAdapter::hasMultipleOutputs() const {
	typedef ::processing::VariableOutputAdapter VOA;
	VOA *va =
		dynamic_cast<VOA*>(getAdaptee().get());
	if (!va)
		return false;
	return true;
}
//-----------------------------------------------------------------------------
INode::Ptr ProcessorAdapter::addOutput() {
	typedef ::processing::VariableOutputAdapter VOA;
	VOA *va =
		dynamic_cast<VOA*>(getAdaptee().get());
	if (!va)
		return INode::Ptr();
	::processing::ProcessorNode::Ptr n = va->addOutputNode();
	if (!n)
		return INode::Ptr();
	NodeAdapter::Ptr res = NodeAdapter::create(n);
	outputs.push_back(res);
	updateParameter();
	// send event
	IOChangedEventSender::notifyListeners(this, IOChangedEvent(getPtr()));
	// return result
	return res;
}
//-----------------------------------------------------------------------------
INode::Ptr ProcessorAdapter::addInput() {
	typedef ::processing::VariableInputAdapter VIA;
	VIA *va =
		dynamic_cast<VIA*>(getAdaptee().get());
	if (!va)
		return INode::Ptr();
	::processing::ProcessorNode::Ptr n = va->addInputNode();
	if (!n)
		return INode::Ptr();
	NodeAdapter::Ptr res = NodeAdapter::create(n);
	inputs.push_back(res);
	updateParameter();
	// send event
	IOChangedEventSender::notifyListeners(this, IOChangedEvent(getPtr()));
	// return result
	return res;
}
//-----------------------------------------------------------------------------
void ProcessorAdapter::updateParameter() {
	// TODO: handles only the case processor has unregistered parameter
	// TODO: ignores the case processor parameters was removed
	// TODO: quite inefficient approach

	// determine the unregistered parameter
	// default parameter
	using ::processing::parameter::Parameter;
	using ::processing::parameter::HasParameter;
	using ::processing::MidiEventProcessor;
	HasParameter::Ptr hp =
		boost::shared_dynamic_cast<HasParameter>(processor);
	if (!hp) {
		return;
	}
	size_t num = hp->getNumParameter();
	size_t istNum = parameters.count(".");
	if ( num == istNum ) { // nothing changed
		return;
	}
	// create comparing sets
	typedef std::vector<::processing::parameter::Parameter::Ptr> Parameters;
	Parameters soll;
	soll.reserve(num);
	Parameters ist;
	ist.reserve(istNum);
	for (size_t i=0; i<num; ++i) {
		soll.push_back(hp->getParameter(i));
	}
	ParameterGroupMap::const_iterator it, end;
	boost::tie(it, end) = parameters.equal_range(".");
	for (; it!=end; ++it) {
		ParameterAdapter::Ptr ad = it->second;
		ist.push_back(ad->getAdaptee());
	}
	// sort comparing sets
	std::sort(soll.begin(), soll.end());
	std::sort(ist.begin(), ist.end());
	// compute differecne
	Parameters res(num);
	Parameters::const_iterator rend = 
		std::set_difference (soll.begin(), soll.end(), ist.begin(), ist.end(), res.begin());
	// save result
	for (Parameters::const_iterator it = res.begin(); it!=rend; ++it) {
		parameters.insert(std::make_pair(".", ParameterAdapter::create(*it)));
	}
	// we ignore output parameter and midi config.
}
//-----------------------------------------------------------------------------
void ProcessorAdapter::initParameter() {
	// default parameter
	using ::processing::parameter::Parameter;
	using ::processing::parameter::HasParameter;
	using ::processing::MidiEventProcessor;
	HasParameter::Ptr hp =
		boost::shared_dynamic_cast<HasParameter>(processor);
	if (hp) {
		size_t num = hp->getNumParameter();
		for (size_t i=0; i<num; ++i) {
			::processing::parameter::Parameter::Ptr p;
			p = hp->getParameter(i);
			parameters.insert(std::make_pair(".", ParameterAdapter::create(p)));
		}
	}
	// out parameter
	using ::processing::parameter::Parameter;
	using ::processing::parameter::HasParameter;
	using ::processing::parameter::HasOutParameter;
	using ::processing::MidiEventProcessor;
	HasOutParameter::Ptr hpo =
		boost::shared_dynamic_cast<HasOutParameter>(processor);
	if (hpo) {
		size_t num = hpo->getNumOutParameter();
		for (size_t i=0; i<num; ++i) {
			::processing::parameter::Parameter::Ptr p;
			p = hpo->getOutParameter(i);
			parameters.insert(std::make_pair("output parameter", ParameterAdapter::create(p)));
		}
	}
	// midi config parameter
	MidiEventProcessor* mevp = 
		dynamic_cast<MidiEventProcessor*>(processor.get());
	if (mevp) {
		Parameter::Ptr p = mevp->getMidiChannelParameter();
		parameters.insert(std::make_pair("midi config", ParameterAdapter::create(p)));
	}
}
//-----------------------------------------------------------------------------
void ProcessorAdapter::getParameterGroupKeys(ParameterGroupKeys &out) const {
	ParameterGroupMap::const_iterator it = parameters.begin();
	while (it!=parameters.end()) {
		const ParameterGroupKey &key = it->first;
		out.insert(key);
		it = parameters.upper_bound(key); // next key
	}
}
//-----------------------------------------------------------------------------
void ProcessorAdapter::
getParameters(const ParameterGroupKey &key, Parameters &out) const 
{
	if (key=="*") { // all parameter
		out.reserve(parameters.size());
		BOOST_FOREACH(const ParameterGroupMap::value_type &v, parameters) {
			out.push_back(v.second);
		}
		return;
	}
	ParameterGroupMap::const_iterator it, end;
	boost::tie(it, end) = parameters.equal_range(key);
	out.reserve(parameters.count(key));
	for (; it!=end; ++it) {
		out.push_back(it->second);
	}
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
	BOOST_FOREACH(const ParameterGroupMap::value_type &v, parameters) {
		res &= v.second->requestRemove(v.second);
	}
	return res && Super::requestRemove(obj);
}
//-----------------------------------------------------------------------------
ProcessorAdapter::IOChangedEventSender::Connection ProcessorAdapter::
addIOChangedListener(const IOChangedEventSender::EventFunction &f)
{
	return IOChangedEventSender::addEventListener(f);
}
//-----------------------------------------------------------------------------
ProcessorAdapter::IOChangedEventSender::Connection ProcessorAdapter::
addTrackedIOChangedListener(const IOChangedEventSender::EventFunction &f, 
		AnyWPtr holder)
{
	return IOChangedEventSender::addTrackedEventListener(f, holder);
}
}} // namespace(s)
