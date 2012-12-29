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
int main(int narg, char **args) {
	std::cout<<"hello dave.."<<std::endl;
	setUp();
	if (!scriptCtrl) {
		std::cout<<"creating script ctrl failed!"<<std::endl;
		return -1;
	}
	scriptCtrl->addScript( "frxOpenPlugin()" );
	scriptCtrl->addScript( "frxOpenEditor()" );
	scriptCtrl->start();
	sambag::disco::components::Window::startMainLoop();
	scriptCtrl->join();
	tearDown();
	return 0;
}
