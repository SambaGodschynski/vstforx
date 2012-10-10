/*
 * ModelController.cpp
 *
 *  Created on: Wed Oct 10 12:06:47 2012
 *      Author: Johannes Unger
 */

#include "ModelController.hpp"

namespace frx { namespace processing {
//=============================================================================
//  Class ModelController
//=============================================================================
//-----------------------------------------------------------------------------
ModelController::Ptr ModelController::create() {
	Ptr res(new ModelController());
	return res;
}
//-----------------------------------------------------------------------------
void ModelController::setGraph(::processing::Graph::Ptr graph) {
	this->graph = graph;
}
//-----------------------------------------------------------------------------
::processing::Graph::Ptr ModelController::getGraph() const {
	return graph;
}
//-----------------------------------------------------------------------------
IProcessor::Ptr ModelController::createVolumeProcessor() {
	if (!graph)
		return IProcessor::Ptr();
	return IProcessor::Ptr();
}
//-----------------------------------------------------------------------------
IProcessor::Ptr ModelController::createPanProcessor() {
	if (!graph)
		return IProcessor::Ptr();
	return IProcessor::Ptr();
}
//-----------------------------------------------------------------------------
IProcessor::Ptr 
ModelController::createInStepProcessor(size_t numInputs) {
	if (!graph)
		return IProcessor::Ptr();
	return IProcessor::Ptr();
}
//-----------------------------------------------------------------------------
IProcessor::Ptr 
ModelController::createOutStepProcessor(size_t numOutputs) {
	if (!graph)
		return IProcessor::Ptr();
	return IProcessor::Ptr();
}
//-----------------------------------------------------------------------------
IProcessor::Ptr 
ModelController::createInSwitchProcessor(size_t numInputs) {
	if (!graph)
		return IProcessor::Ptr();
	return IProcessor::Ptr();
}
//-----------------------------------------------------------------------------
IProcessor::Ptr 
ModelController::createOutSwitchProcessor(size_t numOutputs) {
	if (!graph)
		return IProcessor::Ptr();
	return IProcessor::Ptr();
}
//-----------------------------------------------------------------------------
IProcessor::Ptr ModelController::createPeakTracker() {
	if (!graph)
		return IProcessor::Ptr();
	return IProcessor::Ptr();
}
//-----------------------------------------------------------------------------
IProcessor::Ptr ModelController::createADSRTransformer() {
	if (!graph)
		return IProcessor::Ptr();
	return IProcessor::Ptr();
}
//-----------------------------------------------------------------------------
IConnection::Ptr ModelController::connect(IOutput::Ptr out, IInput::Ptr in) {
	if (!graph)
		return IConnection::Ptr();
	return IConnection::Ptr();
}
//-----------------------------------------------------------------------------
bool ModelController::removeConnection(IConnection::Ptr cn) {
	if (!graph)
		return false;
	return false;
}
//-----------------------------------------------------------------------------
bool ModelController::remove(ModelObject::Ptr obj) {
	if (!graph)
		return false;
	return false;
}
//-----------------------------------------------------------------------------
IEntry::Ptr ModelController::getEntry() {
	if (!graph)
		return IEntry::Ptr();
	return IEntry::Ptr();
}
//-----------------------------------------------------------------------------
IExit::Ptr ModelController::getExit() {
	if (!graph)
		return IExit::Ptr();
	return IExit::Ptr();
}

}} // namespace(s)
