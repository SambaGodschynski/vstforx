/*
 * BridgedPlugin.cpp
 *
 *  Created on: Mon Jan  6 11:53:25 2014
 *      Author: Johannes Unger
 */

#include "BridgedPlugin.hpp"

#if 0

namespace frx { namespace processing {
//=============================================================================
//  Class BridgedPlugin
//=============================================================================
//-----------------------------------------------------------------------------
void BridgedPlugin::createSession() {
}
//-----------------------------------------------------------------------------
void BridgedPlugin::restoreSession() {
}
//-----------------------------------------------------------------------------
void BridgedPlugin::initListener() {
}
//-----------------------------------------------------------------------------
BridgedPlugin::BridgedPlugin(IHostInfo::Ptr hostInfo,
    const std::string &location) : Plugin(hostInfo, location)
{
    createSession();
}
//-----------------------------------------------------------------------------
BridgedPlugin::Ptr BridgedPlugin::create(IHostInfo::Ptr hI, const std::string &location) {
  /*  Ptr res = Ptr( new BridgedPlugin(hI, location) );
    res->self = res;
    return res;*/
}
//-----------------------------------------------------------------------------
size_t BridgedPlugin::getNumPrograms() {
    return 0;
}
//-----------------------------------------------------------------------------
std::string BridgedPlugin::getProgramName(size_t index ) {
    return "";
}
//-----------------------------------------------------------------------------
void BridgedPlugin::setProgram(size_t index ) {
}
//-----------------------------------------------------------------------------
int BridgedPlugin::getProgram() {
    return 0;
}
//-----------------------------------------------------------------------------
bool BridgedPlugin::isAccessable() {
    return false;
}
//-----------------------------------------------------------------------------
bool BridgedPlugin::canHandleMidiEvent() const {
    return false;
}
//-----------------------------------------------------------------------------
void BridgedPlugin::processAdapter( oldPr::Processor::Int numSamples ) {
}
//-----------------------------------------------------------------------------
void BridgedPlugin::processMidiEvents( sambag::dsp::IMidiEvents * events ) {
}
//-----------------------------------------------------------------------------
BridgedPlugin::~BridgedPlugin() {
}
//-----------------------------------------------------------------------------
bool BridgedPlugin::hasEditor() const {
    return false;
}
//-----------------------------------------------------------------------------
void BridgedPlugin::openEditor(void *window) {
}
//-----------------------------------------------------------------------------
void BridgedPlugin::closeEditor(void *window) {
}
//-----------------------------------------------------------------------------
void BridgedPlugin::onEditorIdle() {
}
//-----------------------------------------------------------------------------
size_t BridgedPlugin::getNumInputChannels() const {
    return 0;
}
//-----------------------------------------------------------------------------
size_t BridgedPlugin::getNumOutputChannels() const {
    return 0;
}
//-----------------------------------------------------------------------------
::processing::parameter::ParameterPtr
BridgedPlugin::getParameter ( size_t nr ) const
{
    return ::processing::parameter::ParameterPtr();
}
//-----------------------------------------------------------------------------
size_t BridgedPlugin::getNumParameter () const {
    return 0;
}
}} // namespace(s)

#endif
