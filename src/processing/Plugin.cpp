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

namespace processing{

enum { ALL_CHANNEL = 16 };
	
//=============================================================================
// Plugin
//=============================================================================
//-----------------------------------------------------------------------------
Plugin::Plugin ( frx::processing::IHostInfo::Ptr hostInfo,
    const string &location, PluginInfo::PluginType type ) :
        ProcessAdapter ( hostInfo, 0, 0 ),
        editorPosX ( processing::parameter::Parameter::create() ),
        editorPosY ( processing::parameter::Parameter::create() ),
        editorOpen ( processing::parameter::Parameter::create() )
{
    using frx::processing::PluginFactory;
    impl = PluginFactory::instance().load(type, hostInfo, &parameters, location);
    
    if (!impl) {
        SAMBAG_THROW(sambag::com::exceptions::IllegalStateException,
                     "try to creating plugin without impl.");
    }
    
    namespace se = sambag::com::events;
    impl->se::EventSender<se::PropertyChanged>::addEventListener(
        boost::bind(&Plugin::onImplPropertyChanged, this, _1, _2)
    );
    
	setLocation ( location );
	// init editorPos parameters
	editorPosX->setName("editor_X");
	editorPosY->setName("editor_Y");
	editorOpen->setName("editor_visibility");
	*editorPosX = 0.72f; // 0.5 = 0 SCREEN_X
	*editorPosY = 0.72f; // 0.5 = 0 SCREEN_Y
	*editorOpen = 0.0f;
	
    initListener();

    impl->openPlugin();
    impl->updatePluginInfo(pluginInfo);
    
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
void Plugin::setStatusMsg( const std::string &msg ) {
	statusMsg = msg;
}
//-----------------------------------------------------------------------------
void Plugin::initListener() {
    using namespace parameter;
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
    if (ev.getPropertyName() == "editor location") {
        frx::processing::APluginImpl::EditorLocation _new;
        ev.getNewValue(_new);
        com::events::EventSender<ResizeEditorEvent>::notifyEventListeners(
            this,
            ResizeEditorEvent(_new.first, _new.second)
        );
        return;
    }

}
//-----------------------------------------------------------------------------
void Plugin::processAdapter( Processor::Int numSamples ) {
	// breite daten vor ( mappe frames => matrix )
    size_t c=0;
	for ( size_t i=0; i<getNumInputNodes(); ++i ) {
		ProcessorNode::Ptr pr = getInputNode(i);
		
		if ( !pr->isActive() ) { // inaktiver input
			inMatrix[c++] = nullFrame[0];
			inMatrix[c++] = nullFrame[1];
			continue;
		}
		Frames *fr = pr->popFrame();    
		inMatrix[c++] = (*fr)[0];
		inMatrix[c++] = (*fr)[1];
	}
	
	for ( size_t i=0; i<framebuffer.size(); ++i ) {
        framebuffer[i].setZero( numSamples );
    }
	
    impl->processPlugin(inMatrix, outMatrix, numSamples);
    
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
		Frames *fr = &( framebuffer[i/2] );
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
Plugin::Ptr Plugin::create(frx::processing::IHostInfo::Ptr hI, const std::string &location)
{
    using frx::processing::PluginFactory;
    using frx::processing::APluginImpl;
    Ptr res( new Plugin(hI, location, PluginInfo::UNKNOWN) );
    res->self = res;
    return res;
}
//-----------------------------------------------------------------------------
Plugin::Ptr Plugin::createVST2x(frx::processing::IHostInfo::Ptr hI, const std::string &location)
{
    using frx::processing::PluginFactory;
    using frx::processing::APluginImpl;
    Ptr res( new Plugin(hI, location, PluginInfo::VST2X) );
    res->self = res;
    return res;
}
//-----------------------------------------------------------------------------
Plugin::Ptr Plugin::createVST3x(frx::processing::IHostInfo::Ptr hI, const std::string &location)
{
    using frx::processing::PluginFactory;
    using frx::processing::APluginImpl;
    Ptr res( new Plugin(hI, location, PluginInfo::VST3X) );
    res->self = res;
    return res;}
//-----------------------------------------------------------------------------
Plugin::Ptr Plugin::createAU(frx::processing::IHostInfo::Ptr hI, const std::string &location)
{
    using frx::processing::PluginFactory;
    using frx::processing::APluginImpl;
    Ptr res( new Plugin(hI, location, PluginInfo::AU) );
    res->self = res;
    return res;
}
}//namespace processing
