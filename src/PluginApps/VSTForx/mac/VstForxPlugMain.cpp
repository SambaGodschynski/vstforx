#include "VstForxPlugMain.hpp"
#include <processing/VstForxPlug.hpp>
#include <gui/components/VstForxEditor.hpp>
#include <sambag/dsp/VST2xPluginWrapper.hpp>
#include <string>
#include <audioeffectx.h>
#include <com/Settings.h>
#include <com/one4All.h>
#include <sambag/disco/components/WindowToolkit.hpp>
#include <exception>
#include "VstForxResourceManager.hpp"
#include "CocoaHelper.hpp"
#include <sambag/com/Common.hpp>

std::string getHomeDirectory();

#ifdef FRX_IS_INSTRUMENT
	enum { _FRX_IS_INSTRUMENT = 1 };
#else
	enum { _FRX_IS_INSTRUMENT = 0 };
#endif
#ifdef FRX_IS_DEMO
	enum { _FRX_IS_DEMO = 1 };
#else
	enum { _FRX_IS_DEMO = 0 };
#endif

enum { FRX_UID = '_frx' + (_FRX_IS_INSTRUMENT*2) + (_FRX_IS_DEMO*3) };

//-----------------------------------------------------------------------------
AudioEffect * createEffectInstance ( audioMasterCallback audioMaster ) {
    // setup logging:
    ::sambag::com::addLogFile(getHomeDirectory() + "/VSTForx.log");
    SAMBAG_LOG_INFO<<"wake up";
	// init resourceManager
	try {
        SAMBAG_LOG_INFO<<"loading resources: ...";
		frx::VstForxResourceManager &rm = frx::VstForxResourceManager::instance();
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
	com::getSettings().setIsDemo((bool)_FRX_IS_DEMO);
	com::getSettings().setIsInstrument((bool)_FRX_IS_INSTRUMENT);
	sambag::disco::components::getWindowToolkit()->useWithoutMainloop();
	
	// load plugin
	using namespace sambag::dsp::vst;
	// settingup plugin
	typedef VST2xPluginWrapper<
		frx::processing::VstForxPlug, // Processor
		FRX_UID, // uid
		sambag::dsp::StdPluginTraits<
			2,2,_FRX_IS_INSTRUMENT,::com::Settings::PROGRAM_PARAMETER
		>
		,frx::gui::components::CreateVstForxEditor
	> Plugin;
	// create plugin
	try {
        SAMBAG_LOG_INFO<<"creating effect instance: ...";
		Plugin *pl = new Plugin(audioMaster);
		frx::processing::VstForxPlug &vpl = *pl;
		vpl.setEffectPtr(pl);
		vpl.setMasterCallback((void*)audioMaster);
        SAMBAG_LOG_INFO<<"creating effect instance: SUCCEED";
		return pl;
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
std::string getHomeDirectory() {
	return frx::com::CocoaHelper::getBundleLocation();
}


