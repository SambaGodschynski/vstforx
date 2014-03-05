/*
 * ============================================================================
 * Plugin.cpp
 *      Author: Johannes Unger
 * ============================================================================
 */
#include "Plugin.h"
#include "OS_Specific/os_processing.h"
#include "pluginTypes/PluginFactory.hpp"
#include "com/One4All.h"
#include <boost/filesystem.hpp>
#include <sambag/com/exceptions/IllegalStateException.hpp>
#include <sambag/com/Common.hpp>
#include <com/PluginCollection.h>

namespace frx { namespace processing {

enum { ALL_CHANNEL = 16 };
	
//=============================================================================
// Plugin
//=============================================================================
//-----------------------------------------------------------------------------
Plugin::Plugin() : impl(NULL), processing(true) {}
//-----------------------------------------------------------------------------
Plugin::Plugin ( frx::processing::IHostInfo::Ptr hostInfo,
    const std::string &location, oldPr::PluginInfo::PluginType type ) :
        ProcessAdapter ( hostInfo, 0, 0 ),
        editorPosX ( oldPrPr::Parameter::create() ),
        editorPosY ( oldPrPr::Parameter::create() ),
        editorOpen ( oldPrPr::Parameter::create() ),
        impl(NULL),
        processing(true)
{
    
	setLocation (location);
    setType(type);
    loadImpl();
    // init editorPos parameters
	editorPosX->setName("editor_X");
	editorPosY->setName("editor_Y");
	editorOpen->setName("editor_visibility");
	*editorPosX = 0.72f; // 0.5 = 0 SCREEN_X
	*editorPosY = 0.72f; // 0.5 = 0 SCREEN_Y
	*editorOpen = 0.0f;
	
    initListener();

    // init i/o
    size_t tmp=impl->getNumInputChannels();
	size_t c = (tmp%2==0) ? tmp/2 : tmp/2 + 1; // anzahl der eingaenge
                            
	for ( size_t i=0; i<c; ++i ) {
		createInputNode( getName() + " InputNode(" + sambag::com::toString(i) + ")" );
	}
    tmp=impl->getNumOutputChannels();
	c = ( tmp%2==0 ) ? tmp/2 : tmp + 1; // anzahl der ausgaenge
    
	for ( size_t i=0; i<c; ++i ) {
		createOutputNode( getName() + " OutputNode(" + sambag::com::toString(i) + ")" );
	}
    setupFramesbuffer();
}
//-----------------------------------------------------------------------------
void Plugin::loadImpl() {
    if (impl) {
        impl->closePlugin();
        delete impl;
    }
    frx::processing::IHostInfo::Ptr hI = hostInfo.lock();
    if (!hI) {
		SAMBAG_THROW(sambag::com::exceptions::IllegalStateException,
			"Hostinfo == NULL"
		);
	}
    using frx::processing::PluginFactory;
    impl = PluginFactory::instance().load(hI, &parameters,
        pluginInfo.location, pluginInfo.pluginType);
    
    if (!impl) {
        SAMBAG_THROW(sambag::com::exceptions::IllegalStateException,
                     "try to creating plugin without impl.");
    }
    
    namespace se = sambag::com::events;
    impl->se::EventSender<se::PropertyChanged>::addEventListener(
        boost::bind(&Plugin::onImplPropertyChanged, this, _1, _2)
    );

    impl->openPlugin();
    impl->updatePluginInfo(pluginInfo);
}
//-----------------------------------------------------------------------------
void Plugin::initListener() {
    using oldPrPr::Parameter;
	Parameter::ParameterListenerFunction xC = boost::bind( 
		&Plugin::paramEditorPosXChanged, this, _1, _2 
	);
	Parameter::ParameterListenerFunction yC = boost::bind( 
		&Plugin::paramEditorPosYChanged, this, _1, _2 
	);
	Parameter::ParameterListenerFunction oC = boost::bind( 
		&Plugin::paramEditorOpenChanged, this, _1, _2 
	);
	Parameter::ParameterListenerFunction dC = boost::bind( 
		&Plugin::paramEditorOpenDisplayChanged, this, _1, _2 
	);
	editorPosX->addValueChangedListener(xC);
	editorPosY->addValueChangedListener(yC);
	paramEditorOpenConnection = editorOpen->addValueChangedListener(oC);
	editorOpen->addValueChangedListener(dC);
}
//-----------------------------------------------------------------------------
Plugin::~Plugin() {
    impl->closePlugin();
    delete impl;
    delete[] inMatrix;
	delete[] outMatrix;
}
//-----------------------------------------------------------------------------
void Plugin::onImplPropertyChanged(void*,
        const sambag::com::events::PropertyChanged &ev)
{
    namespace newEvents=sambag::com::events;
    namespace oldEvents=::com::events;
    if (ev.getPropertyName() == "process delay") {
        newEvents::EventSender<newEvents::PropertyChanged>::notifyListeners(
            this,
            ev
        );
        return;
    }
    if (ev.getPropertyName() == "editor size") {
        frx::processing::APluginImpl::EditorSize _new;
        ev.getNewValue(_new);
        com::events::EventSender<ResizeEditorEvent>::notifyEventListeners(
            this,
            ResizeEditorEvent(_new.first, _new.second)
        );
        return;
    }

}
//-----------------------------------------------------------------------------
void Plugin::processAdapter( oldPr::Processor::Int numSamples ) {
	// breite daten vor ( mappe frames => matrix )
    size_t c=0;
	for ( size_t i=0; i<getNumInputNodes(); ++i ) {
		oldPr::ProcessorNode::Ptr pr = getInputNode(i);
		
		if ( !pr->isActive() ) { // inaktiver input
			inMatrix[c++] = nullFrame[0];
			inMatrix[c++] = nullFrame[1];
			continue;
		}
		oldPr::Frames *fr = pr->popFrame();    
		inMatrix[c++] = (*fr)[0];
		inMatrix[c++] = (*fr)[1];
	}
	
	for ( size_t i=0; i<framebuffer.size(); ++i ) {
        framebuffer[i].setZero( numSamples );
    }
	
    if (processing) {
        impl->processPlugin(inMatrix, outMatrix, numSamples);
    }
	if ( impl->getNumOutputChannels() == 1 ) { // mono
		framebuffer[0].mixMonoToAll( numSamples );
		getOutputNode(0)->pushAndCopy( &framebuffer[0], numSamples );
		return;
	}
	for ( size_t i=0; i<getNumOutputNodes(); i++ ) {
		getOutputNode(i)->pushAndCopy( &framebuffer[i], numSamples );
	}
}
//-----------------------------------------------------------------------------
void Plugin::setupFramesbuffer() {
	frx::processing::IHostInfo::Ptr hI = hostInfo.lock();
	if (!hI) {
		SAMBAG_THROW(sambag::com::exceptions::IllegalStateException,
			"Hostinfo == NULL"
		);
	}
    
    inMatrix = new float*[ getNumInputNodes()*2 ];
	outMatrix = new float*[ getNumOutputNodes()*2 ];
	framebuffer = Framebuffer( getNumOutputNodes() );
	
    
	size_t blockSize = hI->getBlockSize();
	// mappe von frames nach float[][]
	for ( size_t i=0; i<getNumOutputNodes()*2; i+=2 ) {
		oldPr::Frames *fr = &( framebuffer[i/2] );
		fr->setSize ( blockSize );
		fr->setZero( blockSize );
		outMatrix[i] = (*fr)[0];
		outMatrix[i+1] = (*fr)[1];
	}
	nullFrame.setSize (blockSize);
	nullFrame.setZero(blockSize);
}
//-----------------------------------------------------------------------------
void Plugin::hostBaseConfigChanged() {
	frx::processing::IHostInfo::Ptr hI = hostInfo.lock();
	if (!hI) {
		SAMBAG_THROW(sambag::com::exceptions::IllegalStateException,
			"Hostinfo == NULL"
		);
	}
	impl->turnOff();
    setupFramesbuffer();
    impl->baseConfigChanged();
    impl->turnOn();
}
//-----------------------------------------------------------------------------
bool Plugin::canHandleMidiEvent() const {
    return impl->canHandleMidiEvent();
}
//-----------------------------------------------------------------------------
void Plugin::processMidiEvents( sambag::dsp::IMidiEvents * events ) {
    impl->processMidiEvents(events);
}
//-----------------------------------------------------------------------------
bool Plugin::isAccessable() const {
    return impl->isAccessable();
}
//-----------------------------------------------------------------------------
bool Plugin::hasEditor() const {
    return impl->hasEditor();
}
//-----------------------------------------------------------------------------
void Plugin::openEditor(void *window) {
    impl->openEditor(window);
}
//-----------------------------------------------------------------------------
void Plugin::closeEditor(void *window) {
    impl->closeEditor(window);
}
//-----------------------------------------------------------------------------
void Plugin::onEditorIdle() {
    impl->onEditorIdle();
}
//-----------------------------------------------------------------------------
size_t Plugin::getNumPrograms() {
    return impl->getNumPrograms();
}
//-----------------------------------------------------------------------------
std::string Plugin::getProgramName( size_t index ) {
    return impl->getProgramName(index);
}
//-----------------------------------------------------------------------------
void Plugin::setProgram( size_t index ) {
    impl->setProgram(index);
}
//-----------------------------------------------------------------------------
int Plugin::getProgram() {
    return impl->getProgram();
}
//-----------------------------------------------------------------------------
size_t Plugin::getNumInputChannels() const {
    return impl->getNumInputChannels();
}
//-----------------------------------------------------------------------------
size_t Plugin::getNumOutputChannels() const {
    return impl->getNumOutputChannels();
}
//-----------------------------------------------------------------------------
std::string Plugin::getStatusMessage() const {
    return impl->statusMsg;
}
//-----------------------------------------------------------------------------
void Plugin::restorePluginInfo() {
    try {
		// restore/update via db
		com::PluginCollection::Ptr pC = com::getPluginCollection();
		pC->restorePluginInfo ( pluginInfo );
	} catch(...) {
	}
}
//-----------------------------------------------------------------------------
void Plugin::saveImplState(com::oArchive &ar, const unsigned int version, Int2Type<1>)
{
   
    size_t numInputs = impl->getNumInputChannels();
    size_t numOutputs = impl->getNumOutputChannels();
    
	ar << numInputs; 
	ar << numOutputs; 
	// chunk
	size_t dataSize;
    void *data;

    boost::tie(dataSize, data) = impl->getStateData();
    ar << dataSize;
	if ( dataSize ) {
        ar.save_binary (data, dataSize);
    }
}
//-----------------------------------------------------------------------------
void Plugin::loadImplState(com::iArchive &ar, const unsigned int version, Int2Type<1>)
{
    // confirm pluginInfo with DB
    restorePluginInfo();
    loadImpl();
    setupFramesbuffer();
    
    size_t numInputs;
    size_t numOutputs;
    
	ar >> numInputs;
	ar >> numOutputs;
    
    if( numInputs   != impl->getNumInputChannels() ||
		numOutputs  != impl->getNumOutputChannels() )
	{
        if (impl->statusMsg.length()==0) {
            impl->statusMsg = " I/O configuration has changed between save and restore. \
Processing is stopped. Please remove and load new.";
        }
		processing = false;
	}

    // chunk
	size_t dataSize;
    unsigned char *data;
    ar >> dataSize;
	if ( dataSize ) {
        data = new unsigned char[dataSize];
        ar.load_binary (data, dataSize);
    }
    impl->setStateData(dataSize, data);
    if (dataSize) {
        delete[] data;
    }
}
//-----------------------------------------------------------------------------
Plugin::Ptr Plugin::create(frx::processing::IHostInfo::Ptr hI, const std::string &location)
{
    using frx::processing::PluginFactory;
    using frx::processing::APluginImpl;
    Ptr res( new Plugin(hI, location, oldPr::PluginInfo::UNKNOWN) );
    res->self = res;
    return res;
}
//-----------------------------------------------------------------------------
Plugin::Ptr Plugin::createVST2x(frx::processing::IHostInfo::Ptr hI, const std::string &location)
{
    using frx::processing::PluginFactory;
    using frx::processing::APluginImpl;
    Ptr res( new Plugin(hI, location, oldPr::PluginInfo::VST2X) );
    res->self = res;
    return res;
}
//-----------------------------------------------------------------------------
Plugin::Ptr Plugin::createVST3x(frx::processing::IHostInfo::Ptr hI, const std::string &location)
{
    using frx::processing::PluginFactory;
    using frx::processing::APluginImpl;
    Ptr res( new Plugin(hI, location, oldPr::PluginInfo::VST3X) );
    res->self = res;
    return res;
}
//-----------------------------------------------------------------------------
Plugin::Ptr Plugin::createAU(frx::processing::IHostInfo::Ptr hI, const std::string &location)
{
    using frx::processing::PluginFactory;
    using frx::processing::APluginImpl;
    Ptr res( new Plugin(hI, location, oldPr::PluginInfo::AU) );
    res->self = res;
    return res;
}
}} //namespace processing
