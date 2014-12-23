/*
 * ============================================================================
 * VSTPlugin2x.cpp
 *  Author: Johannes Unger
 * ============================================================================
 */
#include "processing/processing.h"
#include "VST3xImpl.h"

namespace frx { namespace processing {
//-----------------------------------------------------------------------------
APluginImpl::Ptr createVST3xPluginImpl(IHostInfo::Ptr hI,
    APluginImpl::Parameters *parameters, const std::string &location)
{
    return VST3PluginImpl::create(hI, location, parameters);
}
//-----------------------------------------------------------------------------
VST3PluginImpl::Ptr VST3PluginImpl::create(IHostInfo::Ptr hI, const std::string &location,
        Parameters *parameters)
{
    Ptr res(new VST3PluginImpl(hI, location, parameters));
    return res;
}
//-------------------------------------------------------------------------
VST3PluginImpl::VST3PluginImpl(IHostInfo::Ptr hI,
	const std::string &location, Parameters *parameters)
	: APluginImpl(hI, location, parameters)
{
}
//-------------------------------------------------------------------------
void VST3PluginImpl::baseConfigChanged(){
}
//-------------------------------------------------------------------------
void VST3PluginImpl::turnOff(){
}
//-------------------------------------------------------------------------
void VST3PluginImpl::turnOn(){
}
//-------------------------------------------------------------------------
void VST3PluginImpl::openPlugin(){
}
//-------------------------------------------------------------------------
void VST3PluginImpl::closePlugin(){
}
//-------------------------------------------------------------------------
size_t VST3PluginImpl::getNumInputChannels() const {
	return 0;
}
//-------------------------------------------------------------------------
size_t VST3PluginImpl::getNumOutputChannels() const {
	return 0;
}
//-------------------------------------------------------------------------
/**
* @return true, wenn Plugin ueber Editor verfuegt.
*/
bool VST3PluginImpl::hasEditor() const {
	return false;
}
//-------------------------------------------------------------------------
void VST3PluginImpl::openEditor(sambag::disco::components::WindowPtr win) {
}
//-------------------------------------------------------------------------
void VST3PluginImpl::closeEditor(sambag::disco::components::WindowPtr win) {
}
//-------------------------------------------------------------------------
void VST3PluginImpl::onEditorIdle() {
}
//-------------------------------------------------------------------------
bool VST3PluginImpl::isAccessable() const {
	return false;
}
//-------------------------------------------------------------------------
/**
* @return Anzahl aller Plugin-Programme (aka. Presets)
*/
size_t VST3PluginImpl::getNumPrograms() {
	return 0;
}
//-------------------------------------------------------------------------
/**
* @param index
* @return Program-Name zu index.
*/
std::string VST3PluginImpl::getProgramName( size_t index ) {
	return "";
}
//-------------------------------------------------------------------------
/**
* Aktiviert Program zu index.
* @param index
*/
void VST3PluginImpl::setProgram( size_t index ) {
}
//-------------------------------------------------------------------------
/**
* @return index des akuell gesetzten Program, falls vorhanden. Andernfalls -1.
*/
int VST3PluginImpl::getProgram() {
	return -1;
}
//-------------------------------------------------------------------------
/**
* @return true, if plugin can handle MIDI events
*/
bool VST3PluginImpl::canHandleMidiEvent() const {
	return false;
}
//-------------------------------------------------------------------------
void VST3PluginImpl::processMidiEvents( sambag::dsp::IMidiEvents * events ) 
{
}
//-------------------------------------------------------------------------
size_t VST3PluginImpl::getInitialDelay() const {
	return 0;
}
//-------------------------------------------------------------------------
/**
* @note fills out name, isSynth, uid, vendor, type
*/
void VST3PluginImpl::updatePluginInfo (::processing::PluginInfo &inf) const {
}
//-------------------------------------------------------------------------
void VST3PluginImpl::processPlugin( oldPr::Frames::T **,
	oldPr::Frames::T **, size_t numSamples)
{
}
//-------------------------------------------------------------------------
VST3PluginImpl::~VST3PluginImpl() {
}
//-------------------------------------------------------------------------
std::pair<size_t, void*> VST3PluginImpl::getStateData() const {
	return std::make_pair(0, (void*)NULL);
}
//-------------------------------------------------------------------------
void VST3PluginImpl::setStateData(size_t size, void* data) {
}
}} // namespace



