/*
 * ============================================================================
 * VSTPlugin2x.cpp
 *      Author: Johannes Unger
 * ============================================================================
 */
#include "processing/processing.h"
#include "VST2xImpl.h"
#include "com/Settings.h"
#include <boost/filesystem.hpp> 
#include "com/PluginCollection.h"
#include <float.h>
#include <boost/foreach.hpp>
#include <limits>
#include <OS_Specific/OS_com.h>
#include "TestAeffect.hpp"
#include <sambag/com/Thread.hpp>
#include <sambag/dsp/TimeInfoVst2xHelper.hpp>
#include <sambag/dsp/VstMidiEventAdapter.hpp>
#include <sambag/disco/components/Window.hpp>

/**
 * get the apropriate handler from a window.
 * HWND, WindowRef or NSView
 */
extern void * __getHandlerForVstPlugins_(void*);


#define MAX_BFF_STR 2048
static const int FRX_VST2XPLUGIN_MAX_IDLE_MS = 20;

typedef AEffect* (*PluginEntryProc) (audioMasterCallback audioMaster);

namespace frx { namespace processing {
//-----------------------------------------------------------------------------
APluginImpl::Ptr createVST2xPluginImpl(IHostInfo::Ptr hI,
    APluginImpl::Parameters *parameters, const std::string &location)
{
    return VSTPluginImpl::create(hI, parameters, location);
}
//-----------------------------------------------------------------------------
VSTPluginImpl::Ptr VSTPluginImpl::create( frx::processing::IHostInfo::Ptr hostInfo,
    Parameters *parameters,
    const string &location)
{
    Ptr res(new VSTPluginImpl(hostInfo, parameters, location));
    return res;
}
//-----------------------------------------------------------------------------
boost::unordered_map < AEffect*, VSTPluginImpl* > VSTPluginImpl::relatedPlugNode;
//-----------------------------------------------------------------------------
VSTPluginImpl::VSTPluginImpl( frx::processing::IHostInfo::Ptr hostInfo,
    Parameters *parameters,
    const string &filename ) :
        OS_VSTPlugNode2x ( filename ), // initalisiert aEff
        frx::processing::APluginImpl (hostInfo, filename, parameters),  // ProcessAdapter
        onPlugChangeParameterIndex (-1),
        oldEditorSize(EditorSize(0,0))
{
	loadModule( HostCallBackOnInit (          // erzeugt Mutex lock bis fertig geladen
		(audioMasterCallback)(hostInfo->getMasterCallback()),
		(AudioEffectX*)(hostInfo->getEffectPtr()) ) 
	);
}
//-----------------------------------------------------------------------------
void VSTPluginImpl::openPlugin()
{
	VstPlugCategory pluginCategory = (VstPlugCategory)
		aEff->dispatcher(aEff, effGetPlugCategory, 0, 0, 0, 0);
	
	// shellplugid is setted by loadModule (the filename contains the
	// information eg.: 'plugin.dll@12345')
	if (shellPlugId==0 && pluginCategory==kPlugCategShell) {
		oldPr::ShellPluginInfos infos;
		getShellPluginInfos(infos);
		// plugin delivers shell plugins, at this point we can't go
		// on because we have to specify which plugin we want.
		if (!infos.empty()) {
			throw oldPr::ShellPluginException(location, infos);
        }
    }
	initPlug ( *this );
}
//-----------------------------------------------------------------------------
void VSTPluginImpl::closePlugin()
{
	turnOff();
	aEff->dispatcher ( aEff, effClose, 0, 0, 0, 0.0 );
	// TODO: hier gab es probleme, unload muss aber stattfinden
	if ( aEff != &oldPr::nullAEff ) {
        unloadModule();
    }
}
//-----------------------------------------------------------------------------
void VSTPluginImpl::processMidiEvents( sambag::dsp::IMidiEvents * events ) {
	if ( !canHandleMidiEvent() ) {
		return;
	}
	if (!tmpMidiData) {
		tmpMidiData = VstMidiEventAdapterPtr(
			new sambag::dsp::VstMidiEventAdapter(events)
		);
		aEff->dispatcher( aEff, effProcessEvents, 0, NULL, (void*)tmpMidiData->events, NULL );
		return;
	}
	tmpMidiData->set(events);
	aEff->dispatcher( aEff, effProcessEvents, 0, NULL, (void*)tmpMidiData->events, NULL );
}
//-----------------------------------------------------------------------------
void VSTPluginImpl::updatePluginInfo (::processing::PluginInfo &inf) const {
    char bff[MAX_BFF_STR];
	bff[0] = '\0';
	aEff->dispatcher ( aEff, effGetEffectName, 0, NULL, &bff[0], NULL );
	inf.name = std::string(bff);
	if ( inf.name.length() == 0 ) {
        inf.name = com::getFileNameFromPath(location);
    }
	bff[0] = '\0';
	aEff->dispatcher ( aEff, effGetVendorString, 0, NULL, &bff[0], NULL );
	inf.vendor = string (bff);
	inf.isSynth  = can(effFlagsIsSynth);
	inf.uid = aEff->uniqueID;
	inf.pluginType = oldPr::PluginInfo::VST2X;
}
//-----------------------------------------------------------------------------
void VSTPluginImpl::initPlug( VSTPluginImpl &plug ) {
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Objekt registrieren
	relatedPlugNode.insert ( pair < AEffect*, VSTPluginImpl* >( plug.aEff, &plug ) );
    plug._processDelay = plug.aEff->initialDelay;
	if ( plug.aEff == &oldPr::nullAEff ) {
		plug.statusMsg = "could not load " + plug.location;
	}
		
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// reihenfolge wichtig! ( ueber debugger ermittelt )
	// setze samplerate  
	frx::processing::IHostInfo::Ptr hI =  plug.hostInfo.lock();
	if (!hI) {
		SAMBAG_THROW(sambag::com::exceptions::IllegalStateException,
			"Hostinfo == NULL"
		);
	}
	plug.aEff->dispatcher ( plug.aEff, effSetSampleRate, 0, 0, 0, hI->getSampleRate() );
	// setze blockSize  
	plug.aEff->dispatcher ( plug.aEff, effSetBlockSize, 0, hI->getBlockSize(), 0, 0 );
	// open
	plug.aEff->dispatcher ( plug.aEff, effOpen, 0, 0, 0, 0.0 );

	// parameter
	plug.initParameter();
	// program names
	plug.initProgramNames();
	
	plug.turnOn();

	plug.turnOff();
	plug.turnOn();
}
//-----------------------------------------------------------------------------
string getPrgNameX ( AEffect *aEff, size_t index ) {
	char bff[512] = {0};
	//(AEffect* effect, VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float opt)
	if ( !aEff->dispatcher ( aEff, effGetProgramNameIndexed, index, 0, &bff[0], 0.0f ) ) {
		aEff->dispatcher ( aEff, effSetProgram, 0, index, NULL, 0.0f );
        bff[0]='\0';
		aEff->dispatcher ( aEff, effGetProgramName, 0, 0, &bff[0], 0.0f ); 
	}
	return string( &bff[0] );
}
//-----------------------------------------------------------------------------
string getPrgName ( size_t index ) {
	return "Init " + com::MyString(index);
}
//-----------------------------------------------------------------------------
void VSTPluginImpl::initProgramNames() {
	if ( getNumPrograms() == 0 ) return;
	for ( size_t i=0; i<getNumPrograms(); ++i ) {
		string str = getPrgNameX ( aEff, i );
		if ( str.length() == 0 ) str = getPrgName( i );
		programNames.push_back( str );
	}
}
//-----------------------------------------------------------------------------
size_t VSTPluginImpl::getNumPrograms() {
	return aEff->numPrograms;
}
//-----------------------------------------------------------------------------
int VSTPluginImpl::getProgram() {
	if ( getNumPrograms() == 0 ) return -1;
	return aEff->dispatcher ( aEff, effGetProgram, 0, 0, NULL, 0.0f );
}
//-----------------------------------------------------------------------------
std::string VSTPluginImpl::getProgramName( size_t index ) {
	if ( index > getNumPrograms() ) return "";
	return programNames[index];
}
//-----------------------------------------------------------------------------
void VSTPluginImpl::setProgram(size_t index) {
	if ( index > getNumPrograms() ) return;
	aEff->dispatcher ( aEff, effSetProgram, 0, index, NULL, 0.0f );

	//update parameter
	for ( size_t i=0; i<parameters->size(); i++ ){
		(*parameters)[i]->setValue ( aEff->getParameter ( aEff, i ) );
	}
}
//-----------------------------------------------------------------------------
void VSTPluginImpl::baseConfigChanged() {
    frx::processing::IHostInfo::Ptr hI = hostInfo.lock();
	// setze samplerate
	//(AEffect* effect, VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float opt)
	aEff->dispatcher ( aEff, effSetSampleRate, 0, 0, 0, hI->getSampleRate() );
	// setze blockSize  
	aEff->dispatcher ( aEff, effSetBlockSize, 0, hI->getBlockSize(), 0, 0 );
}
//-----------------------------------------------------------------------------
void VSTPluginImpl::valueChanged(void *src, const float &v) {
	oldPrPr::Parameter *p = (oldPrPr::Parameter*) src;
	size_t index = p->getIndex();
	if ( onPlugChangeParameterIndex == index ) 
		return; // called by editorParameterChanged
	if ( index>=parameters->size() ) {
        return;
    }
	oldPrPr::Parameter::Ptr param = parameters->at(index);
	aEff->setParameter ( aEff, index, param->getValue() );	
	char bff[255] = {0};
	// hole Parameter name
    aEff->dispatcher ( aEff, effGetParamDisplay, index, NULL, &bff[0], NULL );
	param->setDisplay( com::MyString(&bff[0]) );
}
//-----------------------------------------------------------------------------
void VSTPluginImpl::initParameter() {
	char bff[255] = {0};
	parameters->resize( aEff->numParams );
	// initalisiere parameter
	for ( size_t i=0; i<parameters->size(); i++ ) {
        oldPrPr::Parameter::Ptr p = parameters->at(i);
        if (!p) {
            (*parameters)[i] = p = oldPrPr::Parameter::create(i);
            p->setMin( (com::VstNumber)INT_MIN ); //entferne min, max ( siehe issue: 0000049 )
            p->setMax( (com::VstNumber)INT_MAX );
            // hole Parameter wert
            p->setValue ( aEff->getParameter ( aEff, i ) );
            // hole Parameter namelo
            aEff->dispatcher ( aEff, effGetParamName, i, NULL, &bff[0], NULL );
            p->setName ( com::MyString(bff) );
            // hole Parameter label
            bff[0]='\0';
            aEff->dispatcher ( aEff, effGetParamLabel, i, NULL, &bff[0], NULL );
            p->setLabel ( com::MyString(bff) );
            // hole Parameter Display
            bff[0]='\0';
            aEff->dispatcher ( aEff, effGetParamDisplay, i, NULL, &bff[0], NULL );
            p->setDisplay( com::MyString(bff) );
        }
        // add listener
		p->addValueChangedListener (
			boost::bind(&VSTPluginImpl::valueChanged, this, _1, _2)
		);
	}
}
//-----------------------------------------------------------------------------
size_t VSTPluginImpl::getInitialDelay() const {
	return _processDelay = (size_t)aEff->initialDelay;
}
//-----------------------------------------------------------------------------
void VSTPluginImpl::processPlugin(oldPr::Frames::T **_in,
        oldPr::Frames::T **_out, size_t numSamples)
{
    // Process Event
    if ( can( effFlagsCanReplacing ) ) { 
        //aEff->processReplacing ( *aEffect, **src, **dst, frameSize );
        aEff->processReplacing ( aEff, _in, _out, numSamples );
    } else { 
        aEff->DECLARE_VST_DEPRECATED(process) ( aEff, _in, _out, numSamples );
    }
}
//-----------------------------------------------------------------------------
VSTPluginImpl::~VSTPluginImpl() {
    relatedPlugNode.erase ( aEff );
}
//-----------------------------------------------------------------------------
inline VSTPluginImpl * VSTPluginImpl::getVSTPlugImpl(AEffect *aEff){
	RelatedPlugNode::iterator it = relatedPlugNode.find ( aEff );
	if ( it == relatedPlugNode.end() ) return NULL;
	return (*it).second;
}
//-----------------------------------------------------------------------------
void VSTPluginImpl::onIOChanged() {
/*
	resolved with a message that i/o has changed.
	dynamic i/o update was impossible because some plugs 
	(that support dynamic i/o changes [in that case battery1]) gives no information what exactly changes. 
	In detail:
		the AEffect structure wasn't updated and a getSpeakerArrangement() 
		call had no evaluable result. 
*/
    namespace sce=sambag::com::events;
    size_t old = _processDelay;
    size_t _new = getInitialDelay();
    if (old!=_new) {
        sce::EventSender<sce::PropertyChanged>::notifyListeners(this,
            sce::PropertyChanged("process delay", old, _new)
        );
    }
}
//-----------------------------------------------------------------------------
void VSTPluginImpl::onEditorParameterChanged (int index, float value){
	if ( parameters->empty() ) {
		return;
	}
	// try to lock:
	boost::unique_lock<boost::timed_mutex> lock( mutex, boost::try_to_lock);
	if (!lock.owns_lock()) {
		return; // lock failed
	}

	if ( index > (int)parameters->size() ) {
		return;
	}
	onPlugChangeParameterIndex = index; 
	(*parameters)[index]->setValue ( value );
	onPlugChangeParameterIndex = -1;
}
//-----------------------------------------------------------------------------
std::pair<size_t, void*> VSTPluginImpl::getStateData() const {
    void *data;
    size_t s = aEff->dispatcher ( aEff, effGetChunk, 0, 0, &data, 0 );
    SAMBAG_LOG_TRACE<<"Vst2x getChunk "<<s<<" bytes got.";
    return std::make_pair(s, data);
}
//-----------------------------------------------------------------------------
void VSTPluginImpl::setStateData(size_t size, void* data)
{
    TRY_TO_LOCK_TIMED(mutex);
	if (!size) {
		return;
    }
	//[ptr]: chunk data [value]: byte size [index]: 0 for bank, 1 for program  @see AudioEffect::setChunk
	//(AEffect* effect, VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float opt)
	aEff->dispatcher ( aEff, effSetChunk, 0, size, data, 0 );
    resetPlugin();
    
    for ( size_t i=0; i<parameters->size(); i++ ) {
        oldPrPr::Parameter::Ptr p = parameters->at(i);
		p->setValue ( aEff->getParameter ( aEff, i ) );
    }
    SAMBAG_LOG_TRACE<<"Vst2x setChunk "<<size<<" bytes set.";

}
//-----------------------------------------------------------------------------
void VSTPluginImpl::onPlugRequestWindowResize (size_t w, size_t h) {
    namespace sce=sambag::com::events;
    EditorSize _new(w, h);
    if (oldEditorSize!=_new) {
        sce::EventSender<sce::PropertyChanged>::notifyListeners(this,
            sce::PropertyChanged("editor size", oldEditorSize, _new)
        );
    }
    oldEditorSize = _new;
}
//-----------------------------------------------------------------------------
void VSTPluginImpl::openEditor(sambag::disco::components::WindowPtr _window) {
    if (!_window)
		return;
    SAMBAG_ASSERT(_window->getWindowImpl());
	void *hndl = ::__getHandlerForVstPlugins_(
        _window->getWindowImpl()->getSystemHandle()
    );
    ERect *size = NULL;
	// get editor size
	aEff->dispatcher ( aEff, effEditGetRect, 0, 0, &size, 0);
	// set size
	if ( size ) {
        oldEditorSize = EditorSize(0,0); // reset old size
        onPlugRequestWindowResize(size->right - size->left, size->bottom - size->top);
	}
    aEff->dispatcher ( aEff, effEditOpen, 0, 0, hndl, 0);
}
//-----------------------------------------------------------------------------
void VSTPluginImpl::closeEditor(sambag::disco::components::WindowPtr window) {
    if (!window)
		return;
    SAMBAG_ASSERT(window->getWindowImpl());
	void *hndl = ::__getHandlerForVstPlugins_(
        window->getWindowImpl()->getSystemHandle()
    );
    aEff->dispatcher ( aEff, effEditClose, 0, 0, hndl, 0);
}
//-----------------------------------------------------------------------------
void VSTPluginImpl::onEditorIdle() { 
	aEff->dispatcher ( aEff, effEditIdle, 0, 0, 0, 0);
}
//-----------------------------------------------------------------------------
void VSTPluginImpl::getShellPluginInfos(oldPr::ShellPluginInfos &out) {
	// scan shell for subplugins
	char tempName[256] = {0}; 
	VstInt32 plugUniqueID = 0;
	//(AEffect* effect, VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float opt)
	while ((plugUniqueID = aEff->dispatcher (aEff, effShellGetNextPlugin, 0, 0, tempName, 0)) != 0) { 
		// subplug needs a name 
		if (tempName[0] != 0) {
			out.push_back(oldPr::ShellPluginInfo(std::string(tempName), plugUniqueID));
		}
	}
}
//-----------------------------------------------------------------------------
bool VSTPluginImpl::canHandleMidiEvent() const {
    if (can(effFlagsIsSynth)) {
        return true;
    }
    // can receive vst events?
    char can[] = "receiveVstMidiEvent";
    int ret = aEff->dispatcher ( aEff, effCanDo, 0, 0, &can[0], 0.0 );
    return ret == 1;
}
//-----------------------------------------------------------------------------
VstIntPtr VSTPluginImpl::_hostCallback ( AEffect* effect, 
						 VstInt32 opcode, 
						 VstInt32 index, 
						 VstIntPtr value, 
						 void* ptr, 
						 float opt ) 
{
	// special case: call during loadmodule
 	if ( callBkOnInit.first && callBkOnInit.second ) { 
		// Set callBkOnInit to zero before call.
		// Because when VSTForx is loaded in VSTForx then this
		// call occurs a stack overflow. 
		// ( it calls callBkOnInit[static] again and again because it is not zero )
		// see bug: 0000088
		if (opcode==audioMasterCurrentId) {
			return shellPlugIdOnInit;
		}
		HostCallBackOnInit tmp = callBkOnInit;
		callBkOnInit = HostCallBackOnInit( NULL, NULL );
		int ret = tmp.first( tmp.second->getAeffect(), opcode, index, value, ptr, opt );
		callBkOnInit = tmp;
		return ret;
	}

	// find related plugin
	RelatedPlugNode::iterator it = relatedPlugNode.find ( effect );
	if ( it == relatedPlugNode.end() ) return 0;
	VSTPluginImpl *pl = it->second;
	if ( !pl ) return 0;
	
	switch (opcode) {
		case audioMasterAutomate:
			pl->onEditorParameterChanged (index, opt);
			return 0;
		case audioMasterSizeWindow : // plugin fordert windowresize
			pl->onPlugRequestWindowResize ((size_t)index, (size_t)value);
			return 1;
		case audioMasterIOChanged:
			pl->onIOChanged();
			return 0;
	}
    using frx::processing::IHostInfo;
	IHostInfo::Ptr hI =  pl->hostInfo.lock();
	if (!hI) {
		SAMBAG_THROW(sambag::com::exceptions::IllegalStateException,
			"Hostinfo == NULL"
		);
	}
    if (hI->getMasterType() == IHostInfo::VST2X
        // execludes these:
        && opcode!=audioMasterProcessEvents)
    {
        // master is vst2x we can call master directly:
        audioMasterCallback hostCallback =
            (audioMasterCallback)(hI->getMasterCallback());
        if ( !hostCallback ) {
            SAMBAG_LOG_WARN<<"VST2X no mastercallback";
            return 0;
        }
        return hostCallback(
            ( (AudioEffectX*)(hI->getEffectPtr()) )->getAeffect(),
            opcode,
            index,
            value,
            ptr,
            opt
        );
    }
    VstInt32 ret = 0;
    bool suc = false;
    boost::tie(ret, suc) =
        pl->processRequest(hI, effect, opcode, index, value, ptr, opt);
    
    if (suc) {
        return ret;
    }
    SAMBAG_LOG_WARN<<"plugin vst2.x couldn't process request("<<opcode<<")";
    return 0;
}
//-----------------------------------------------------------------------------
std::pair<VstIntPtr, bool> VSTPluginImpl::processRequest( frx::processing::IHostInfo::Ptr hI,
                    AEffect* effect,
                    VstInt32 opcode, 
                    VstInt32 index, 
                    VstIntPtr value,
                    void* ptr,
                    float opt)
{
    switch (opcode) {
        //---------------------------------------------------------------------
        case audioMasterIdle:
            boost::this_thread::sleep( boost::posix_time::milliseconds(
                FRX_VST2XPLUGIN_MAX_IDLE_MS
            ));
            return std::make_pair(0, true);
        //---------------------------------------------------------------------
        case DECLARE_VST_DEPRECATED(audioMasterWantMidi):
            return std::make_pair(0, true);
        //---------------------------------------------------------------------
        case audioMasterGetSampleRate:
            return std::make_pair((VstInt32)hI->getSampleRate(), true);
        //---------------------------------------------------------------------
        case audioMasterGetBlockSize:
            return std::make_pair(hI->getBlockSize(), true);
        //---------------------------------------------------------------------
        case audioMasterUpdateDisplay:
            return std::make_pair(0, true);
        //---------------------------------------------------------------------
        case audioMasterBeginEdit:
            return std::make_pair(0, true);
        //---------------------------------------------------------------------
        case audioMasterEndEdit:
            return std::make_pair(0, true);
        //---------------------------------------------------------------------
        case audioMasterAutomate:
            return std::make_pair(0, true);
        //---------------------------------------------------------------------
        case DECLARE_VST_DEPRECATED(audioMasterGetPreviousPlug):
            return std::make_pair(0, true);
        //---------------------------------------------------------------------
        case DECLARE_VST_DEPRECATED(audioMasterGetNextPlug):
            return std::make_pair(0, true);
        //---------------------------------------------------------------------
        case DECLARE_VST_DEPRECATED(audioMasterWillReplaceOrAccumulate):
            return std::make_pair(0, true);
        //---------------------------------------------------------------------
        case audioMasterGetTime: {
            using namespace sambag::dsp;
            HostTimeInfo::Filter filter =
                timeInfoVst2xHelper::toHostTimeFilter(value);
            HostTimeInfo * ti = hI->getHostTimeInfo(filter);
            timeInfoVst2xHelper::convert(tmpInfo, *ti);
            tmpInfo.flags |= value; // set filter back to timeinfo
                                    // because HostTimeInfo doesn't save the filter
                                    // flags but VstTimeInfo does.
            return std::make_pair((VstIntPtr)&tmpInfo, true);
        }
        //---------------------------------------------------------------------
        case audioMasterProcessEvents: {
            VstEvents *ev = static_cast<VstEvents*>(ptr);
            if (!ev || ev->numEvents==0) {
                return std::make_pair(0, true);
            }
            sambag::dsp::VstMidiEventAdapter midiev(ev);
            try {
                oldPr::IMidiEventProcessor::EventSender::notifyListeners(this, &midiev);
            } catch(...) {
                SAMBAG_LOG_ERR<<"VST2xImpl. audioMasterProcessEvents failed";
                return std::make_pair(0, true);
            }
            return std::make_pair(1, true);
        }
        //---------------------------------------------------------------------
        case audioMasterGetCurrentProcessLevel:
            return std::make_pair(kVstProcessLevelUnknown, true);
        //---------------------------------------------------------------------
        // TODOs:
        case DECLARE_VST_DEPRECATED(audioMasterPinConnected):
            return std::make_pair(0, false);
        //---------------------------------------------------------------------
        case DECLARE_VST_DEPRECATED(audioMasterSetTime):
            return std::make_pair(0, false);
        //---------------------------------------------------------------------
        case DECLARE_VST_DEPRECATED(audioMasterTempoAt):
            return std::make_pair(0, false);
        //---------------------------------------------------------------------
        case DECLARE_VST_DEPRECATED(audioMasterGetNumAutomatableParameters):
            return std::make_pair(0, false);
        //---------------------------------------------------------------------
        case DECLARE_VST_DEPRECATED(audioMasterGetParameterQuantization):
            return std::make_pair(0, false);
        //---------------------------------------------------------------------
        case DECLARE_VST_DEPRECATED(audioMasterNeedIdle):
            return std::make_pair(0, false);
        //---------------------------------------------------------------------
        case audioMasterGetInputLatency:
            return std::make_pair(0, false);
        //---------------------------------------------------------------------
        case audioMasterGetOutputLatency:
            return std::make_pair(0, false);
        //---------------------------------------------------------------------
        case audioMasterGetAutomationState:
            return std::make_pair(0, false);
        //---------------------------------------------------------------------
        case audioMasterOfflineStart:
            return std::make_pair(0, false);
        //---------------------------------------------------------------------
        case audioMasterOfflineRead:
            return std::make_pair(0, false);
        //---------------------------------------------------------------------
        case audioMasterOfflineGetCurrentPass:
            return std::make_pair(0, false);
        //---------------------------------------------------------------------
        case audioMasterOfflineGetCurrentMetaPass:
            return std::make_pair(0, false);
        //---------------------------------------------------------------------
        case DECLARE_VST_DEPRECATED(audioMasterSetOutputSampleRate):
            return std::make_pair(0, false);
        //---------------------------------------------------------------------
        case DECLARE_VST_DEPRECATED(audioMasterGetOutputSpeakerArrangement):
            return std::make_pair(0, false);
        //---------------------------------------------------------------------
        case audioMasterGetVendorString:
            return std::make_pair(0, false);
        //---------------------------------------------------------------------
        case audioMasterGetProductString:
            return std::make_pair(0, false);
        //---------------------------------------------------------------------
        case audioMasterGetVendorVersion:
            return std::make_pair(0, false);
        //---------------------------------------------------------------------
        case audioMasterVendorSpecific:
            return std::make_pair(0, false);
        //---------------------------------------------------------------------
        case DECLARE_VST_DEPRECATED(audioMasterSetIcon):
            return std::make_pair(0, false);
        //---------------------------------------------------------------------
        case audioMasterCanDo:
            return std::make_pair(0, false);
        //---------------------------------------------------------------------
        case audioMasterGetLanguage:
            return std::make_pair(0, false);
        //---------------------------------------------------------------------
        case DECLARE_VST_DEPRECATED(audioMasterOpenWindow):
            return std::make_pair(0, false);
        //---------------------------------------------------------------------
        case DECLARE_VST_DEPRECATED(audioMasterCloseWindow):
            return std::make_pair(0, false);
        //---------------------------------------------------------------------
        case audioMasterGetDirectory:
            return std::make_pair(0, false);
        //---------------------------------------------------------------------
        case audioMasterOpenFileSelector:
            return std::make_pair(0, false);
        //---------------------------------------------------------------------
        case audioMasterCloseFileSelector:
            return std::make_pair(0, false);
        //---------------------------------------------------------------------
        case DECLARE_VST_DEPRECATED(audioMasterEditFile):
            return std::make_pair(0, false);
        //---------------------------------------------------------------------
        case DECLARE_VST_DEPRECATED(audioMasterGetChunkFile):
            return std::make_pair(0, false);
        //---------------------------------------------------------------------
        case DECLARE_VST_DEPRECATED(audioMasterGetInputSpeakerArrangement):
            return std::make_pair(0, false);
    }
    return std::make_pair(0, false);
}

}} //namespace processing

