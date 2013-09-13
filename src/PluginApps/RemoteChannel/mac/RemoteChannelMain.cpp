#include "RemoteChannelMain.hpp"
#include "../Plugin.hpp"
#include <sambag/dsp/VST2xPluginWrapper.hpp>
#include <string>
#include <audioeffectx.h>
#include <exception>
#include <com/one4All.h>

enum {RC_UID='fxRC'};

//-----------------------------------------------------------------------------
AudioEffect * createEffectInstance ( audioMasterCallback audioMaster ) {
	// load plugin
	using namespace sambag::dsp::vst;
	// settingup plugin
	typedef VST2xPluginWrapper<
		frx::processing::Plugin, // Processor
		RC_UID, // uid
		sambag::dsp::StdPluginTraits<
			2,2,false,::com::Settings::PROGRAM_PARAMETER
		>
	> Plugin;
	// create plugin
	try {
		Plugin *pl = new Plugin(audioMaster);
		return pl;
	} catch (const std::exception &ex) {
		std::stringstream ss;
		ss<<"Creating of plugin instance failed: "<<ex.what();
		com::osMessageBox("Error", 
			ss.str(), com::MSG_ALERT);
		return NULL;
	} catch(...) {
		com::osMessageBox("Error", 
			"Creating of plugin instance failed: unkonwn reason.", com::MSG_ALERT);
		return NULL;
	}
	return NULL;
}


