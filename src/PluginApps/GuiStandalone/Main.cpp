#include <processing/VstForxPlug.hpp>
#include <sambag/dsp/VST2xPluginWrapper.hpp>
#include <string>
#include <com/Settings.h>
#include <aeffect.h>
#include <boost/tuple/tuple.hpp>
#include <boost/thread.hpp>
#include <scripts/PluginScriptCtrl.hpp>
#include <sambag/disco/components/Window.hpp>
#include <gui/components/VstForxEditor.hpp>
#include <sambag/disco/IResourceManager.hpp>
#include <sambag/disco/components/WindowToolkit.hpp>
#ifdef FRX_OS_LINUX
#include <sambag/disco/components/windowImpl/X11WindowToolkit.hpp>
#endif
#include <sambag/com/Common.hpp>
#include <boost/program_options.hpp>
#include <com/Settings.h>


#ifdef FRX_OS_WINDOWS
	#define WIN32ONLY(x) x
	#include <crtdbg.h>
	#pragma comment(linker, "\"/manifestdependency:type='Win32' name='Microsoft.VC90.CRT' version='9.0.21022.8' processorArchitecture='X86'	publicKeyToken='1fc8b3b9a1e18e3b' language='*'\"")
	void* hInstance=NULL;
#else
	#define WIN32ONLY(x)
#endif

typedef sambag::dsp::vst::VST2xPluginWrapper<
	frx::processing::VstForxPlug, // Processor
	'frxr', // uid
	sambag::dsp::StdPluginTraits<
		2,4,false,::com::Settings::PROGRAM_PARAMETER
	>,
	frx::gui::components::CreateVstForxEditor
> Plugin;

