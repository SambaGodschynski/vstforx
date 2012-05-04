#include "PPiVst.h"
#include <string>
#include "vstgui.h"
#include <sstream>
#include "processing/ConcreteProcessAdapter.h"
#include <set>
#include "com/one4All.h"
#include <time.h>
#include "gui/Resources.h"
#include "com/RegisterBoostTypes.h"

extern HINSTANCE GetInstance();

using namespace processing;
using namespace std;
//---------------------------------------------------------------------------------------
std::string  getHomeDirectory();
//---------------------------------------------------------------------------------------
namespace { string home_dir; }
//---------------------------------------------------------------------------------------
namespace ppiGui {
	//-----------------------------------------------------------------------------------
	const long CHILDW_ICON = IDI_ICON1;
	//-----------------------------------------------------------------------------------
	const long CONTEXT_CURSOR = IDC_CONTEXT_CURSOR;
} // ppiGui
//=======================================================================================
//class PPIVst
//=======================================================================================
namespace {
//---------------------------------------------------------------------------------------
CBitmap * createBitmap ( long id ) {
	CBitmap *bmp = new CBitmap ( id );
	LOG_ASSERT ( bmp != NULL )
	return bmp;
}
} // namespace
//---------------------------------------------------------------------------------------
CBitmap ** loadResources() { 
	using namespace ppiGui;
	CBitmap ** bitmaps = new CBitmap*[ppiGui::Resources::NUM_BITMAPS];
	bitmaps[Resources::VSTPLUG_NODE]            = createBitmap(IDB_PNG6);
	bitmaps[Resources::VSTPLUG_INPUT]			= createBitmap(IDB_PNG5);
	bitmaps[Resources::VSTPLUG_OUTPUT]			= createBitmap(IDB_PNG7);
	bitmaps[Resources::VSTPLUG_OUTPUT_GLOW]		= createBitmap(IDB_PNG20);
	bitmaps[Resources::MAIN_INPUT_NODE]			= createBitmap (IDB_PNG1);
	bitmaps[Resources::MAIN_OUTPUT_NODE]		= createBitmap (IDB_PNG4);
	bitmaps[Resources::STD_KNOB]				= createBitmap (IDB_PNG13);
	bitmaps[Resources::PASSIVE_KNOB]			= createBitmap (IDB_PNG12);
	bitmaps[Resources::VOLUME_ADAPTER]			= createBitmap (IDB_PNG10);
	bitmaps[Resources::STEP_ADAPTER]			= createBitmap (IDB_PNG9);
	bitmaps[Resources::OSWITCH_ADAPTER]			= createBitmap (IDB_PNG19);
	bitmaps[Resources::ADSR_ADAPTER]			= createBitmap (IDB_PNG2);
	bitmaps[Resources::PEAK_TRACK_ADAPTER]		= createBitmap (IDB_PNG8);
	bitmaps[Resources::CHANNEL_SPLIT_ADAPTER]	= createBitmap (IDB_PNG3);
	bitmaps[Resources::MIDI_RECEIVER]	        = createBitmap (IDB_PNG35);
	bitmaps[Resources::BACKGROUND]			    = createBitmap (IDB_PNG36);
	// toolbox
	bitmaps[Resources::EDIT_BUTTON]				= createBitmap (IDB_PNG11);
	bitmaps[Resources::MENU_ROCKER]				= createBitmap (IDB_PNG14);
	bitmaps[Resources::TLBX_BK]					= createBitmap (IDB_PNG18);
	bitmaps[Resources::TLBX_BTN_USE]			= createBitmap (IDB_PNG17);
	bitmaps[Resources::TLBX_BTN_CNT]			= createBitmap (IDB_PNG15);
	bitmaps[Resources::TLBX_BTN_MOVE]			= createBitmap (IDB_PNG16);
	bitmaps[Resources::TLBX_BTN_SETUP]			= createBitmap (IDB_PNG21);
	bitmaps[Resources::DLG_SYSM_EXIT]			= createBitmap (IDB_PNG22);
	bitmaps[Resources::DLG_SETTINGS_FRAME]		= createBitmap (IDB_PNG23);
	bitmaps[Resources::DLG_BTN_REMOVE_DIR]		= createBitmap (IDB_PNG24);
	bitmaps[Resources::DLG_BTN_ADD_DIR]			= createBitmap (IDB_PNG25);
	bitmaps[Resources::DLG_BTN_CANCEL]			= createBitmap (IDB_PNG26);
	bitmaps[Resources::DLG_BTN_CHDIR]			= createBitmap (IDB_PNG27);
	bitmaps[Resources::DLG_BTN_OK]				= createBitmap (IDB_PNG28);
	bitmaps[Resources::DLG_CHKBX_FASTSCAN]		= createBitmap (IDB_PNG29);
	bitmaps[Resources::DLG_SCANNING_FRAME]		= createBitmap (IDB_PNG30);
	bitmaps[Resources::DLG_RESIZE_ROCKER]		= createBitmap (IDB_PNG31);
	bitmaps[Resources::DLG_RESIZE_ROCKER_WIDTH]	= createBitmap (IDB_PNG32);
	bitmaps[Resources::DLG_RESIZE_ROCKER_HEIGHT]= createBitmap (IDB_PNG33);
	bitmaps[Resources::DLG_SCAN_NOW]			= createBitmap (IDB_PNG34);
	return bitmaps;
}
//---------------------------------------------------------------------------------------
int PPIVst::instances = 0;
//---------------------------------------------------------------------------------------
// Konstruktor
PPIVst::PPIVst(audioMasterCallback audioMaster) :
AudioEffectX ( audioMaster , 0, com::Settings::PROGRAM_PARAMETER ),  //AudioEffectX ( ..., numProgramm, numParameter ) 
sampleRate (0),
blockSize(0),
chunk ( NULL ),
oldDelay(0)
{ 
#ifdef MEMORY_TRACKING_STD
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF ); //VS memory tracking
#endif
	setNumInputs (2); 
	setNumOutputs (2); 
	canProcessReplacing ();
	programsAreChunks(true);
	noTail ( false );
	
	// Initalisiere Resourcen
	if ( instances++ == 0 ) {
		// windows gdi+
		Gdiplus::GdiplusStartupInput gdiplusStartupInput; 
		Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL); 
	}

	// Initalisiere Editor
	try{
		setEditor ( new ppiGui::PpiEditor( this ) );
	}catch (...){
		MessageBox ( NULL, "Could not init Effect Editor.", "Error", 0 );
		editor = NULL;
	}

