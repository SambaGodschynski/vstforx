/*
 * ===========================================================================================================
 * OS_VSTPlugin2x.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#ifdef FRX_OS_WINDOWS

#include "OS_VSTPlugin3x.h" 

#include <windows.h>
#include <conio.h>
extern "C"
{
	typedef bool (PLUGIN_API *InitModuleProc) ();
	typedef bool (PLUGIN_API *ExitModuleProc) ();
}
static const Steinberg::FIDString kInitModuleProcName = "InitDll";
static const Steinberg::FIDString kExitModuleProcName = "ExitDll";

namespace {
//------------------------------------------------------------------------------------------------------------
void unloadModule ();
//------------------------------------------------------------------------------------------------------------
static void loadModule (const char *filename,  
	processing::OS_VSTPlugNode3x::Module *module, Steinberg::IPluginFactory **factory ) 
{
	GetFactoryProc entryProc = NULL;
	*module = ::LoadLibraryA (filename);
	processing::OS_VSTPlugNode3x::Module libHandle = *module;
	if (libHandle)
	{
		InitModuleProc initProc = (InitModuleProc)::GetProcAddress ((HMODULE)libHandle, kInitModuleProcName);
		if (initProc)
		{
			if (initProc () == false)
			{
				FreeLibrary ((HMODULE)libHandle);
				libHandle = 0;	
			}
		}
	}
	if (libHandle) {
		entryProc = (GetFactoryProc)::GetProcAddress ((HMODULE)libHandle, "GetPluginFactory");
	}
	// create factory
	if (entryProc) {
		*factory = entryProc ();
	}
}
//------------------------------------------------------------------------------------------------------------
void unloadModule (  processing::OS_VSTPlugNode3x::Module module ) {
	if ( !module ) {
		return;
	}
	ExitModuleProc exitProc = (ExitModuleProc)::GetProcAddress ((HMODULE)module, kExitModuleProcName);
	if (exitProc) {
		exitProc ();
	}
	
	::FreeLibrary ((HMODULE)module);

	
	if ( !FreeLibrary ( module ) ) {
		throw com::ppiError::DllError ("dll unload failed.", __FILE__, __LINE__ );
	}
	
}
} // namepsace


namespace processing {
//============================================================================================================
// Klasse: OS_VSTPlugNode2x.
// Laed plugin handler.
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
bool OS_VSTPlugNode3x::loadModule() {
	if ( moduleLocation.length() == 0 ) {
        return false;
    }
    Module moduleWrapper[] = { NULL };
    Steinberg::IPluginFactory *factoryWrapper[] = { NULL };
    ::loadModule(moduleLocation.c_str(), &moduleWrapper[0], &factoryWrapper[0]);
    this->module = moduleWrapper[0];
    this->factory = factoryWrapper[0];
	return true;
}
//------------------------------------------------------------------------------------------------------------
bool OS_VSTPlugNode3x::unloadModule() {
	try {
    	// release factory
        if (factory) {
            factory->release ();
        }
        factory = NULL;
        if (module) {
            ::unloadModule ( module );
        }
        module = NULL;
	} catch (...) {
		return false;
	}
	return true;
}
} //namespace

#endif //#ifdef FRX_OS_WINDOWS


