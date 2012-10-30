#include "VstForxPlugMain.hpp"
#include <processing/VstForxPlug.hpp>
#include <gui/components/VstForxEditor.hpp>
#include <sambag/dsp/VST2xPluginWrapper.hpp>
#include <string>
#include <audioeffectx.h>
#include <com/Settings.h>

std::string getHomeDirectory();

//-----------------------------------------------------------------------------
AudioEffect * createEffectInstance ( audioMasterCallback audioMaster ) {
#if _WIN32
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF ); //VS memory tracking
#endif

	// init settings
	::com::initSettings(getHomeDirectory());
	// load plugin
	using namespace sambag::dsp::vst;
	// settingup plugin
	typedef VST2xPluginWrapper<
		frx::processing::VstForxPlug, // Processor
		'frxr', // uid
		sambag::dsp::StdPluginTraits<
			2,2,false,::com::Settings::PROGRAM_PARAMETER
		>, 
		frx::gui::components::CreateVstForxEditor
	> Plugin;
	// create plugin
	try {
		Plugin *pl = new Plugin(audioMaster);
		frx::processing::VstForxPlug &vpl = *pl;
		vpl.setEffectPtr(pl);
		vpl.setMasterCallback(audioMaster);
		return pl;
	} catch(...) {
		return NULL;
	}
}
//-----------------------------------------------------------------------------
extern void *hInstance; // @see vstsdk2.4::vstplugmain.cpp
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
	return res + "/";
}