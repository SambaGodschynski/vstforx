//============================================================================================================
// Klasse: VSTPlugNode.
// Ein VSTPlugNode Repraesentiert ein ladbares VST-Plugin.
// Bei Initalisierung wird das Plugin ueber ein DLL call geladen.
// Die meisten Methoden dieser Klasse dienen somit als Vermittler zwischen
// PPI und VST-Plugin.
//============================================================================================================

#include "processing/processing.h"
#include "VSTPlugNode2x.h"
#include "com/Settings.h"
#include <boost/filesystem.hpp> 
#include "com/PluginCollection.h"
#include <float.h>


#define MAX_BFF_STR 2048

typedef AEffect* (*PluginEntryProc) (audioMasterCallback audioMaster);


namespace processing{
//------------------------------------------------------------------------------------------------------------
boost::unordered_map < AEffect*, VSTPlugNode* > VSTPlugNode::relatedPlugNode;
//------------------------------------------------------------------------------------------------------------
VSTPlugNode::VSTPlugNode( IHostInfo *hostInfo, const string &filename ) : 
OS_VSTPlugNode2x ( filename ), // initalisiert aEff
PlugNode ( hostInfo, filename, 0,  0 ),  // ProcessAdapter
onPlugChangeParameterIndex (0),
param(NULL),
canReceiveVstEvents(false)
{ 
	loadModule( HostCallBackOnInit (          // erzeugt Mutex lock bis fertig geladen
		hostInfo->getAudioMasterCallback(), 
		hostInfo->getAudioEffectX() ) 
	);

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
MyString VSTPlugNode::extractNameFromFilename( const string &fileName ){
	boost::filesystem::path p(fileName);
	return MyString ( p.stem() );
}
//------------------------------------------------------------------------------------------------------------
void VSTPlugNode::processMidiEvents( VstEvents * events ) {
	if ( canHandleMidiEvent() )
		aEff->dispatcher ( aEff, effProcessEvents, 0, NULL, (void*)events, NULL );
}
//------------------------------------------------------------------------------------------------------------
void VSTPlugNode::initPlug( VSTPlugNode &plug ) {
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Objekt registrieren
	relatedPlugNode.insert ( pair < AEffect*, VSTPlugNode* >( plug.aEff, &plug ) );

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
	
	plug.aEff->dispatcher ( plug.aEff, effSetSampleRate, 0, 0, 0, plug.hostInfo->getSampleRate() );
	// setze blockSize  
	plug.aEff->dispatcher ( plug.aEff, effSetBlockSize, 0, plug.hostInfo->getBlockSize(), 0, 0 );
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
	
	plug.setupFramebuffer();
}
//------------------------------------------------------------------------------------------------------------
void VSTPlugNode::setupFramebuffer() {
	blockSize = hostInfo->getBlockSize();
	// mappe von frames nach float[][]
	for ( int i=0; i<getNumOutputNodes()*2; i+=2 ) {
		Frame *fr = &( framebuffer[i/2] );
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
void VSTPlugNode::initProgramNames() {
	if ( getNumPrograms() == 0 ) return;
	for ( size_t i=0; i<getNumPrograms(); ++i ) {
		string str = getPrgNameX ( aEff, i );
		if ( str.length() == 0 ) str = getPrgName( i );
		programNames.push_back( str );
	}
}
//------------------------------------------------------------------------------------------------------------
size_t VSTPlugNode::getNumPrograms() {
	return aEff->numPrograms;
}
//------------------------------------------------------------------------------------------------------------
int VSTPlugNode::getProgram() {
	if ( getNumPrograms() == 0 ) return -1;
	return aEff->dispatcher ( aEff, effGetProgram, 0, 0, NULL, 0.0f );
}
//------------------------------------------------------------------------------------------------------------
std::string VSTPlugNode::getProgramName( size_t index ) {
	if ( index > getNumPrograms() ) return "";
	return programNames[index];
}
//------------------------------------------------------------------------------------------------------------
void VSTPlugNode::setProgram(size_t index) {
	if ( index > getNumPrograms() ) return;
	aEff->dispatcher ( aEff, effSetProgram, 0, index, NULL, 0.0f );
}
//------------------------------------------------------------------------------------------------------------
void VSTPlugNode::hostInfoChanged() {
	turnOff();
	setupFramebuffer();
	// setze samplerate
	//(AEffect* effect, VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float opt)
	aEff->dispatcher ( aEff, effSetSampleRate, 0, 0, 0, hostInfo->getSampleRate() );
	// setze blockSize  
	aEff->dispatcher ( aEff, effSetBlockSize, 0, hostInfo->getBlockSize(), 0, 0 );
	turnOn();
}
//------------------------------------------------------------------------------------------------------------
void VSTPlugNode::valueChanged(void *src, const float &v) {
	Parameter *p = (Parameter*) src;
	size_t index = p->getIndex();
	if ( onPlugChangeParameterIndex == index ) return; // called by editorParameterChanged
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
void VSTPlugNode::initParameter(){
	char bff[255];
	param = ParameterContainer ( aEff->numParams );
	// initalisiere parameter
	for ( size_t i=0; i<param.size(); i++ ){
		param[i] = Parameter::create(i);
		param[i]->setMin( -FLT_MAX ); //entferne min, max ( siehe issue: 0000049 )
		param[i]->setMax( FLT_MAX );
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
		param[i]->addValueChangedListener ( this );
	}
}
//------------------------------------------------------------------------------------------------------------
size_t VSTPlugNode::getProcessDelay() const {
	return (size_t)aEff->initialDelay;
}
//------------------------------------------------------------------------------------------------------------
//ruft die processReplacing Methode des zugeordneten VST-Plugin auf.
void VSTPlugNode::_processAdapter( Processor::Int sampleFrames ) { 
	// breite daten vor ( mappe frames => matrix )
	for ( int i=0; i<getNumInputNodes(); i+=2 ) {
		ProcessorNode::Ptr pr = getInputNode(i/2);
		
		if ( !pr->isActive() ) { // inaktiver input
			inMatrix[i] = nullFrame[0];
			inMatrix[i+1] = nullFrame[1];
			continue;
		}
		Frame *fr = pr->popFrame();    
		inMatrix[i] = (*fr)[0];
		inMatrix[i+1] = (*fr)[1];
	}
	
	for ( int i=0; i<framebuffer.size(); ++i ) framebuffer[i].setZero( sampleFrames );

	// Process Event
	if ( can( effFlagsCanReplacing ) ) { 
		//aEff->processReplacing ( *aEffect, **src, **dst, frameSize );
		aEff->processReplacing ( aEff, inMatrix, outMatrix, sampleFrames );
	}else { 
		aEff->DECLARE_VST_DEPRECATED(process) ( aEff, inMatrix, outMatrix, sampleFrames );
	}
	if ( aEff->numOutputs == 1 ) { // mono
		framebuffer[0].mixMonoToAll( sampleFrames );
		getOutputNode(0)->pushAndCopy( &framebuffer[0], sampleFrames );
		return;
	}
	for ( int i=0; i<getNumOutputNodes(); i++ ) {
		getOutputNode(i)->pushAndCopy( &framebuffer[i], sampleFrames );
	}
}
//------------------------------------------------------------------------------------------------------------
VSTPlugNode::~VSTPlugNode() {
	relatedPlugNode.erase ( aEff );
	turnOff();
	aEff->dispatcher ( aEff, effClose, 0, 0, 0, 0.0 );
	delete[] inMatrix;
	delete[] outMatrix;
	// TODO: hier gab es probleme, unload muss aber stattfinden
	if ( aEff != &nullAEff ) unloadModule();
	TOLOG ( "-" + getName() );
}
//------------------------------------------------------------------------------------------------------------
inline VSTPlugNode * VSTPlugNode::getVSTPlugNode(AEffect *aEff){
	RelatedPlugNode::iterator it = relatedPlugNode.find ( aEff );
	if ( it == relatedPlugNode.end() ) return NULL;
	return (*it).second;
}
//------------------------------------------------------------------------------------------------------------
void VSTPlugNode::editorParameterChanged ( AEffect *aEff, int index, float value ){
	VSTPlugNode *plug = getVSTPlugNode ( aEff );
	if (!plug) return;
	
	if ( plug->param.empty() ) return;
	// try to lock:
	boost::unique_lock<boost::timed_mutex> lock( plug->mutex, boost::try_to_lock);
	if (!lock.owns_lock()) return; // lock failed

	if ( index > plug->getNumParameter() ) return;
	plug->onPlugChangeParameterIndex = index; 
	plug->param[index]->setValue ( value );
	plug->onPlugChangeParameterIndex = -1;
}
//------------------------------------------------------------------------------------------------------------
void VSTPlugNode::save(com::oArchive &ar, const unsigned int version) const {
	ar << boost::serialization::base_object< PlugNode > ( *this );
	// save plugInfo
	const PluginInfo plugInfo = getPluginInfo();
	ar << plugInfo;
	// parameter
	ar << param;
	// chunk
	size_t size = 0;
	if ( !can (effFlagsProgramChunks) ) {
		ar << size;
		return;
	}
	void *data;
	//[ptr]: void** for chunk data address [index]: 0 for bank, 1 for program  @see AudioEffect::getChunk
	//(AEffect* effect, VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float opt)
	size = aEff->dispatcher ( aEff, effGetChunk, 0, 0, &data, 0 );
	ar << size;
	if ( size ) ar.save_binary ( data, size );
}
//------------------------------------------------------------------------------------------------------------
void VSTPlugNode::load(com::iArchive &ar, const unsigned int version) {
	TRY_TO_LOCK_TIMED(mutex);
	ar >> boost::serialization::base_object< PlugNode > ( *this );
	// get plugInfo
	PluginInfo plugInfo;
	ar>>plugInfo;

	// restore/update via db
	com::PluginCollection::Ptr pC = com::PluginCollection::getPluginCollection();
	pC->restorePluginInfo ( hostInfo, plugInfo );

	//check type
	if ( plugInfo.pluginType != PluginInfo::VST2X )
		throw com::ppiError::SerializationError ("incompatible plugin types", __FILE__, __LINE__);

	setLocation ( plugInfo.location );

	// load Plugin
	OS_VSTPlugNode2x::setModuleLocation ( getLocation() );
	loadModule( HostCallBackOnInit (
		hostInfo->getAudioMasterCallback(), 
		hostInfo->getAudioEffectX() ) 
	);
	
	initPlug ( *this );

	// parameter
	ar >> param;
	for ( size_t i=0; i<param.size(); ++i ) {
		param[i]->addValueChangedListener ( this );
		param[i]->setValue ( *param[i] );
	}
	// chunk
	size_t size;
	ar >> size;
	if (!size) return;
	unsigned char *data[1] = { new unsigned char[size] };
	if ( size ) ar.load_binary ( *data, size );
	//[ptr]: chunk data [value]: byte size [index]: 0 for bank, 1 for program  @see AudioEffect::setChunk
	//(AEffect* effect, VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float opt)
	aEff->dispatcher ( aEff, effSetChunk, 0, size, *data, 0 );
	resetPlugin();
	delete *data;
}
//------------------------------------------------------------------------------------------------------------
void VSTPlugNode::plugRequestWindowResize ( AEffect* effect, size_t w, size_t h ) {
	RelatedPlugNode::iterator it = relatedPlugNode.find ( effect );
	if ( it == relatedPlugNode.end() ) return;
	VSTPlugNode *pl = it->second;
	if ( !pl ) return;
	pl->EventSender<ResizeEditorEvent>::notifyEventListeners( pl, ResizeEditorEvent(w,h) );

}
//------------------------------------------------------------------------------------------------------------
VstIntPtr VSTPlugNode::_hostCallback ( AEffect* effect, 
						 VstInt32 opcode, 
						 VstInt32 index, 
						 VstIntPtr value, 
						 void* ptr, 
						 float opt ) 
{
	
 	if ( callBkOnInit.first && callBkOnInit.second ) { 
		// Set callBkOnInit to zero before call.
		// Because when VSTForx is loaded in VSTForx then this
		// call occurs a stack overflow. 
		// ( it calls callBkOnInit[static] again and again because it is not zero )
		// see bug: 0000088
		HostCallBackOnInit tmp = callBkOnInit;
		callBkOnInit = HostCallBackOnInit( NULL, NULL );
		int ret = tmp.first( tmp.second->getAeffect(), opcode, index, value, ptr, opt );
		callBkOnInit = tmp;
		return ret;
	}
	
	RelatedPlugNode::iterator it = relatedPlugNode.find ( effect );
	if ( it == relatedPlugNode.end() ) return 0;
	VSTPlugNode *pl = it->second;
	if ( !pl ) return 0;

	AudioMasterCallback hostCallback = pl->hostInfo->getAudioMasterCallback();
	if ( !hostCallback ) return 0;
	// eigentlicher host callback ( VSTForx nach host )
	return hostCallback( pl->hostInfo->getAudioEffectX()->getAeffect(), opcode, index, value, ptr, opt);
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
	switch (opcode)
	{
		case audioMasterVersion :
			return 2400;

		case audioMasterBeginEdit : // interessiert den host nicht
		case audioMasterEndEdit   : 
			return 0;

		case audioMasterAutomate :
			processing::VSTPlugNode::editorParameterChanged ( effect, index, opt );
			return 0;

		case audioMasterSizeWindow : // plugin fordert windowresize
			processing::VSTPlugNode::plugRequestWindowResize ( effect, (size_t)index, (size_t)value );
			return 1;

		case audioMasterCanDo :
			const char *text = (const char*) ptr;
			if (!strcmp (text, "sizeWindow") )
				return 1;
			else break;

	}

	return processing::VSTPlugNode::_hostCallback ( effect, opcode, index, value, ptr, opt );
}



