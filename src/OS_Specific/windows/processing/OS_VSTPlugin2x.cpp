/*
 * ===========================================================================================================
 * OS_VSTPlugin2x.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#ifdef FRX_OS_WINDOWS

#include "OS_VSTPlugin2x.h" 
#include "processing/pluginTypes/NullAEffect.h"
#include <boost/tuple/tuple.hpp> // for boost::tie


// Callback Methode fuer VST-Plugin.
extern VstIntPtr VSTCALLBACK pluginCallToPlugNode (
	AEffect* effect, 
    VstInt32 opcode, 
	VstInt32 index, 
    VstIntPtr value, 
	void* ptr, 
    float opt
);

typedef AEffect* (*PluginEntryProc) (audioMasterCallback audioMaster);

namespace {
//------------------------------------------------------------------------------------------------------------
void unloadModule ( processing::OS_VSTPlugNode2x::Module module );
//------------------------------------------------------------------------------------------------------------
AEffect * getAEffect(  processing::OS_VSTPlugNode2x::Module module ) {
	using namespace processing;
	AEffect *aEff = NULL;
	PluginEntryProc mainProc = 0;
	mainProc = (PluginEntryProc)GetProcAddress ((OS_VSTPlugNode2x::Module)(module), "VSTPluginMain");
	if (!mainProc) {
		mainProc = (PluginEntryProc)GetProcAddress ((OS_VSTPlugNode2x::Module)(module), "main");
	}
	if (!mainProc) return NULL;
	__try {
		aEff = mainProc(pluginCallToPlugNode);
	} __except ( EXCEPTION_EXECUTE_HANDLER ){
		aEff = NULL;
	}
	return aEff;
}
//------------------------------------------------------------------------------------------------------------
static void loadModule ( const char *filename,  processing::OS_VSTPlugNode2x::Module *module, AEffect **aEff ) {
	*aEff = NULL;
	// Lade Plugin (.dll datei)
	__try {
		*module = LoadLibrary ( filename );
	}  __except ( EXCEPTION_EXECUTE_HANDLER ) {
		*module = NULL;
		return;
	}
	if ( ! (*module) ) return;
	__try {
		*aEff = getAEffect ( *module );
	} __except ( EXCEPTION_EXECUTE_HANDLER ) {
		*aEff = NULL;
		return;
	}
	if ( *aEff ) {
		if ( (*aEff)->magic != kEffectMagic ) *aEff = NULL;
	}
}
//------------------------------------------------------------------------------------------------------------
void unloadModule (  processing::OS_VSTPlugNode2x::Module module ) {
	if ( !module ) return;
	if ( !FreeLibrary ( module ) ) {
		throw com::ppiError::DllError ("dll unload failed.", __FILE__, __LINE__ );
	}
	module = NULL;
}
} // namepsace


namespace processing {
//============================================================================================================
// Klasse: OS_VSTPlugNode2x.
// Laed plugin handler.
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
OS_VSTPlugNode2x::HostCallBackOnInit OS_VSTPlugNode2x::callBkOnInit = HostCallBackOnInit(NULL, NULL);
//------------------------------------------------------------------------------------------------------------
com::Mutex OS_VSTPlugNode2x::onInitLock;
//------------------------------------------------------------------------------------------------------------
int OS_VSTPlugNode2x::shellPlugIdOnInit = 0;
//------------------------------------------------------------------------------------------------------------
bool OS_VSTPlugNode2x::loadModule( const HostCallBackOnInit &_callBkOnInit ) {
	if ( moduleLocation.length() == 0 ) return false;
	std::string filename;
	boost::tie(filename, shellPlugId) = com::extractVSTPluginFilename(moduleLocation);
	{ // lock scope
		TRY_TO_LOCK_TIMED (onInitLock)
		shellPlugIdOnInit = shellPlugId;
		OS_VSTPlugNode2x::callBkOnInit = _callBkOnInit;
		::loadModule ( filename.c_str(), &module, &aEff );
		OS_VSTPlugNode2x::callBkOnInit = HostCallBackOnInit(NULL, NULL);
		shellPlugIdOnInit = 0;
	}
	if ( aEff ) {
		return true;
	}
	aEff = &nullAEff;
	::unloadModule ( module );
	return false;
}
//------------------------------------------------------------------------------------------------------------
bool OS_VSTPlugNode2x::unloadModule() {
	try {
		::unloadModule ( module );
	} catch (...) {
		return false;
	}
	return true;
}
} //namespace

#endif //#ifdef FRX_OS_WINDOWS