//-----------------------------------------------------------------------------
// Callback Methode.
// Wird von geladenen Plugin Aufgerufen.
// effect = zeiger auf Callback Quelle (?)
// opcode = Art des aufrufs
// index = ?
// value = evnt. uebegebner wert
// ptr = zeiger
// opt = optionaler float wert
VstIntPtr VSTCALLBACK pluginCallToPlugNode (AEffect* effect, 
									VstInt32 opcode, 
									VstInt32 index, 
									VstIntPtr value, 
									void* ptr, 
									float opt ) 
{
	switch (opcode) {
		case audioMasterVersion :
			return 2400;

		case audioMasterBeginEdit : // interessiert den host nicht
		case audioMasterEndEdit   : 
			return 0;

		case audioMasterCanDo : {
			const char *text = (const char*) ptr;
			if (!strcmp (text, "sizeWindow") ) {
				return 1;
			}
			if (!strcmp (text, "shellCategory") ) {
				return 1;
			}
            if (!strcmp (text, "receiveVstMidiEvent") ) {
				return 1;
			}
            if (!strcmp (text, "sendVstMidiEvent") ) {
				return 1;
			}
            if (!strcmp (text, "sendVstTimeInfo") ) {
				return 1;
			}
            if (!strcmp (text, "acceptIOChanges") ) {
				return -1;
			}
            if (!strcmp (text, "reportConnectionChanges") ) {
				return -1;
			}
			else
				break;
		}
	}

	return frx::processing::VSTPluginImpl::_hostCallback ( effect, opcode, index, value, ptr, opt );
}