#ifdef FORX_IS_SYNTH
	isSynth(true);
	setUniqueID ( com::Settings::UNIQUE_ID_INSTR );
#else
	setUniqueID ( com::Settings::UNIQUE_ID );
#endif
	resume();
}
//---------------------------------------------------------------------------------------
VstInt32 PPIVst::canDo ( char *text ) {
	if (!strcmp (text, "receiveVstEvents") )
		return 1;
	if (!strcmp (text, "receiveVstMidiEvent") )
		return 1;
	return -1;	// explicitly can't do; 0 => don't know
}
//---------------------------------------------------------------------------------------
void PPIVst::initHostParameter() {
	for ( int i=0; i<graph->getNumHostParameter(); ++i ){
		graph->getHostParameter(i)->addValueChangedListener ( 
			boost::bind(&PPIVst::valueChanged, this, _1, _2)
		);
	}
}
//---------------------------------------------------------------------------------------
PPIVst::~PPIVst() {
	if ( chunk ) delete chunk;

	if ( --instances == 0 ) {
		Gdiplus::GdiplusShutdown(gdiplusToken); 
		settings.reset();
	}

	TOLOG ("-PPIVST");
}
//---------------------------------------------------------------------------------------
void PPIVst::eventHandler ( void *src, const ppiGui::OnClose &ev ) {
}
//---------------------------------------------------------------------------------------
void PPIVst::open() {
	settings = Settings::getSettings();
	time_t t; time(&t);
	string str_time( ctime(&t ) );
	str_time.at( str_time.length() - 1 ) = '+'; // "\n" entfernen
	TOLOG ("::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::");
	TOLOG (Settings::versionToString() + "++" + str_time + "+" );
	TOLOG ("::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::");

	// Initalisiere Graph
	graph = Graph::create(this);
	graph->EventSender<GraphChanged>::addEventListener(this);
	// nicht vergessen:
	static_cast<ppiGui::PpiEditor*>(editor)->setGraph ( graph );
	// Set HostInfo
	Graph::Janitor::Ptr janitor = graph->getJanitor();
	if ( sampleRate != 0.0 && blockSize != 0 )
		janitor->hostInfoChanged();
	// init Host Parameter
	initHostParameter();
	onUpdate = false;
	pluginCollection = PluginCollection::getPluginCollection();

	commandWorker = CommandWorker::getCommandWorker();

}
//---------------------------------------------------------------------------------------
void PPIVst::setBlockSize ( VstInt32 blockSize ){
	PPIVst::blockSize = blockSize;
	// Set HostInfo
	if ( !graph ) return;
	Graph::Janitor::Ptr janitor = graph->getJanitor();
	if ( sampleRate != 0.0 && blockSize != 0 ) janitor->hostInfoChanged();
}
//---------------------------------------------------------------------------------------
void PPIVst::setSampleRate( float sampleRate ){
	PPIVst::sampleRate = sampleRate;
	// Hole HostInfo
	if ( !graph ) return;
	Graph::Janitor::Ptr janitor = graph->getJanitor();
	if ( sampleRate != 0.0 && blockSize != 0 ) janitor->hostInfoChanged();
}
//---------------------------------------------------------------------------------------
void PPIVst::close() {
	AudioEffectX::close();
	graph.reset();
}
//---------------------------------------------------------------------------------------
void PPIVst::processReplacing(float ** inputs, float ** outputs, VstInt32 numSamples) {
	if ( !graph ) return;
	Frames fr ( inputs, numSamples ); 
	TRY_TO_LOCK_TIMED2 ( graph->getProcessingLock(), 30 );
	if ( !graph->getEndNode()->isActive() ){
		fr.setZero ( numSamples );
		fr.getBlock ( outputs, numSamples );
		return;
	} 
	graph->pushAndCopy ( &fr, numSamples );
	graph->processGraph( outputs, numSamples  );
}
//---------------------------------------------------------------------------------------
void PPIVst::setParameter(VstInt32 index, float value) {
	if ( !graph ) return;
	if (!onUpdate) *(graph->getHostParameter(index)) = value;
}
//---------------------------------------------------------------------------------------
void PPIVst::valueChanged ( void *src, const float &val ){
	Parameter *p = (Parameter*) src;
	onUpdate = true;
	this->setParameterAutomated ( p->getIndex(), val );
	onUpdate = false;
}
//---------------------------------------------------------------------------------------
float PPIVst::getParameter(VstInt32 index) {
	if ( !graph ) return 0.0;
	return *(graph->getHostParameter(index));
}
//---------------------------------------------------------------------------------------
void PPIVst::getParameterLabel(VstInt32 index, char * label) {
	if ( !graph ) return;
	const MyString &str = graph->getHostParameter(index)->getLabel();
	vst_strncpy ( label , str.c_str(), str.length() );
}
//---------------------------------------------------------------------------------------
void PPIVst::getParameterDisplay(VstInt32 index, char * text) {
	if ( !graph ) return;
	const MyString &str = graph->getHostParameter(index)->getDisplay();
	vst_strncpy ( text , str.c_str(), str.length() );
}
//---------------------------------------------------------------------------------------
void PPIVst::getParameterName(VstInt32 index, char *name) {
	if ( !graph ) return;
	const MyString &str = graph->getHostParameter(index)->getName();
	vst_strncpy ( name , str.c_str(), str.length() );
}
//---------------------------------------------------------------------------------------
VstInt32 PPIVst::processEvents(VstEvents * events) {
	if ( !graph ) return 0;
	if ( events->numEvents == 0 ) return 1;
	graph->processEvents( events );
	return 1;
}
//---------------------------------------------------------------------------------------
void PPIVst::suspend(){
	AudioEffectX::suspend();
}
//---------------------------------------------------------------------------------------
void PPIVst::resume(){
	AudioEffectX::resume();
	if ( chunk ) delete chunk; chunk = NULL;
}
//---------------------------------------------------------------------------------------
//wird aufgerufen wenn Host Parameter Chunk abruft.
//liefert groesse des Chunks in byte.
VstInt32 PPIVst::getChunk(void **data, bool isPreset) {
	try {
		com::MethodMessage<PPIVst> methodMessage("getChunk()");
		std::stringstream ss;
		oArchive ar(ss);
		register_types<oArchive>(ar);
		// byte stream des graphen:
		graph->save(ar);
		// byte stream der view:
		ppiGui::PpiEditor *ed = ( ppiGui::PpiEditor* ) editor;
		if (ed) ed->save (ar);
		// bytestream in c_array kopieren
		MyString str = ss.str();
		if ( chunk ) delete chunk;
		chunk = new char[str.size()];
		*data = (void*)chunk;
		str.copyToC_Str ( (char*)*data );
		TOLOG (MyString (str.size()) + " bytes transmitted to chunk.");
		return str.size();
	} catch (...) {
		com::MessageBox ( "Error while saving data.", "Error!", com::MSG_ALERT );
		return 0;
	}
}

