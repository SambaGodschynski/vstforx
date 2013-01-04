/*
 * ===========================================================================================================
 * VSTPlugin2x.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#include "processing/processing.h"
#include "VSTPlugin2x.h"
#include "com/Settings.h"
#include <boost/filesystem.hpp> 
#include "com/PluginCollection.h"
#include <float.h>
#include <boost/foreach.hpp>
#include <limits>
#include <OS_Specific/OS_com.h>

#define MAX_BFF_STR 2048

typedef AEffect* (*PluginEntryProc) (audioMasterCallback audioMaster);


namespace processing{
//------------------------------------------------------------------------------------------------------------
boost::unordered_map < AEffect*, VSTPlugin* > VSTPlugin::relatedPlugNode;
//------------------------------------------------------------------------------------------------------------
VSTPlugin::VSTPlugin( frx::processing::IHostInfo::Ptr hostInfo, const string &filename ) : 
OS_VSTPlugNode2x ( filename ), // initalisiert aEff
Plugin ( hostInfo, filename, 0,  0 ),  // ProcessAdapter
onPlugChangeParameterIndex (-1),
param(NULL),
canReceiveVstEvents(false),
ioChangedLock(false)
{ 
	loadModule( HostCallBackOnInit (          // erzeugt Mutex lock bis fertig geladen
		(audioMasterCallback)(hostInfo->getMasterCallback()), 
		(AudioEffectX*)(hostInfo->getEffectPtr()) ) 
	);
	
	VstPlugCategory pluginCategory = (VstPlugCategory)
		aEff->dispatcher(aEff, effGetPlugCategory, 0, 0, 0, 0);
	
	// shellplugid is setted by loadModule (the filename contains the
	// information eg.: 'plugin.dll@12345')
	if (shellPlugId==0 && pluginCategory==kPlugCategShell) {
		ShellPluginInfos infos;
		getShellPluginInfos(infos);
		// plugin delivers shell plugins, at this pouint we can't go
		// on because we have to specify which plugin we want.
		if (!infos.empty())
			throw 
				ShellPluginException(filename, infos);
	}

	// init i/o 
	size_t c = ( aEff->numInputs%2==0 ) ? aEff->numInputs/2 : aEff->numInputs/2 + 1; // anzahl der eingaenge
	for ( size_t i=0; i<c; ++i ) {
		createInputNode( getName() + " InputNode(" + MyString(i) + ")" );
	}
	c = ( aEff->numOutputs%2==0 ) ? aEff->numOutputs/2 : aEff->numOutputs/2 + 1; // anzahl der ausgaenge
	for ( size_t i=0; i<c; ++i ) {
		createOutputNode( getName() + " OutputNode(" + MyString(i) + ")" );
	}
	
	initPlug ( *this ); // muss nach init i/o erfolgen
	TOLOG ( "+" + getName() );
}
//------------------------------------------------------------------------------------------------------------
MyString VSTPlugin::extractNameFromFilename( const string &fileName ){
	boost::filesystem::path p(fileName);
	return MyString ( p.stem().string() );
}
//------------------------------------------------------------------------------------------------------------
void VSTPlugin::processMidiEvents( sambag::dsp::IMidiEvents * events ) {
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
//------------------------------------------------------------------------------------------------------------
void VSTPlugin::initPlug( VSTPlugin &plug ) {
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Objekt registrieren
	relatedPlugNode.insert ( pair < AEffect*, VSTPlugin* >( plug.aEff, &plug ) );
	//hole name und hersteller
	char bff[MAX_BFF_STR];
	bff[0] = '\0';
	plug.aEff->dispatcher ( plug.aEff, effGetEffectName, 0, NULL, &bff[0], NULL );
	plug.setPlugName ( string(bff) );
	if ( plug.getPlugName().length() == 0 ) plug.setPlugName ( extractNameFromFilename( plug.getLocation() ) );
	bff[0] = '\0';
	plug.aEff->dispatcher ( plug.aEff, effGetVendorString, 0, NULL, &bff[0], NULL );
	plug.setPlugVendor( string (bff) );
	plug.setIsSynth ( plug.can(effFlagsIsSynth) );
	plug.setUid ( plug.aEff->uniqueID );
	plug.setType ( PluginInfo::VST2X );

	if ( plug.aEff == &nullAEff ) {
		plug.setPlugName( "could not load " + plug.getLocation() );
	}
	
	// can receive vst events?
	char can[] = "receiveVstMidiEvent";
	//(AEffect* effect, VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float opt)
	int ret = plug.aEff->dispatcher ( plug.aEff, effCanDo, 0, 0, &can[0], 0.0 );
	plug.canReceiveVstEvents = ret == 1;
	
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
	plug.inMatrix = new float*[ plug.getNumInputNodes()*2 ];
	plug.outMatrix = new float*[ plug.getNumOutputNodes()*2 ];
	plug.framebuffer = Framebuffer( plug.getNumOutputNodes() );
	
	plug.setupFramesbuffer();
}
//------------------------------------------------------------------------------------------------------------
void VSTPlugin::setupFramesbuffer() {
	frx::processing::IHostInfo::Ptr hI = hostInfo.lock();
	if (!hI) {
		SAMBAG_THROW(sambag::com::exceptions::IllegalStateException,
			"Hostinfo == NULL"
		);
	}
	blockSize = hI->getBlockSize();
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
//------------------------------------------------------------------------------------------------------------
string getPrgNameX ( AEffect *aEff, size_t index ) {
	char bff[512] = {0};
	//(AEffect* effect, VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float opt)
	if ( !aEff->dispatcher ( aEff, effGetProgramNameIndexed, index, 0, &bff[0], 0.0f ) ) {
		aEff->dispatcher ( aEff, effSetProgram, 0, index, NULL, 0.0f );
		aEff->dispatcher ( aEff, effGetProgramName, 0, 0, &bff[0], 0.0f ); 
	}
	return string( &bff[0] );
}
//------------------------------------------------------------------------------------------------------------
string getPrgName ( size_t index ) {
	return "Init " + MyString(index);
}
//------------------------------------------------------------------------------------------------------------
void VSTPlugin::initProgramNames() {
	if ( getNumPrograms() == 0 ) return;
	for ( size_t i=0; i<getNumPrograms(); ++i ) {
		string str = getPrgNameX ( aEff, i );
		if ( str.length() == 0 ) str = getPrgName( i );
		programNames.push_back( str );
	}
}
//------------------------------------------------------------------------------------------------------------
size_t VSTPlugin::getNumPrograms() {
	return aEff->numPrograms;
}
//------------------------------------------------------------------------------------------------------------
int VSTPlugin::getProgram() {
	if ( getNumPrograms() == 0 ) return -1;
	return aEff->dispatcher ( aEff, effGetProgram, 0, 0, NULL, 0.0f );
}
//------------------------------------------------------------------------------------------------------------
std::string VSTPlugin::getProgramName( size_t index ) {
	if ( index > getNumPrograms() ) return "";
	return programNames[index];
}
//------------------------------------------------------------------------------------------------------------
void VSTPlugin::setProgram(size_t index) {
	if ( index > getNumPrograms() ) return;
	aEff->dispatcher ( aEff, effSetProgram, 0, index, NULL, 0.0f );
}
//------------------------------------------------------------------------------------------------------------
void VSTPlugin::hostBaseConfigChanged() {
	frx::processing::IHostInfo::Ptr hI = hostInfo.lock();
	if (!hI) {
		SAMBAG_THROW(sambag::com::exceptions::IllegalStateException,
			"Hostinfo == NULL"
		);
	}
	turnOff();
	setupFramesbuffer();
	// setze samplerate
	//(AEffect* effect, VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float opt)
	aEff->dispatcher ( aEff, effSetSampleRate, 0, 0, 0, hI->getSampleRate() );
	// setze blockSize  
	aEff->dispatcher ( aEff, effSetBlockSize, 0, hI->getBlockSize(), 0, 0 );
	turnOn();
}
//------------------------------------------------------------------------------------------------------------
void VSTPlugin::valueChanged(void *src, const float &v) {
	Parameter *p = (Parameter*) src;
	size_t index = p->getIndex();
	if ( onPlugChangeParameterIndex == index ) 
		return; // when called by editorParameterChanged
	if ( index>=param.size() || index<0 ) return;
	Parameter::Ptr param = getParameter (index);
	aEff->setParameter ( aEff, index, param->getValue() );	
	char bff[255];
	// hole Parameter name
	//(AEffect* effect, VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float opt)
	aEff->dispatcher ( aEff, effGetParamName, index, NULL, &bff[0], NULL );
	param->setName ( MyString(bff) );
	// hole Parameter label
	aEff->dispatcher ( aEff, effGetParamLabel, index, NULL, &bff[0], NULL );
	param->setLabel ( MyString(bff) );
	// hole Parameter Display
	aEff->dispatcher ( aEff, effGetParamDisplay, index, NULL, &bff[0], NULL );
	param->setDisplay( MyString(bff) );
}
//------------------------------------------------------------------------------------------------------------
void VSTPlugin::initParameter(){
	char bff[255];
	param = ParameterContainer ( aEff->numParams );
	// initalisiere parameter
	for ( size_t i=0; i<param.size(); i++ ){
		param[i] = Parameter::create(i);
		param[i]->setMin( (VstNumber)INT_MIN ); //entferne min, max ( siehe issue: 0000049 )
		param[i]->setMax( (VstNumber)INT_MAX );
		// hole Parameter wert
		param[i]->setValue ( aEff->getParameter ( aEff, i ) );
		// hole Parameter name
		aEff->dispatcher ( aEff, effGetParamName, i, NULL, &bff[0], NULL );
		param[i]->setName ( MyString(bff) );
		// hole Parameter label
		aEff->dispatcher ( aEff, effGetParamLabel, i, NULL, &bff[0], NULL );
		param[i]->setLabel ( MyString(bff) );
		// hole Parameter Display
		aEff->dispatcher ( aEff, effGetParamDisplay, i, NULL, &bff[0], NULL );
		param[i]->setDisplay( MyString(bff) );
		// add listener
		param[i]->addValueChangedListener ( 
			boost::bind(&VSTPlugin::valueChanged, this, _1, _2)
		);
	}
}
//------------------------------------------------------------------------------------------------------------
size_t VSTPlugin::getProcessDelay() const {
	return (size_t)aEff->initialDelay;
}
//------------------------------------------------------------------------------------------------------------
//ruft die processReplacing Methode des zugeordneten VST-Plugin auf.
void VSTPlugin::processAdapter( Processor::Int numSamples ) {
	// breite daten vor ( mappe frames => matrix )
	for ( size_t i=0; i<getNumInputNodes(); i+=2 ) {
		ProcessorNode::Ptr pr = getInputNode(i/2);
		
		if ( !pr->isActive() ) { // inaktiver input
			inMatrix[i] = nullFrame[0];
			inMatrix[i+1] = nullFrame[1];
			continue;
		}
		Frames *fr = pr->popFrame();    
		inMatrix[i] = (*fr)[0];
		inMatrix[i+1] = (*fr)[1];
	}
	
	for ( size_t i=0; i<framebuffer.size(); ++i ) framebuffer[i].setZero( numSamples );
	
	if (!ioChangedLock) { 
		// Process Event
		if ( can( effFlagsCanReplacing ) ) { 
			//aEff->processReplacing ( *aEffect, **src, **dst, frameSize );
			aEff->processReplacing ( aEff, inMatrix, outMatrix, numSamples );
		} else { 
			aEff->DECLARE_VST_DEPRECATED(process) ( aEff, inMatrix, outMatrix, numSamples );
		}
	}
	if ( aEff->numOutputs == 1 ) { // mono
		framebuffer[0].mixMonoToAll( numSamples );
		getOutputNode(0)->pushAndCopy( &framebuffer[0], numSamples );
		return;
	}
	for ( size_t i=0; i<getNumOutputNodes(); i++ ) {
		getOutputNode(i)->pushAndCopy( &framebuffer[i], numSamples );
	}
}
//------------------------------------------------------------------------------------------------------------
VSTPlugin::~VSTPlugin() {
	relatedPlugNode.erase ( aEff );
	turnOff();
	aEff->dispatcher ( aEff, effClose, 0, 0, 0, 0.0 );
	delete[] inMatrix;
	delete[] outMatrix;
	// TODO: hier gab es probleme, unload muss aber stattfinden
	if ( aEff != &nullAEff )
		unloadModule();
	TOLOG ( "-" + getName() );
}
//------------------------------------------------------------------------------------------------------------
inline VSTPlugin * VSTPlugin::getVSTPlugNode(AEffect *aEff){
	RelatedPlugNode::iterator it = relatedPlugNode.find ( aEff );
	if ( it == relatedPlugNode.end() ) return NULL;
	return (*it).second;
}
//------------------------------------------------------------------------------------------------------------
void VSTPlugin::onIOChanged() {
/*
	resolved with a message that i/o has changed.
	dynamic i/o update was impossible because some plugs 
	(that support dynamic i/o changes [in that case battery1]) gives no information what exactly changes. 
	In detail:
		the AEffect structure wasn't updated and a getSpeakerArrangement() 
		call had no evaluable result. 
*/
}
//------------------------------------------------------------------------------------------------------------
void VSTPlugin::onEditorParameterChanged (int index, float value){
	if ( param.empty() ) {
		return;
	}
	// try to lock:
	boost::unique_lock<boost::timed_mutex> lock( mutex, boost::try_to_lock);
	if (!lock.owns_lock()) {
		return; // lock failed
	}

	if ( index > (int)getNumParameter() ) {
		return;
	}
	onPlugChangeParameterIndex = index; 
	param[index]->setValue ( value );
	onPlugChangeParameterIndex = -1;
}
//------------------------------------------------------------------------------------------------------------
void VSTPlugin::save(com::oArchive &ar, const unsigned int version) const {
	ar << boost::serialization::base_object< Plugin > ( *this ); //.........................................1
	// save plugInfo
	const PluginInfo plugInfo = getPluginInfo();
	ar << plugInfo; //......................................................................................2

	// parameter
	ar << param; //.........................................................................................3
	ar << aEff->numInputs; // to make sure that io config is the same after save/load ......................4
	ar << aEff->numOutputs; //..............................................................................5
	// chunk
	size_t size = 0;
	if ( !can (effFlagsProgramChunks) ) {
		// nothing to save: size = 0
		ar << size; //......................................................................................6!
		return;
	}
	// save chunk
	void *data;
	//[ptr]: void** for chunk data address [index]: 0 for bank, 1 for program  @see AudioEffect::getChunk
	//(AEffect* effect, VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float opt)
	size = aEff->dispatcher ( aEff, effGetChunk, 0, 0, &data, 0 );
	ar << size; //..........................................................................................6!
	if ( size ) ar.save_binary ( data, size ); //...........................................................7
}
//------------------------------------------------------------------------------------------------------------
void VSTPlugin::load(com::iArchive &ar, const unsigned int version) {
	TRY_TO_LOCK_TIMED(mutex);
	ar >> boost::serialization::base_object< Plugin > ( *this ); //..........................................1
	// get plugInfo
	PluginInfo plugInfo;
	ar>>plugInfo; //.........................................................................................2

	try {
		// restore/update via db
		com::PluginCollection &pC = com::getPluginCollection();
		pC.restorePluginInfo ( hostInfo.lock(), plugInfo );
	} catch(...) {
	}

	//check type
	if ( plugInfo.pluginType != PluginInfo::VST2X )
		throw com::ppiError::SerializationError ("incompatible plugin types", __FILE__, __LINE__);
	setLocation ( plugInfo.location );
	// load Plugin
	frx::processing::IHostInfo::Ptr hI = hostInfo.lock();
	if (!hI) {
		SAMBAG_THROW(sambag::com::exceptions::IllegalStateException,
			"Hostinfo == NULL"
		);
	}
	OS_VSTPlugNode2x::setModuleLocation ( getLocation() );
	loadModule( HostCallBackOnInit (
		(audioMasterCallback)hI->getMasterCallback(), 
		(AudioEffectX*)hI->getEffectPtr() ) 
	);
	param.clear();
	initPlug ( *this );
	// parameter
	ar >> param; //.........................................................................................3

	// make sure that io config is the same after save/load
	VstInt32 numInputs, numOutputs;
	ar >> numInputs;  //....................................................................................4
	ar >> numOutputs; //....................................................................................5
	if( aEff->numInputs   != numInputs  ||
		aEff->numOutputs  != numOutputs ||
		aEff->numParams != param.size() ) 
	{
		com::osMessageBox(getPlugName(), getPlugName() + " I/O configuration has changed."
			" Plugin output ist stopped until reload!", com::MSG_ALERT);
		ioChangedLock = true;
		// do not return, because it breaks the restore mechanism
	}
	// init parameter
	for ( size_t i=0; i<param.size(); ++i ) {
		param[i]->addValueChangedListener (
			boost::bind(&VSTPlugin::valueChanged, this, _1, _2)
		);
		param[i]->setValue ( *param[i] );
	}

	// load chunk
	size_t size;
	ar >> size; //..........................................................................................6
	if (!size) 
		return;
	unsigned char *data[1] = { new unsigned char[size] };
	if ( size ) ar.load_binary ( *data, size ); //..........................................................7
	//[ptr]: chunk data [value]: byte size [index]: 0 for bank, 1 for program  @see AudioEffect::setChunk
	//(AEffect* effect, VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float opt)
	aEff->dispatcher ( aEff, effSetChunk, 0, size, *data, 0 );
	resetPlugin();
	delete *data;
}
//------------------------------------------------------------------------------------------------------------
void VSTPlugin::onPlugRequestWindowResize (size_t w, size_t h) {
	EventSender<ResizeEditorEvent>::notifyEventListeners( this, ResizeEditorEvent(w,h) );
}
//------------------------------------------------------------------------------------------------------------
void VSTPlugin::openEditor(void *window) {
	if (!window)
		return;
	ERect *size = NULL;
	// get editor size
	aEff->dispatcher ( aEff, effEditGetRect, 0, 0, &size, 0);
	// set size
	if ( size ) {
		onPlugRequestWindowResize(size->right, size->bottom);
	}
	aEff->dispatcher ( aEff, effEditOpen, 0, 0, window, 0);
}
//------------------------------------------------------------------------------------------------------------
void VSTPlugin::closeEditor(void *window) {
	if (!window)
		return;
	aEff->dispatcher ( aEff, effEditClose, 0, 0, window, 0);
}
//--------------------------------------------------------------------------------------------------------
void VSTPlugin::onEditorIdle() {
	aEff->dispatcher ( aEff, effEditIdle, 0, 0, 0, 0);
}
//------------------------------------------------------------------------------------------------------------
void VSTPlugin::getShellPluginInfos(ShellPluginInfos &out) {
	// scan shell for subplugins
	char tempName[256] = {0}; 
	VstInt32 plugUniqueID = 0;
	//(AEffect* effect, VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float opt)
	while ((plugUniqueID = aEff->dispatcher (aEff, effShellGetNextPlugin, 0, 0, tempName, 0)) != 0) { 
		// subplug needs a name 
		if (tempName[0] != 0) {
			out.push_back(ShellPluginInfo(std::string(tempName), plugUniqueID));
		}
	}
}
//------------------------------------------------------------------------------------------------------------
VstIntPtr VSTPlugin::_hostCallback ( AEffect* effect, 
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
	VSTPlugin *pl = it->second;
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
	frx::processing::IHostInfo::Ptr hI =  pl->hostInfo.lock();
	if (!hI) {
		SAMBAG_THROW(sambag::com::exceptions::IllegalStateException,
			"Hostinfo == NULL"
		);
	}
	// no specific handling: call VSTForx's host
	audioMasterCallback hostCallback = (audioMasterCallback)(hI->getMasterCallback());
	if ( !hostCallback ) 
		return 0;
	// eigentlicher host callback ( VSTForx nach host )
	return hostCallback( 
		( (AudioEffectX*)(hI->getEffectPtr()) )->getAeffect(),
		opcode, 
		index, 
		value, 
		ptr, 
		opt
	);
}
} //namespace processing

//------------------------------------------------------------------------------------------------------------
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
			if (!strcmp (text, "sizeWindow") )
				return 1;
			else
				break;
		}
	}

	return processing::VSTPlugin::_hostCallback ( effect, opcode, index, value, ptr, opt );
}



