#include "RemoteChannelMain.hpp"
#include "../Plugin.hpp"
#include <sambag/dsp/VST2xPluginWrapper.hpp>
#include <string>
#include <audioeffectx.h>
#include <exception>
#include <com/one4All.h>
#include <sambag/com/Common.hpp>

enum {RC_UID='fxRC'};

//-----------------------------------------------------------------------------
AudioEffect * createEffectInstance ( audioMasterCallback audioMaster ) {
    // setup logging:
    ::sambag::com::addLogFile("VSTForx.RemoteChannel.log");
    SAMBAG_LOG_INFO<<"wake up";
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