//---------------------------------------------------------------------------------------
VstInt32 PPIVst::setChunk(void *data, VstInt32 byteSize, bool isPreset) {
	com::MethodMessage<PPIVst> methodMessage("setChunk()");
	try {
		// void *data in string stream transfr. 
		stringstream ss;
		string str( (char*)data, byteSize );
		ss<<str;
		iArchive ar(ss);
		register_types<iArchive>(ar);
		// restore graph
		Graph::Ptr alt = graph; // bezweckt das graph erst released wenn alt scope verliert
		graph = Graph::load( ar, this );
		// hostInfo
		Graph::Janitor::Ptr janitor = graph->getJanitor();
		if ( sampleRate != 0.0 && blockSize != 0 ) janitor->hostInfoChanged();
		// restore view
		ppiGui::PpiEditor *ed = ( ppiGui::PpiEditor* ) editor;
		if (ed) ed->load ( ar, graph );
		// react. listener
		initHostParameter();
		TOLOG (MyString (byteSize) + " bytes received from chunk.");
		return byteSize;
	} catch (...) {
		graph = Graph::create( this );
		com::MessageBox ( "VSTForx Error!", "Error while loading data.", com::MSG_ALERT );
		Graph::Janitor::Ptr janitor = graph->getJanitor();
		if ( sampleRate != 0.0 && blockSize != 0 ) janitor->hostInfoChanged();
		initHostParameter();
		return 0;
	}
}

//---------------------------------------------------------------------------------------
AudioEffect * createEffectInstance ( audioMasterCallback audioMaster ){

	try{
		return new PPIVst(audioMaster);
	}catch(...){
		MessageBox ( NULL, "Could not create VSTForx Effect Instance!", "Error!", 0 );
		return NULL;
	}
}
//---------------------------------------------------------------------------------------
std::string getHomeDirectory() {
	if ( home_dir == "" ) {
		//AudioEffectX::getDirectory(); Ableton Live liefert hier "" 
		const size_t N = 2048; 
		char _d[N];
		HINSTANCE instance = GetInstance();
		LOG_ASSERT ( instance );
		DWORD r = GetModuleFileName ( instance, &_d[0], N );
		LOG_ASSERT ( r );
		com::Filename f( _d  );
		if ( is_regular_file(f) ) home_dir = f.remove_filename().string();
		else home_dir = f.string();
	}
	return home_dir + "/";
}
