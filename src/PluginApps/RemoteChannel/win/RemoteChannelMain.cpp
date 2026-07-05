#include "../Plugin.hpp"
#include <sambag/dsp/VST2xPluginWrapper.hpp>
#include <string>
#include <audioeffectx.h>
#include <exception>
#include <com/one4All.h>
#include <sambag/com/Common.hpp>

extern void *hInstance; // @see vstsdk2.4::vstplugmain.cpp
std::string getHomeDirectory();

#ifdef _DEBUG
#pragma comment(linker, "\"/manifestdependency:type='Win32' name='Microsoft.VC90.CRT' version='9.0.21022.8' processorArchitecture='X86' publicKeyToken='1fc8b3b9a1e18e3b' language='*'\"")
#endif

enum {RC_UID='fxRC'};

void globAddRemoteChannelSender(size_t numSender);
const char * globGetProductName();

//-----------------------------------------------------------------------------
AudioEffect * createEffectInstance ( audioMasterCallback audioMaster ) {
	// setup logging:
	::sambag::com::addLogFile(getHomeDirectory() + "/RemoteChannel.log");
	SAMBAG_LOG_INFO<<"woke up";
    // load plugin
    using namespace sambag::dsp::vst;
    // settingup plugin
    typedef VST2xPluginWrapper<
                frx::processing::remoteChannel::Plugin, // Processor
                RC_UID, // uid
                sambag::dsp::StdPluginTraits<
                        2,2,false,::com::Settings::PROGRAM_PARAMETER
                >
    > Plugin;
    // create plugin
    try {
		SAMBAG_LOG_INFO<<"creating effect instance: ...";
		Plugin *pl = new Plugin(audioMaster);
		SAMBAG_LOG_INFO<<"creating effect instance: SUCCEED";
		return pl;
	} catch (const std::exception &ex) {
		SAMBAG_LOG_ERR<<"Creating of plugin instance: FAILED, "<<ex.what();
		std::stringstream ss;
		ss<<"Creating of plugin instance failed: "<<ex.what();
		com::osMessageBox("Error",
		ss.str(), com::MSG_ALERT);
		return NULL;
        } catch(...) {
			SAMBAG_LOG_ERR<<"Creating of plugin instance failed: unkown error";
			com::osMessageBox("Error",
                        "Creating of plugin instance: FAILED, unkonwn reason.", com::MSG_ALERT);
			return NULL;
        }
	return NULL;
}
//-----------------------------------------------------------------------------
std::string getHomeDirectory() {
	const size_t N = 2048; 
	char _d[N];
	DWORD r = GetModuleFileName ( (HINSTANCE)hInstance, &_d[0], N );
	std::filesystem::path f( _d  );
	std::string res;
	if ( is_regular_file(f) ) {
		res = f.remove_filename().string();
	} else {
		res = f.string();
	}
	return res;
}

void globAddRemoteChannelSender(size_t numSender) {
#ifdef FRX_REMOTE_IS_DEMO
	if (numSender == 0) {
		return;
	}
	std::stringstream ss;
	ss<<"LIMITATION: you can't use more than one sender.";
	ss<<"The plugin will be loaded anyway but you cant use it!";
	throw(std::runtime_error(ss.str()));
#endif
}


const char * globGetProductName() {
	#ifdef FRX_REMOTE_IS_DEMO
		return "RemoteChannelSender_DEMO";
	#endif
	return "RemoteChannelSender";
}