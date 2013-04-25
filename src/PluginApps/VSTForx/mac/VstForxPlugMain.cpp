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

enum { FRX_UID = '_frx' + _FRX_IS_INSTRUMENT + _FRX_IS_DEMO };

//-----------------------------------------------------------------------------
AudioEffect * createEffectInstance ( audioMasterCallback audioMaster ) {
	//std::cout<<"XXXXXX VSTForx XXXXXXX"<<std::endl;
	// init resourceManager
	try {
		//frx::VstForxResourceManager &rm = frx::VstForxResourceManager::instance();
		//rm.initMap((HINSTANCE)hInstance);
		//sambag::disco::installResourceManager(rm);
	} catch (const std::exception &ex) {
		std::stringstream ss;
		ss<<"Initiation of plugin instance failed: "<<ex.what();
		com::osMessageBox("Error", 
			ss.str(), com::MSG_ALERT);
		return NULL;
	} catch(...) {
		com::osMessageBox("Error", 
			"Initiation of plugin instance failed: unkonwn reason.", com::MSG_ALERT);
		return NULL;
	}
	// init settings
	::com::initSettings(getHomeDirectory());
	com::getSettings().setIsDemo((bool)_FRX_IS_DEMO);
	sambag::disco::components::getWindowToolkit()->useWithoutMainloop();
	
	// load plugin
	using namespace sambag::dsp::vst;
	// settingup plugin
	typedef VST2xPluginWrapper<
		frx::processing::VstForxPlug, // Processor
		FRX_UID, // uid
		sambag::dsp::StdPluginTraits<
			2,2,_FRX_IS_INSTRUMENT,::com::Settings::PROGRAM_PARAMETER
		>//,
		//frx::gui::components::CreateVstForxEditor
	> Plugin;
	// create plugin
	try {
		Plugin *pl = new Plugin(audioMaster);
		frx::processing::VstForxPlug &vpl = *pl;
		vpl.setEffectPtr(pl);
		vpl.setMasterCallback((void*)audioMaster);
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
//-----------------------------------------------------------------------------
std::string getHomeDirectory() {
	return ".";
}


