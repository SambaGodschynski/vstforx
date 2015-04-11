/*
 * ============================================================================
 * VSTPlugin2x.cpp
 *  Author: Johannes Unger
 * ============================================================================
 */
#include "processing/processing.h"
#include "VST3xImpl.h"
#include "pluginterfaces/base/ipluginbase.h"
#include "pluginterfaces/gui/iplugview.h"
#include "pluginterfaces/vst/ivstcomponent.h"
#include "processing/parameter/parameter.h"
#include "base/source/fstring.h"
#include "sambag/disco/components/Window.hpp"
#include "sambag/disco/components/windowImpl/CocoaWindowImpl.hpp"



namespace Steinberg {
	DEF_CLASS_IID (IPluginBase)
	DEF_CLASS_IID (IPlugView)
}

extern void * __getHandlerForVstPlugins_(void*);

#define FRX_WARN_ON_FAILURE(x) warnOnFailure(x, __FILE__, __LINE__)

namespace frx { namespace processing {
namespace {
    std::string tostdstring(const Steinberg::Vst::String128 &str) {
        char ascii[128];
        Steinberg::ConstString::wideStringToMultiByte(&ascii[0], &str[0], 128);
        return std::string(ascii);
    }
    
    static int warnOnFailure (int result, const char * fname, int line)
    {
        using namespace Steinberg;
        if (result==kResultOk) {
            return result;
        }
        SAMBAG_LOG_WARN<<fname<<" unsuccessful: " << result
                       << " ("<< fname << ":" << line << ")";
        return result;
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
    , onPlugChangeParameterIndex(-1)
    , editor(NULL)
    , inParameterChanges(NULL)
    , outParameterChanges(NULL)
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
        if (infos.size()>1) {
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
            indexMap[pInf.id] = i; // add id to indexmap
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
            Steinberg::Vst::String128 displ = {0};
            controller->getParamStringByValue(pInf.id, value, &displ[0]);
            p->setDisplay(tostdstring(displ));
            // add listener
            p->addValueChangedListener (
                boost::bind(&VST3PluginImpl::valueChanged, this, _1, _2)
            );
        }
    }
    inParameterChanges = new VST3ParameterChanges();
    outParameterChanges = new VST3ParameterChanges();
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
    Steinberg::int32 dummyIndex;
    inParameterChanges->addParameterData (pInf.id, dummyIndex)->addPoint (0, value, dummyIndex);
    updateParameterDisplay(index);
}
//-----------------------------------------------------------------------------
void VST3PluginImpl::updateParameterDisplay(int index) {
    oldPrPr::Parameter::Ptr param = parameters->at(index);
    Steinberg::Vst::ParameterInfo pInf;
    controller->getParameterInfo(index, pInf);
    controller->setParamNormalized(pInf.id, param->getValue());
    Steinberg::Vst::String128 displ = {0};
    controller->getParamStringByValue(pInf.id, param->getValue(), &displ[0]);
    param->setDisplay(tostdstring(displ));
}
//-----------------------------------------------------------------------------
void VST3PluginImpl::createPluginInstance(const std::string &id)
{
    Steinberg::FUID fuid;
    fuid.fromString(id.c_str());
    component.loadFromFactory(factory, fuid);
    if (component.get() == NULL) {
        throw std::runtime_error("creating component failed.");
    }
    
    processor.loadFrom(component);
    
    //connect component and controller
    componentConnection.loadFrom (component);
    controllerConnection.loadFrom (controller);
    if (componentConnection && controllerConnection)
    {
        FRX_WARN_ON_FAILURE (controllerConnection->connect (componentConnection));
        FRX_WARN_ON_FAILURE (componentConnection->connect (controllerConnection));
    }
}
//-----------------------------------------------------------------------------
void VST3PluginImpl::determinePluginInstances(oldPr::ShellPluginInfos &infos) const
{
    Steinberg::int32 nc = factory->countClasses();
    for (Steinberg::int32 i=0; i<nc; ++i) {
        Steinberg::PClassInfo info;
        factory->getClassInfo(i, &info);
        if (std::strcmp (info.category, kVstAudioEffectClass) != 0) {
            continue;
        }
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
    if (!controller.loadFrom(component))
    {
        FUID controllerCID;
        // ask for the associated controller class ID
        if (component->getControllerClassId (controllerCID) == kResultTrue && controllerCID.isValid ())
        {
            if (controller.loadFromFactory(factory, controllerCID))
            {
                // initialize the component with our context
                FRX_WARN_ON_FAILURE(controller->initialize (&dummyContext));
            }
        }
    }
    if (controller) {
        controller->setComponentHandler(this);
    }
}
//-----------------------------------------------------------------------------
void VST3PluginImpl::baseConfigChanged() {
	frx::processing::IHostInfo::Ptr hI = hostInfo.lock();
	if (!hI) {
		SAMBAG_THROW(sambag::com::exceptions::IllegalStateException,
			"Hostinfo == NULL"
		);
	}
    turnOff();
    using namespace Steinberg;
    Vst::ProcessSetup setup;
    setup.symbolicSampleSize   = Vst::kSample32;
    setup.maxSamplesPerBlock   = hI->getBlockSize();
    setup.sampleRate           = hI->getSampleRate();
    setup.processMode          = Vst::kRealtime;
    FRX_WARN_ON_FAILURE(processor->setupProcessing (setup));
    turnOn();
}
//-----------------------------------------------------------------------------
void VST3PluginImpl::initBusArrangements() {
    using namespace Steinberg;
    using namespace Vst;
    SpeakerArrangement *ins = NULL, *outs = NULL;
    if (!processor) {
        return;
    }
    const Steinberg::int32 numInBuses = component->getBusCount (kAudio, kInput);
    ins = numInBuses > 0 ? new SpeakerArrangement[numInBuses] : NULL;
    for (int32 i = numInBuses; --i >= 0;) {
        FRX_WARN_ON_FAILURE( processor->getBusArrangement(kInput, i, ins[i]) );
    }
    const Steinberg::int32 numOutBuses = component->getBusCount (kAudio, kOutput);
    outs = numOutBuses > 0 ? new SpeakerArrangement[numOutBuses] : NULL;
    for (int32 i = numOutBuses; --i >= 0;) {
        FRX_WARN_ON_FAILURE( processor->getBusArrangement(kOutput, i, outs[i]) );
    }
    FRX_WARN_ON_FAILURE(
        processor->setBusArrangements(ins, numInBuses, outs, numOutBuses)
    );
}
//-----------------------------------------------------------------------------
void VST3PluginImpl::activateBusses(bool state, Steinberg::Vst::MediaTypes mediaType,
    Steinberg::Vst::BusDirections direction)
{
    const Steinberg::int32 numBuses = component->getBusCount (mediaType, direction);
    for (Steinberg::int32 i = numBuses; --i >= 0;) {
        FRX_WARN_ON_FAILURE(component->activateBus(mediaType, direction, i, state));
    }
}
//-----------------------------------------------------------------------------
void VST3PluginImpl::activateAudioBusses(bool val) {
    using namespace Steinberg;
    activateBusses(val, Vst::kAudio, Vst::kInput);
    activateBusses(val, Vst::kAudio, Vst::kOutput);
}
//-----------------------------------------------------------------------------
void VST3PluginImpl::activateEventBusses(bool val) {
    using namespace Steinberg;
    activateBusses(val, Vst::kEvent, Vst::kInput);
    activateBusses(val, Vst::kEvent, Vst::kOutput);
}
//-----------------------------------------------------------------------------
void VST3PluginImpl::turnOff() {
    activateAudioBusses(false);
    activateEventBusses(false);
    FRX_WARN_ON_FAILURE(component->setActive(false));
    
}
//-----------------------------------------------------------------------------
void VST3PluginImpl::turnOn() {
    activateAudioBusses(true);
    activateEventBusses(true);
    FRX_WARN_ON_FAILURE(component->setActive(true));
}
//-----------------------------------------------------------------------------
void VST3PluginImpl::openPlugin() {
    using namespace Steinberg;
    using namespace Vst;
    createPluginInstance(cid);
    // initialize the component with our context
    if (component->initialize (&dummyContext) != kResultOk) {
        throw std::runtime_error("component initalizing failed");
    }
    initController();
    initBusArrangements();
    baseConfigChanged();
    initParameters();
    tryCreateEditor();
}
//-----------------------------------------------------------------------------
void VST3PluginImpl::closePlugin() {
    unloadPlugin();
}
//-----------------------------------------------------------------------------
size_t VST3PluginImpl::getNumInputChannels() const {
    using namespace Steinberg;
    return (size_t)component->getBusCount(Vst::kAudio, Vst::kInput);
}
//-----------------------------------------------------------------------------
size_t VST3PluginImpl::getNumOutputChannels() const {
    using namespace Steinberg;
    return (size_t)component->getBusCount(Vst::kAudio, Vst::kOutput);
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
void VST3PluginImpl::onEditorBoundsChanged(const sambag::com::events::PropertyChanged &ev)
{
    if (!editor) {
        return;
    }
    namespace sd = sambag::disco;
    sd::Rectangle bounds;
    ev.getNewValue(bounds);
    int w = (int)bounds.getWidth();
    int h = (int)bounds.getHeight();
    if (w>0 && h>0 && editor->canResize()) {
        Steinberg::ViewRect size(0,0,w,h);
        editor->onSize(&size);
    }
}
//-----------------------------------------------------------------------------
namespace {
    std::pair<void*, Steinberg::FIDString>
    getSytemHandle(sambag::disco::components::WindowPtr win,
        Steinberg::IPlugView *editor)
    {
		using namespace sambag::disco::components;
        AWindowImpl::Ptr impl = win->getWindowImpl();
#ifdef DISCO_USE_COCOA
        // check if we have a cocoa window
        CocoaWindowImpl::Ptr cocoa =
        boost::dynamic_pointer_cast<CocoaWindowImpl>(impl);
        if (!cocoa) {
            void *res = ::__getHandlerForVstPlugins_(impl->getSystemHandle());
            return std::make_pair((void*)res, Steinberg::kPlatformTypeHWND);
        }
        return std::make_pair((void*)cocoa->getNSView(), Steinberg::kPlatformTypeNSView);
#else
	void *res = ::__getHandlerForVstPlugins_(impl->getSystemHandle());
    return std::make_pair((void*)res, Steinberg::kPlatformTypeHWND);
#endif
    }
}
void VST3PluginImpl::openEditor(sambag::disco::components::WindowPtr win) {
    namespace sd = sambag::disco;
    namespace sce = sambag::com::events;
    Steinberg::ViewRect size;
    editor->getSize(&size);
    win->setWindowSize(sd::Dimension(size.getWidth(), size.getHeight()));
    void *hnd = NULL;
    Steinberg::FIDString type = NULL;
    boost::tie(hnd, type) = getSytemHandle(win, editor);
    FRX_WARN_ON_FAILURE(editor->attached(hnd, type));
    // add event(s)
    evBoundsConnection = win->sce::EventSender<sce::PropertyChanged>::addEventListener(
        boost::bind(&VST3PluginImpl::onEditorBoundsChanged, this, _2)
    );
}
//-----------------------------------------------------------------------------
void VST3PluginImpl::closeEditor(sambag::disco::components::WindowPtr win) {
    editor->removed();
    if (evBoundsConnection.connected()) {
        evBoundsConnection.disconnect();
    }
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
void VST3PluginImpl::processMidiEvents( sambag::dsp::IMidiEvents::Ptr events )
{
    if (!midiEv) {
        midiEv = sambag::dsp::Vst3MidiAdapter::create();
    }
    midiEv->set(events);
}
//-----------------------------------------------------------------------------
size_t VST3PluginImpl::getInitialDelay() const {
	if (!processor) {
        return 0;
    }
    return (size_t)processor->getLatencySamples();
}
//-----------------------------------------------------------------------------
std::string VST3PluginImpl::getPluginName() const {
    oldPr::ShellPluginInfos infos;
    determinePluginInstances(infos);
    BOOST_FOREACH(const oldPr::ShellPluginInfo &x, infos) {
        // search for info with fitting id
        if (x.id == cid) {
            return x.name;
        }
    }
    return com::getFileNameFromPath(location);
}
//-----------------------------------------------------------------------------
std::string VST3PluginImpl::getPluginVendor() const {
    using namespace Steinberg;
    PFactoryInfo info;
    factory->getFactoryInfo(&info);
    return std::string(&info.vendor[0]);
}
//-----------------------------------------------------------------------------
/**
* @note fills out name, isSynth, uid, vendor, type
*/
void VST3PluginImpl::updatePluginInfo (::processing::PluginInfo &inf) const {
    using namespace Steinberg;
	inf.name = getPluginName();
    inf.vendor = getPluginVendor();
	inf.isSynth  = component->getBusCount(Vst::kEvent, Vst::kInput);
	inf.uid = cid;
	inf.pluginType = oldPr::PluginInfo::VST3X;
}
//-----------------------------------------------------------------------------
void VST3PluginImpl::processPlugin( oldPr::Frames::T ** inData,
	oldPr::Frames::T ** outData, size_t numSamples)
{
    using namespace Steinberg;
    if (!processor) {
        return;
    }
    // setup process data
    Vst::ProcessData data;
    data.processMode = Vst::kRealtime;
    data.symbolicSampleSize = Vst::kSample32;
    data.numSamples = numSamples;
    data.numInputs  = (int32)getNumInputChannels();
    data.numOutputs = (int32)getNumOutputChannels();
    data.inputParameterChanges = inParameterChanges;
    data.outputParameterChanges = outParameterChanges;
    // buffers
    Vst::AudioBusBuffers ins, outs;
    data.inputs = &ins;
    data.outputs = &outs;
    ins.numChannels = data.numInputs;
    outs.numChannels = data.numOutputs;
    ins.channelBuffers32 = inData;
    outs.channelBuffers32 = outData;
    // events
    if (midiEv) {
        data.inputEvents = midiEv.get();
        // TODO: out events
    }
    // process
    processor->process(data);
    // clear old events
    if (midiEv) {
        midiEv->set(sambag::dsp::IMidiEvents::Ptr());
    }
    updateParameterChages(outParameterChanges);
    inParameterChanges->clear();
    outParameterChanges->clear();
}
//-----------------------------------------------------------------------------
void VST3PluginImpl::updateParameterChages(VST3ParameterChanges* changes) {
    // set only the last available value (max sample offset)
    if (!changes || !parameters) {
        return;
    }
    typedef Steinberg::int32 int32;
    typedef Steinberg::Vst::ParamID ParamID;
    typedef Steinberg::Vst::IParamValueQueue IParamValueQueue;
    typedef Steinberg::Vst::ParamValue ParamValue;
    int32 count = changes->getParameterCount();
    for (int32 i=0; i<count; ++i) { // all parameter
        IParamValueQueue *queue = changes->getParameterData(i);
        if (!queue) {
            continue;
        }
        ParamID id = queue->getParameterId();
        int32 maxSampleOffset = INT_MIN;
        ParamValue endValue = -1.0;
        int32 nbPoints = queue->getPointCount();
        for (int32 i=0; i<nbPoints; ++i) { // all points
            int32 sampleOffset = INT_MIN;
            ParamValue value = 0;
            queue->getPoint(i, sampleOffset, value);
            if (sampleOffset>maxSampleOffset) {
                maxSampleOffset = sampleOffset;
                endValue = value;
            }
        } // all points
        if (maxSampleOffset!=INT_MIN) {
            int index = getParameterIndex(id);
            oldPrPr::Parameter::Ptr param = parameters->at(index);
            param->setValue(endValue);
            updateParameterDisplay(index);
        }
    }  // all parameter
}
//-----------------------------------------------------------------------------
void VST3PluginImpl::unloadPlugin() {
    using namespace Steinberg;
    using namespace Vst;
	bool controllerIsComponent = false;

	if (component)
	{
		controllerIsComponent = FUnknownPtr<IEditController> (component).getInterface () != 0;
		component->terminate ();
        component.reset();
	}

	if (controller && controllerIsComponent == false) {
		controller->terminate ();
        controller.reset();
    }
    
    if (processor) {
        processor.reset();
    }
    
    if (componentConnection) {
        componentConnection.reset();
    }
    
    if (controllerConnection) {
        controllerConnection.reset();
    }
    if(inParameterChanges) {
        inParameterChanges->release();
        inParameterChanges = NULL;
    }
    if(outParameterChanges) {
        outParameterChanges->release();
        outParameterChanges = NULL;
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
        controller->queryInterface (IPlugView::iid, (void**) &editor);
    }
}
//-----------------------------------------------------------------------------
int VST3PluginImpl::getParameterIndex(Steinberg::Vst::ParamID id) const
{
    VstParam2Index::const_iterator it = indexMap.find(id);
    if (it==indexMap.end()) {
        return -1;
    }
    return it->second;
}
///////////////////////////////////////////////////////////////////////////
// IComponentHandler
static bool doUIDsMatch (const Steinberg::TUID a, const Steinberg::TUID b)
{
    return std::memcmp (a, b, sizeof (Steinberg::TUID)) == 0;
}
//-----------------------------------------------------------------------------
Steinberg::tresult VST3PluginImpl::queryInterface (const Steinberg::TUID iid, void **obj)
{
    if (doUIDsMatch (iid, Steinberg::Vst::IComponentHandler::iid))
    {
        *obj = dynamic_cast<Steinberg::Vst::IComponentHandler*> (this);
        return Steinberg::kResultOk;
    }
    return Steinberg::kResultFalse;
}
//-----------------------------------------------------------------------------
Steinberg::uint32 VST3PluginImpl::addRef ()
{
    // we override the ref counting here because we own the plugin
    // and this will be deleted when the plugin is gone already
    return 1;
}
//-----------------------------------------------------------------------------
Steinberg::uint32 VST3PluginImpl::release ()
{
    // we override the ref counting here because we own the plugin
    // and this will be deleted when the plugin is gone already
    return 1;
}
//-----------------------------------------------------------------------------
Steinberg::tresult VST3PluginImpl::beginEdit (Steinberg::Vst::ParamID id)
{
    return Steinberg::kResultTrue;
}
//-----------------------------------------------------------------------------
Steinberg::tresult VST3PluginImpl::performEdit (Steinberg::Vst::ParamID id,
        Steinberg::Vst::ParamValue valueNormalized)
        
{
    int index = getParameterIndex(id);
    if (index<0) {
        SAMBAG_LOG_WARN<<"parameter " << id << " not found";
        return Steinberg::kResultTrue;
    }
    if ( parameters->empty() ) {
		return Steinberg::kResultTrue;
	}
	// try to lock:
	boost::unique_lock<boost::timed_mutex> lock( mutex, boost::try_to_lock);
	if (!lock.owns_lock()) {
		return Steinberg::kResultTrue; // lock failed
	}

	if ( index > (int)parameters->size() ) {
		return Steinberg::kResultTrue;
	}
	onPlugChangeParameterIndex = index; 
	(*parameters)[index]->setValue ( valueNormalized );
	onPlugChangeParameterIndex = -1;
    Steinberg::int32 dummyIndex;
    inParameterChanges->addParameterData (id, dummyIndex)->addPoint (0, valueNormalized, dummyIndex);
    updateParameterDisplay(index);
    return Steinberg::kResultTrue;
}
//-----------------------------------------------------------------------------
Steinberg::tresult VST3PluginImpl::endEdit (Steinberg::Vst::ParamID id)
{
    return Steinberg::kResultTrue;
}
//-----------------------------------------------------------------------------
Steinberg::tresult VST3PluginImpl::restartComponent (Steinberg::int32 flags)
{
    return Steinberg::kResultTrue;
}
}} // namespace













