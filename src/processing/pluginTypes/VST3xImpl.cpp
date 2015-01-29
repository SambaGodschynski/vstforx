/*
 * ============================================================================
 * VSTPlugin2x.cpp
 *  Author: Johannes Unger
 * ============================================================================
 */
#include "processing/processing.h"
#include "VST3xImpl.h"
#include "pluginterfaces/vst/ivstcomponent.h"
#include "processing/parameter/parameter.h"
#include "base/source/fstring.h"
#include "sambag/disco/components/Window.hpp"
#include "sambag/disco/components/windowImpl/CocoaWindowImpl.hpp"

extern void * __getHandlerForVstPlugins_(void*);

namespace frx { namespace processing {
namespace {
    std::string tostdstring(const Steinberg::Vst::String128 &str) {
        char ascii[128];
        Steinberg::ConstString::wideStringToMultiByte(&ascii[0], &str[0], 128);
        return std::string(ascii);
    }
}
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
    , plugin(NULL)
    , onPlugChangeParameterIndex(-1)
    , editor(NULL)
{
    std::string path;
    boost::tie(path, cid) = com::extractVSTPluginFilename(location);
    setModuleLocation(path);
    loadModule();
    if (cid.empty()) {
        oldPr::ShellPluginInfos infos;
        determinePluginInstances(infos);
        if (infos.empty()) {
            throw std::runtime_error("no plugins found");
        }
        if (infos.size()>0) {
            // now we have to throw because we don't know
            // which exact plugin the user wan't
            throw oldPr::ShellPluginException(location, infos);
        }
        cid = infos.front().id;
    }
}
//-----------------------------------------------------------------------------
void VST3PluginImpl::initParameters() {
    if (!controller) {
        return;
    }
    int num = (int)controller->getParameterCount();
    parameters->resize(num);
    for (int i = 0; i<num; ++i) {
        oldPrPr::Parameter::Ptr p = parameters->at(i);
        if (!p) {
            Steinberg::Vst::ParameterInfo pInf;
            controller->getParameterInfo(i, pInf);
            (*parameters)[i] = p = oldPrPr::Parameter::create(i);
            p->setMin( (com::VstNumber)INT_MIN ); //entferne min, max ( siehe issue: 0000049 )
            p->setMax( (com::VstNumber)INT_MAX );
            Steinberg::Vst::ParamValue value = controller->getParamNormalized(pInf.id);
            // wert
            p->setValue(value);
            // name
            p->setName(tostdstring(pInf.title));
            // label
            p->setLabel(tostdstring(pInf.units));
            // display
            Steinberg::Vst::String128 displ;
            controller->getParamStringByValue(pInf.id, value, &displ[0]);
            p->setDisplay(tostdstring(displ));
            // add listener
            p->addValueChangedListener (
                boost::bind(&VST3PluginImpl::valueChanged, this, _1, _2)
            );
        }
    }
}
//-----------------------------------------------------------------------------
void VST3PluginImpl::valueChanged(void *src, const float &value) {
    if (!controller) {
        return;
    }
	oldPrPr::Parameter *p = (oldPrPr::Parameter*) src;
	int index = (int)p->getIndex();
	if ( onPlugChangeParameterIndex == index ) 
		return; // called by editorParameterChanged
	if ( index>=parameters->size() ) {
        return;
    }
	oldPrPr::Parameter::Ptr param = parameters->at(index);
    Steinberg::Vst::ParameterInfo pInf;
    controller->getParameterInfo(index, pInf);
    controller->setParamNormalized(pInf.id, value);
    Steinberg::Vst::String128 displ;
    controller->getParamStringByValue(pInf.id, value, &displ[0]);
    param->setDisplay(tostdstring(displ));
}
//-----------------------------------------------------------------------------
void VST3PluginImpl::createPluginInstance(const std::string &id)
{
    Steinberg::FUID fuid;
    fuid.fromString(id.c_str());
    void * container[] = { NULL };
    Steinberg::tresult res = factory->createInstance(fuid, Steinberg::Vst::IComponent::iid, container);
    plugin = (Steinberg::Vst::IComponent*) *container;
    if (res!=Steinberg::kResultOk || !plugin) {
        throw std::runtime_error("creating plugin failed.");
    }
}
//-----------------------------------------------------------------------------
void VST3PluginImpl::determinePluginInstances(oldPr::ShellPluginInfos &infos)
{
    Steinberg::int32 nc = factory->countClasses();
    for (Steinberg::int32 i=0; i<nc; ++i) {
        Steinberg::PClassInfo info;
        factory->getClassInfo(i, &info);
        Steinberg::char8 cidString[50];
        Steinberg::FUID (info.cid).toString (cidString);
        infos.push_back(oldPr::ShellPluginInfo(std::string(&info.name[0]), std::string(cidString)));
    }
}
//-----------------------------------------------------------------------------
void VST3PluginImpl::initController() {
    using namespace Steinberg;
    using namespace Vst;

    // try to create the controller part from the component
    // (for Plug-ins which did not succeed to separate component from controller)
    if (plugin->queryInterface (IEditController::iid, (void**)&controller) != kResultTrue)
    {
        FUID controllerCID;
        // ask for the associated controller class ID
        if (plugin->getControllerClassId (controllerCID) == kResultTrue && controllerCID.isValid ())
        {
            // create its controller part created from the factory
            tresult result = factory->createInstance (controllerCID, IEditController::iid, (void**)&controller);
            if (controller && (result == kResultOk))
            {
                // initialize the component with our context
                if (controller->initialize (&dummyContext) != kResultOk) {
                    throw std::runtime_error("controller initalizing failed");
                }
            }
        }
    }
}
//-----------------------------------------------------------------------------
void VST3PluginImpl::baseConfigChanged() {
}
//-----------------------------------------------------------------------------
void VST3PluginImpl::turnOff() {
}
//-----------------------------------------------------------------------------
void VST3PluginImpl::turnOn() {
}
//-----------------------------------------------------------------------------
void VST3PluginImpl::openPlugin() {
    using namespace Steinberg;
    using namespace Vst;
    createPluginInstance(cid);
    // initialize the component with our context
    if (plugin->initialize (&dummyContext) != kResultOk) {
        throw std::runtime_error("plugin initalizing failed");
    }
    initController();
    initParameters();
    tryCreateEditor();
}
//-----------------------------------------------------------------------------
void VST3PluginImpl::closePlugin() {
    unloadPlugin();
}
//-----------------------------------------------------------------------------
size_t VST3PluginImpl::getNumInputChannels() const {
	return 0;
}
//-----------------------------------------------------------------------------
size_t VST3PluginImpl::getNumOutputChannels() const {
	return 0;
}
//-----------------------------------------------------------------------------
/**
* @return true, wenn Plugin ueber Editor verfuegt.
*/
bool VST3PluginImpl::hasEditor() const {
    if (editor!=NULL) {
        return true;
    }
    return false;
}
//-----------------------------------------------------------------------------
namespace {
    std::pair<void*, Steinberg::FIDString> getSytemHandle(sambag::disco::components::WindowPtr win)
    {
        using namespace sambag::disco::components;
        AWindowImpl::Ptr impl = win->getWindowImpl();
        // check if we have a cocoa window
        CocoaWindowImpl::Ptr cocoa =
        boost::dynamic_pointer_cast<CocoaWindowImpl>(impl);
        if (!cocoa) {
            void *res = ::__getHandlerForVstPlugins_(impl->getSystemHandle());
            return std::make_pair((void*)res, Steinberg::kPlatformTypeHWND);
        }
        return std::make_pair((void*)cocoa->getNSView(), Steinberg::kPlatformTypeNSView);
    }
}
void VST3PluginImpl::openEditor(sambag::disco::components::WindowPtr win) {
    namespace sd = sambag::disco;
    Steinberg::ViewRect size;
    editor->getSize(&size);
    win->setWindowSize(sd::Dimension(size.getWidth(), size.getHeight()));
    void *hnd = NULL;
    Steinberg::FIDString type = NULL;
    boost::tie(hnd, type) = getSytemHandle(win);
    editor->attached(hnd, type);
}
//-----------------------------------------------------------------------------
void VST3PluginImpl::closeEditor(sambag::disco::components::WindowPtr win) {
    editor->removed();
}
//-----------------------------------------------------------------------------
void VST3PluginImpl::onEditorIdle() {
}
//-----------------------------------------------------------------------------
bool VST3PluginImpl::isAccessable() const {
	return false;
}
//-----------------------------------------------------------------------------
/**
* @return Anzahl aller Plugin-Programme (aka. Presets)
*/
size_t VST3PluginImpl::getNumPrograms() {
	return 0;
}
//-----------------------------------------------------------------------------
/**
* @param index
* @return Program-Name zu index.
*/
std::string VST3PluginImpl::getProgramName( size_t index ) {
	return "";
}
//-----------------------------------------------------------------------------
/**
* Aktiviert Program zu index.
* @param index
*/
void VST3PluginImpl::setProgram( size_t index ) {
}
//-----------------------------------------------------------------------------
/**
* @return index des akuell gesetzten Program, falls vorhanden. Andernfalls -1.
*/
int VST3PluginImpl::getProgram() {
	return -1;
}
//-----------------------------------------------------------------------------
/**
* @return true, if plugin can handle MIDI events
*/
bool VST3PluginImpl::canHandleMidiEvent() const {
	return false;
}
//-----------------------------------------------------------------------------
void VST3PluginImpl::processMidiEvents( sambag::dsp::IMidiEvents * events ) 
{
}
//-----------------------------------------------------------------------------
size_t VST3PluginImpl::getInitialDelay() const {
	return 0;
}
//-----------------------------------------------------------------------------
/**
* @note fills out name, isSynth, uid, vendor, type
*/
void VST3PluginImpl::updatePluginInfo (::processing::PluginInfo &inf) const {
}
//-----------------------------------------------------------------------------
void VST3PluginImpl::processPlugin( oldPr::Frames::T **,
	oldPr::Frames::T **, size_t numSamples)
{
}
//-----------------------------------------------------------------------------
void VST3PluginImpl::unloadPlugin() {
    using namespace Steinberg;
    using namespace Vst;
	bool controllerIsComponent = false;
    if (editor) {
        editor->release();
        editor = NULL;
    }
	if (plugin)
	{
		controllerIsComponent = FUnknownPtr<IEditController> (plugin).getInterface () != 0;
		plugin->terminate ();
	}

	if (controller && controllerIsComponent == false) {
		controller->terminate ();
    }

	if (plugin)
	{
		plugin->release ();
		plugin = NULL;
	}

	if (controller)
	{
		controller->release ();
		controller = NULL;
	}

}
//-----------------------------------------------------------------------------
VST3PluginImpl::~VST3PluginImpl() {
    unloadModule();
}
//-----------------------------------------------------------------------------
std::pair<size_t, void*> VST3PluginImpl::getStateData() const {
	return std::make_pair(0, (void*)NULL);
}
//-----------------------------------------------------------------------------
void VST3PluginImpl::setStateData(size_t size, void* data) {
}
//-----------------------------------------------------------------------------
void VST3PluginImpl::tryCreateEditor() {
    using namespace Steinberg;
    using namespace Vst;
    if (!controller) {
        return;
    }
    editor = controller->createView (ViewType::kEditor);
    if (editor == NULL) {
        editor = controller->createView (NULL);
    }
    if (editor == NULL) {
        controller->queryInterface (IPlugView_iid, (void**) &editor);
    }
}
}} // namespace













