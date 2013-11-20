#include "VstForxPlugMain.hpp"
#include <processing/VstForxPlug.hpp>
#include <gui/components/VstForxEditor.hpp>
#include <sambag/dsp/VST2xPluginWrapper.hpp>
#include <string>
#include <audioeffectx.h>
#include <com/Settings.h>
#include <com/one4All.h>
#include <sambag/disco/components/WindowToolkit.hpp>
#include <windows.h>
#include <exception>
#include "VstForxResourceManager.hpp"
#include "PluginApps/com/FrxPlugSettings.hpp"

extern void *hInstance; // @see vstsdk2.4::vstplugmain.cpp
std::string getHomeDirectory();

struct Console {
	Console();
	~Console();
};
Console::Console() {
	/*
		A process can be attached to at most one console:
		http://msdn.microsoft.com/en-us/library/windows/desktop/ms683150%28v=vs.85%29.aspx
	*/
	AllocConsole();
	freopen("conin$","r",stdin);
	freopen("conout$","w",stdout);
	freopen("conout$","w",stderr);
	HWND consoleHandle = GetConsoleWindow();
	MoveWindow(consoleHandle,1,1,680,480,1);
	std::cout<<"VSTForx console initalized."<<std::endl;
}
Console::~Console() {
	FreeConsole();
}


#ifdef _DEBUG
Console console;
#pragma comment(linker, "\"/manifestdependency:type='Win32' name='Microsoft.VC90.CRT' version='9.0.21022.8' processorArchitecture='X86' publicKeyToken='1fc8b3b9a1e18e3b' language='*'\"")
#endif

//-----------------------------------------------------------------------------
AudioEffect * createEffectInstance ( audioMasterCallback audioMaster ) {

	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF ); //VS memory tracking
	// setup logging:
	::sambag::com::addLogFile(getHomeDirectory() + "/VSTForx.log");
	SAMBAG_LOG_INFO<<"woke up";
	// init resourceManager
	try {
	        SAMBAG_LOG_INFO<<"loading resources: ...";
		frx::VstForxResourceManager &rm = frx::VstForxResourceManager::instance();
		rm.initMap((HINSTANCE)hInstance);
		sambag::disco::installResourceManager(rm);
		SAMBAG_LOG_INFO<<"loading resources: SUCCEED";
	} catch (const std::exception &ex) {
	    SAMBAG_LOG_ERR<<"loading of resources: FAILED, "<<ex.what();
		std::stringstream ss;
		ss<<"Initiation of plugin instance failed: "<<ex.what();
		com::osMessageBox("Error", 
			ss.str(), com::MSG_ALERT);
		return NULL;
	} catch(...) {
	        SAMBAG_LOG_ERR<<"loading of resources : FAILED, unknown error";
		com::osMessageBox("Error", 
			"Initiation of plugin instance failed: unkonwn reason.", com::MSG_ALERT);
		return NULL;
	}
	// init settings
	::com::initSettings(getHomeDirectory());
	sambag::disco::components::setGlobalUserData(
		"win32.hinstance",
		sambag::com::createObject((HINSTANCE)hInstance)
	);

	sambag::disco::components::getWindowToolkit()->useWithoutMainloop();
	
	// load plugin
	using namespace sambag::dsp::vst;
	// settingup plugin
	typedef VST2xPluginWrapper<
		frx::processing::VstForxPlug, // Processor
	        PlugSettings::FRX_UID, // uid
		sambag::dsp::StdPluginTraits<
		  2,2,
		  PlugSettings::IsInstrument,
		  ::com::Settings::PROGRAM_PARAMETER
		>,
		frx::gui::components::CreateVstForxEditor
	> Plugin;
	// create plugin
	try {
	        SAMBAG_LOG_INFO<<"creating effect instance: ...";
		Plugin *pl = new Plugin(audioMaster);
		frx::processing::VstForxPlug &vpl = *pl;
		vpl.setEffectPtr(pl);
		vpl.setMasterCallback(audioMaster);
		return pl;
		SAMBAG_LOG_INFO<<"creating effect instance: SUCCEED";
	} catch (const std::exception &ex) {
	        SAMBAG_LOG_ERR<<"creating effect instance: FAILED, "<<ex.what();
		std::stringstream ss;
		ss<<"Creating of plugin instance failed: "<<ex.what();
		com::osMessageBox("Error", 
			ss.str(), com::MSG_ALERT);
		return NULL;
	} catch(...) {
	        SAMBAG_LOG_ERR<<"creating effect instance: FAILED";
		com::osMessageBox("Error", 
			"Creating of plugin instance failed: unkonwn reason.", com::MSG_ALERT);
		return NULL;
	}
	return NULL;
}
//-----------------------------------------------------------------------------
void onDllEntry() {
}
//-----------------------------------------------------------------------------
void onDllExit() {
}
//-----------------------------------------------------------------------------
std::string getHomeDirectory() {
	const size_t N = 2048; 
	char _d[N];
	DWORD r = GetModuleFileName ( (HINSTANCE)hInstance, &_d[0], N );
	boost::filesystem::path f( _d  );
	std::string res;
	if ( is_regular_file(f) ) {
		res = f.remove_filename().string();
	} else {
		res = f.string();
	}
	return res;
}