Plugin * plug;
Plugin * createPlug();
frx::scripts::PluginScriptCtrl *scriptCtrl;
bool failed;
boost::thread processingThread;
bool plugProcessing = false;
namespace po = boost::program_options;
po::variables_map vm;
typedef std::string File;
typedef std::vector<File> Files;
typedef std::vector<std::string> Commands;
//-----------------------------------------------------------------------------
VstIntPtr testHostCallback(AEffect* effect, VstInt32 opcode,
		VstInt32 index, VstIntPtr value, void* ptr, float opt)
{
	/*SAMBAG_LOG_INFO<<"hostCallback { opc: "<<opcode<<
		", index: "<<index<<". value:"<<value<<", opt:"<<opt<<
		", ptr:"<<std::hex<<ptr<<" }";*/
	return 0;
}
//-----------------------------------------------------------------------------
void fillBlock(float **data, int blockSize) {
	for (int i=0; i<blockSize; ++i) {
		data[0][i] = (float)rand() / (float)RAND_MAX;
		data[1][i] = (float)rand() / (float)RAND_MAX;
	}
}
//-----------------------------------------------------------------------------
void processPlugin(Plugin * plug) {
	static const int BS = 512;
	float **in = new float*[2];
	float **out = new float*[4];
	in[0] = new float[BS];
	in[1] = new float[BS];
	fillBlock(in, BS);
	out[0] = new float[BS];
	out[1] = new float[BS];
    out[2] = new float[BS];
    out[3] = new float[BS];
	plug->suspend();
	plug->setSampleRate(44100.);
	plug->setBlockSize(BS);
	plug->resume();
	while(plugProcessing) {
		if (plug->isOpen()) {
			fillBlock(in, BS);
			plug->process(in, out, BS);
			boost::this_thread::sleep(boost::posix_time::milliseconds(10));
		}
	}
	delete[] in[0];
	delete[] in[1];
	delete[] out[0];
	delete[] out[1];
    delete[] out[2];
	delete[] out[3];
	delete[] in;
	delete[] out;
}
//-----------------------------------------------------------------------------
void onScriptExeFailed(void *src, const frx::scripts::ScriptExeFailedEvent &ev) 
{
}
//-----------------------------------------------------------------------------
void onScriptEnd(void *src, const frx::scripts::ScriptEnded &ev) {
}
//-----------------------------------------------------------------------------
void setUp() {
	std::cout<<"seting up..";
	srand ( time(NULL) );
	::com::initSettings(".");
	WIN32ONLY(
		_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF ); //VS memory tracking
	)

	namespace sce = sambag::com::events;
	try {
		plug = createPlug();
	} catch (...) {
		std::cout<<"createPlug failed."<<std::endl;	
		return;
	}
	if (!plug) {
		std::cout<<"createPlug failed."<<std::endl;	
		return;
	}
	scriptCtrl = new frx::scripts::PluginScriptCtrl();
	scriptCtrl->setPlugin(plug);
	scriptCtrl->sce::EventSender<frx::scripts::ScriptExeFailedEvent>::addEventListener(
		&onScriptExeFailed
	);
	scriptCtrl->sce::EventSender<frx::scripts::ScriptEnded>::addEventListener(
		&onScriptEnd
	);
	failed = false;
	std::cout<<"succeed."<<std::endl;
}
//-----------------------------------------------------------------------------
void tearDown() {
	std::cout<<"tearing down..";
	plugProcessing = false;
	processingThread.join();
	delete scriptCtrl;
	delete plug;
	std::cout<<"succeed."<<std::endl;
}
//-----------------------------------------------------------------------------
Plugin * createPlug() {
	audioMasterCallback audioMaster = &testHostCallback;
	Plugin *pl = new Plugin(audioMaster);
	frx::processing::VstForxPlug &vpl = *pl;
	vpl.setEffectPtr(pl);
	vpl.setMasterCallback((void*)audioMaster);
	return pl;
}
//-----------------------------------------------------------------------------
void processScript(const File &file) {
	if ( !std::filesystem::exists(file) ) {
		std::cout<<file<<" does not exist."<<std::endl;
		return;
	}
	std::cout<<"add script: "<<file<<std::endl;
	try {
		std::fstream f(file.c_str(), std::fstream::in);
		std::string res;
		std::stringstream ss;
		std::string line;
		while (std::getline(f, line)) {
			ss<<line<<std::endl;
		}
		f.close();
		// add script
		scriptCtrl->appendJob(ss.str());
	} catch(...) {
		std::cout<<"adding script failed."<<std::endl;
	}
}
//-----------------------------------------------------------------------------
void processScripts() {
	if (vm.count("scripts") == 0) {
		std::cout<<"no scripts to process."<<std::endl;
		return;
	}
	const Files &files = vm["scripts"].as<Files>();
	BOOST_FOREACH(const File &f, files) {
		processScript(f);
	}
}
//-----------------------------------------------------------------------------
void processExecutes() {
	if (vm.count("exec") == 0) {
		std::cout<<"nothing to execute."<<std::endl;
		return;
	}
	const Commands &execs = vm["exec"].as<Commands>();
	BOOST_FOREACH(const std::string &c, execs) {
		scriptCtrl->appendJob(c);
	}
}
//-----------------------------------------------------------------------------
bool processArguments(int narg, char **args) {
	po::options_description options("options for standalone app");
	options.add_options()
	("help", "produce help message")
    ("scripts,s", po::value<Files>(), "script files to load on startup")
    ("exec,e", po::value<Commands>(), "executes commands on startup");
	
	try {
		po::store(po::parse_command_line(narg, args, options), vm);
	} catch(...) {
		std::cout<<"invalide command line syntax."<<std::endl;
		return false;
	}
	po::notify(vm);
	if (vm.count("help")) {
		std::cout << options << std::endl;
		return false;
	}
	return true;
}
//-----------------------------------------------------------------------------
void onConsoleThread(bool *consoleRunning) {
	while (*consoleRunning) {
		std::string input;
		std::cout<<">";
		std::getline(std::cin, input);
		if (input=="exit()" || input=="quit()" || input=="bye()") {
            static_cast<frx::gui::components::VstForxEditor*>(plug->getEditor())->close();
			sambag::disco::components::getWindowToolkit()->quit();
			break;
		}
		try {
			scriptCtrl->execute(input);
		} catch(const sambag::lua::ExecutionFailed &ex) {
			std::cout<<ex.errMsg<<std::endl;
		}
	}
}
//-----------------------------------------------------------------------------
int main(int narg, char **args) {
	std::cout<<"hello dave.."<<std::endl;
	if (!processArguments(narg, args)) {
		return 0;
	}
	setUp();
	if (!scriptCtrl) {
		std::cout<<"creating script ctrl failed!"<<std::endl;
		return -1;
	}
	scriptCtrl->appendJob( "frx.openPlugin()" );
	scriptCtrl->appendJob( "frx.openEditor()" );
	scriptCtrl->appendJob( "require\"scripts/util\"" );
    scriptCtrl->appendJob( "require\"scripts/vstforx-helper\"" );
	processScripts();
    processExecutes();
	// Start the main loop in its own thread so that scripts can safely use
	// invokeLater() to queue X11 work onto the main-loop thread.
	boost::thread mainLoopThread([](){
		sambag::disco::components::Window::startMainLoop();
	});
	// Wait until the main loop is actually spinning before launching scripts.
	while (!sambag::disco::components::X11WindowToolkit::isMainLoopRunning()) {
		boost::this_thread::sleep(boost::posix_time::milliseconds(1));
	}
	scriptCtrl->start();
	// start console thread
	bool consoleRunning = true;
	boost::thread consoleThread(boost::bind(&onConsoleThread, &consoleRunning));
	// start processing thread
	plugProcessing = true;
	processingThread = boost::thread(
		boost::bind(&processPlugin, plug)
	);
	mainLoopThread.join();
	consoleRunning = false;
	scriptCtrl->join();
	consoleThread.join();
	tearDown();
	std::cout<<"bye dave."<<std::endl;
	return 0;
}
