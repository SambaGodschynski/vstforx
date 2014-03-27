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
#include "IModelController.hpp"

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
	updateParameters();
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
	updateParameters();
	// send event
	IOChangedEventSender::notifyListeners(this, IOChangedEvent(getPtr()));
	// return result
	return res;
}
//-----------------------------------------------------------------------------
void ProcessorAdapter::updateParameter(::processing::parameter::Parameter::Ptr p)
{
	ParameterAdapterPtr pAd = getAdapter(p);
	if (!pAd) {
		return;
	}
	ParameterGroupMap::right_map::iterator it = parameters.right.find(pAd);
	if ( it==parameters.right.end() ) {
		parameters.insert( 
			ParameterGroupMap::value_type(p->getGroupName(), pAd) 
		);
		return;
	}
}
//-----------------------------------------------------------------------------
void ProcessorAdapter::updateParameters() {
	// TODO: handles only the case processor has unregistered parameter
	// TODO: ignores the case processor parameters was removed
	using ::processing::parameter::Parameter;
	using ::processing::parameter::HasParameter;
	using ::processing::MidiEventProcessor;
	HasParameter::Ptr hp =
		boost::dynamic_pointer_cast<HasParameter>(processor);
	if (!hp) {
		return;
	}
	size_t num = hp->getNumParameter();
	if ( num == parameters.left.count(".") ) { // nothing changed
		return;
	}
	for (size_t i=0; i<num; ++i) {
		updateParameter(hp->getParameter(i));
	}
}
//-----------------------------------------------------------------------------
ParameterAdapterPtr 
ProcessorAdapter::getAdapter(::processing::parameter::Parameter::Ptr p)
{
	ParameterAdapterMap::const_iterator it = parameterAdapterMap.find(p);
	if (it==parameterAdapterMap.end()) {
		ParameterAdapterPtr neu = ParameterAdapter::create(p);
		parameterAdapterMap.insert(ParameterAdapterMap::value_type(p, neu));
		return neu;
	}
	return it->second;
}
//-----------------------------------------------------------------------------
void ProcessorAdapter::initParameter() {
	// default parameter
	using ::processing::parameter::Parameter;
	using ::processing::parameter::HasParameter;
	using ::processing::MidiEventProcessor;
	HasParameter::Ptr hp =
		boost::dynamic_pointer_cast<HasParameter>(processor);
	if (hp) {
		size_t num = hp->getNumParameter();
		for (size_t i=0; i<num; ++i) {
			::processing::parameter::Parameter::Ptr p;
			p = hp->getParameter(i);
			ParameterGroupKey gName = p->getGroupName() == "" ? "." : p->getGroupName();
			parameters.insert(ParameterGroupMap::value_type(gName, getAdapter(p)));
		}
	}
	// out parameter
	using ::processing::parameter::Parameter;
	using ::processing::parameter::HasParameter;
	using ::processing::parameter::HasOutParameter;
	using ::processing::MidiEventProcessor;
	HasOutParameter::Ptr hpo =
		boost::dynamic_pointer_cast<HasOutParameter>(processor);
	if (hpo) {
		size_t num = hpo->getNumOutParameter();
		for (size_t i=0; i<num; ++i) {
			::processing::parameter::Parameter::Ptr p;
			p = hpo->getOutParameter(i);
			parameters.insert(ParameterGroupMap::value_type("output parameter", getAdapter(p)));
		}
	}
	// midi config parameter
	MidiEventProcessor* mevp = 
		dynamic_cast<MidiEventProcessor*>(processor.get());
	if (mevp) {
		Parameter::Ptr p = mevp->getMidiChannelParameter();
		parameters.insert(ParameterGroupMap::value_type("midi config", getAdapter(p)));
		p = mevp->getMidiSendParameter();
		parameters.insert(ParameterGroupMap::value_type("midi config", getAdapter(p)));
	}
}
//-----------------------------------------------------------------------------
void ProcessorAdapter::getParameterGroupKeys(ParameterGroupKeys &out) const {
	ParameterGroupMap::left_map::const_iterator it = parameters.left.begin();
	while (it!=parameters.left.end()) {
		const ParameterGroupKey &key = it->first;
		out.insert(key);
		it = parameters.left.upper_bound(key); // next key
	}
}
//-----------------------------------------------------------------------------
void ProcessorAdapter::
getParameters(const ParameterGroupKey &key, Parameters &out) const 
{
	if (key=="*") { // all parameter
		out.reserve(parameters.size());
		BOOST_FOREACH(const ParameterGroupMap::left_map::value_type &v, parameters.left) {
			out.push_back(v.second);
		}
		return;
	}
	ParameterGroupMap::left_map::const_iterator it, end;
	boost::tie(it, end) = parameters.left.equal_range(key);
	out.reserve(parameters.left.count(key));
	for (; it!=end; ++it) {
		out.push_back(it->second);
	}
}
//-----------------------------------------------------------------------------
bool ProcessorAdapter::requestRemove() {
	bool res = true;
	BOOST_FOREACH(ModelObject::Ptr m, inputs) {
		res &= m->requestRemove();
	}
	BOOST_FOREACH(ModelObject::Ptr m, outputs) {
		res &= m->requestRemove();
	}
	BOOST_FOREACH(const ParameterGroupMap::left_map::value_type &v, parameters.left) {
		res &= v.second->requestRemove();
	}
	return res && Super::requestRemove();
}
//-----------------------------------------------------------------------------
size_t ProcessorAdapter::getNumPresets() const {
	return 0;
}
//-----------------------------------------------------------------------------
std::string ProcessorAdapter::getPresetName(size_t i) const {
	return "";
}
//-----------------------------------------------------------------------------
void ProcessorAdapter::setPreset(int i) {
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
//-----------------------------------------------------------------------------
ProcessorAdapter::PropertyChangedSender::Connection
ProcessorAdapter::
addPropertyChangedListener(const PropertyChangedSender::EventFunction &f)
{
	namespace sce = sambag::com::events;
	return processor->sce::EventSender<PropertyChangedEvent>::addEventListener(
		f
	);
}
//-----------------------------------------------------------------------------
ProcessorAdapter::PropertyChangedSender::Connection
ProcessorAdapter::
addTrackedPropertyChangedListener(const PropertyChangedSender::EventFunction & f, 
	AnyWPtr holder)
{
	namespace sce = sambag::com::events;
	return processor->sce::EventSender<PropertyChangedEvent>::addTrackedEventListener(
		f, holder
	);
}
//-----------------------------------------------------------------------------
bool ProcessorAdapter::removeImpl(IModelControllerPtr ctrl) {
	return ctrl->removeProcessor(getPtr());
}
//-------------------------------------------------------------------------
std::string ProcessorAdapter::getStatusMessage() const {
    return getAdaptee()->getStatusMessage();
}

}} // namespace(s)
