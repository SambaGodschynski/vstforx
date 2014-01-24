/*
 * BridgedPlugin.cpp
 *
 *  Created on: Mon Jan  6 11:53:25 2014
 *  Author: Johannes Unger
 */

#include "BridgedPlugin.hpp"
#include <processing/interprocess/BridgeSessionManager.hpp>
#include <gui/components/interprocess/WindowSession.hpp>

namespace frx { namespace processing {
//=============================================================================
//  Class BridgedPlugin
//=============================================================================
//-----------------------------------------------------------------------------
BridgedPlugin::BridgedPlugin(IHostInfo::Ptr hI, const std::string &location,
        Parameters *parameters) :
        APluginImpl(hI, location, parameters)
{
    using namespace interprocess;
    session = BridgeSessionManager::instance().createPluginSession(location, hI);
    session->sce::EventSender<sce::PropertyChanged>::addEventListener(
        boost::bind(&BridgedPlugin::onPluginPropertyChanged, this, _1, _2)
    );
}
//-----------------------------------------------------------------------------
void BridgedPlugin::onPluginPropertyChanged(void*, const sce::PropertyChanged &ev)
{
    sce::EventSender<sce::PropertyChanged>::notifyListeners(this, ev);
}
//-----------------------------------------------------------------------------
void BridgedPlugin::parameterChanged(int index) {
    session->setParameterValues(parameters->at(index), index);
}
//-----------------------------------------------------------------------------
void BridgedPlugin::initParameters() {
    using ::processing::parameter::Parameter;
    int num = session->getNumParameter();
    if (num==0) {
        return;
    }
    parameters->resize(num);
    for (int i=0; i<num; ++i) {
        Parameter::Ptr p = parameters->at(i);
        if (!p) {
            (*parameters)[i] = p = Parameter::create(i);
            p->setMin( (::com::VstNumber)INT_MIN ); //entferne min, max ( siehe issue: 0000049 )
            p->setMax( (::com::VstNumber)INT_MAX );
            session->getParameterValues(p, i);
        }
        // add listener
		p->addValueChangedListener (
			boost::bind(&BridgedPlugin::parameterChanged, this, i)
		);
    }
}
//-----------------------------------------------------------------------------
BridgedPlugin::~BridgedPlugin() {
    using namespace interprocess;
    BridgeSessionManager::instance().closePluginSession(session);
}
//-----------------------------------------------------------------------------
void BridgedPlugin::baseConfigChanged() {
}
//-----------------------------------------------------------------------------
void BridgedPlugin::turnOff() {
    session->turnOff();
}
//-----------------------------------------------------------------------------
void BridgedPlugin::turnOn() {
    session->turnOn();
}
//-----------------------------------------------------------------------------
void BridgedPlugin::openPlugin() {
    session->openPlugin();
    initParameters();
}
//-----------------------------------------------------------------------------
void BridgedPlugin::closePlugin() {
    session->closePlugin();
}
//-----------------------------------------------------------------------------
size_t BridgedPlugin::getNumInputChannels() const {
    return session->getNumInputChannels();
}
//-----------------------------------------------------------------------------
size_t BridgedPlugin::getNumOutputChannels() const {
    return session->getNumOutputChannels();
}
//-----------------------------------------------------------------------------
bool BridgedPlugin::hasEditor() const {
    return session->hasEditor();
}
//-----------------------------------------------------------------------------
void BridgedPlugin::openEditor(void *window) {
    session->openEditor();
}
//-----------------------------------------------------------------------------
void BridgedPlugin::closeEditor(void *window) {
    session->closeEditor();
}
//-----------------------------------------------------------------------------
void BridgedPlugin::onEditorIdle() {
}
//-----------------------------------------------------------------------------
bool BridgedPlugin::isAccessable() const {
    return false;
}
//-----------------------------------------------------------------------------
size_t BridgedPlugin::getNumPrograms() {
    return 0;
}
//-----------------------------------------------------------------------------
std::string BridgedPlugin::getProgramName( size_t index ) {
    return "";
}
//-----------------------------------------------------------------------------
void BridgedPlugin::setProgram( size_t index ) {
}
//-----------------------------------------------------------------------------
int BridgedPlugin::getProgram() {
    return 0;
}
//-----------------------------------------------------------------------------
bool BridgedPlugin::canHandleMidiEvent() const {
    return false;
}
//-----------------------------------------------------------------------------
void BridgedPlugin::processMidiEvents( sambag::dsp::IMidiEvents * events ) {
}
//-----------------------------------------------------------------------------
size_t BridgedPlugin::getInitialDelay() const {
    return 0;
}
//-----------------------------------------------------------------------------
void BridgedPlugin::updatePluginInfo (::processing::PluginInfo &inf) const {
    session->updatePluginInfo(inf);
}
//-----------------------------------------------------------------------------
void BridgedPlugin::processPlugin( oldPr::Frames::T **ins,
oldPr::Frames::T **outs, size_t numSamples)
{
    session->process(ins, outs, numSamples);
}
//-----------------------------------------------------------------------------
std::pair<size_t, void*> BridgedPlugin::getStateData() const {
    return std::make_pair(0, (void*)NULL);
}
//-----------------------------------------------------------------------------
void BridgedPlugin::setStateData(size_t size, void* data) {
}
//-----------------------------------------------------------------------------
BridgedPlugin::AWindowImplPtr BridgedPlugin::getWindowImpl() {
    using frx::gui::components::interprocess::WindowSessionClient;
    AWindowImplPtr res = windowSession;
    if (res) {
        return res;
    }
    std::string id = session->getEditorSessionId();
    windowSession = res = WindowSessionClient::create(id);
    return res;
}
///////////////////////////////////////////////////////////////////////////////
APluginImpl * createBridgedPluginImpl(IHostInfo::Ptr hI,
    APluginImpl::Parameters* par, const std::string& loc)
{
    return new BridgedPlugin(hI, loc, par);
}
}} // namespace(s)

