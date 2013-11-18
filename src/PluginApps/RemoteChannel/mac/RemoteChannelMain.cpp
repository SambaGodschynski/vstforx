#include "RemoteChannelMain.hpp"
#include "../Plugin.hpp"
#include <sambag/dsp/VST2xPluginWrapper.hpp>
#include <string>
#include <audioeffectx.h>
#include <exception>
#include <com/one4All.h>
#include <sambag/com/Common.hpp>

enum {RC_UID='fxRC'};

namespace frx { namespace com { 
    extern std::string getResourceLocation(const std::string &path);
    extern std::string getBundleLocation();
}}
void globAddRemoteChannelSender(size_t numSender);
const char * globGetProductName();

//-----------------------------------------------------------------------------
AudioEffect * createEffectInstance ( audioMasterCallback audioMaster ) {
    // setup logging:
    ::sambag::com::addLogFile(frx::com::getBundleLocation() + "/RemoteChannel.log");
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
