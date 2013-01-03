#include <processing/VstForxPlug.hpp>
#include <sambag/dsp/VST2xPluginWrapper.hpp>
#include <string>
#include <com/Settings.h>
#include <aeffect.h>
#include <boost/tuple/tuple.hpp>
#include <boost/thread.hpp>
#include <scripts/PluginScriptCtrl.hpp>
#include <boost/thread.hpp>
#include <sambag/disco/components/Window.hpp>
#include <gui/components/VstForxEditor.hpp>
#include <sambag/disco/IResourceManager.hpp>
#include <sambag/disco/components/WindowToolkit.hpp>
#include <boost/program_options.hpp>
#include <com/settings.h>

#ifdef FRX_OS_WINDOWS
	#define WIN32ONLY(x) x
	#include <crtdbg.h>
#else
	#define WIN32ONLY(x)
#endif

typedef sambag::dsp::vst::VST2xPluginWrapper<
	frx::processing::VstForxPlug, // Processor
	'frxr', // uid
	sambag::dsp::StdPluginTraits<
		2,2,false,::com::Settings::PROGRAM_PARAMETER
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
//-----------------------------------------------------------------------------
int testHostCallback(AEffect* effect, VstInt32 opcode, 
		VstInt32 index, VstIntPtr value, void* ptr, float opt)
{
	return 0;
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
	plugProcessing = true;
	/*processingThread = boost::thread(
		boost::bind(&processPlugin, plug)
	);*/
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
	if ( !boost::filesystem::exists(file) ) {
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
bool processArguments(int narg, char **args) {
	po::options_description options("options for standalone app");
	options.add_options()
	("help", "produce help message")
    ("scripts,s", po::value<Files>(), "scripts");
	
	try {
		po::store(po::parse_command_line(narg, args, options), vm);
	} catch(...) {
		std::cout<<"invalide command line syntax."<<std::endl;
		return false;
	}
	po::notify(vm);
	if (vm.count("help")) {
		cout << options << std::endl;
		return false;
	}
	return true;
}
//-----------------------------------------------------------------------------
void onConsoleThread(bool *consoleRunning) {
	while (*consoleRunning) {
		std::string input;
		cout<<">";
		cin>>input;
		if (input=="exit()" || input=="quit()" || input=="bye()") {
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
	scriptCtrl->appendJob( "frxOpenPlugin()" );
	scriptCtrl->appendJob( "frxOpenEditor()" );
	processScripts();
	scriptCtrl->start();
	// start console thread
	bool consoleRunning = true;
	boost::thread consoleThread(boost::bind(&onConsoleThread, &consoleRunning));
	sambag::disco::components::Window::startMainLoop();
	consoleRunning = false;
	scriptCtrl->join();
	consoleThread.join();
	tearDown();
	std::cout<<"by dave"<<std::endl;
	return 0;
}
