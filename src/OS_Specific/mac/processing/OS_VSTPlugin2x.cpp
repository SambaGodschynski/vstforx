#include "OS_VSTPlugin2x.h" 
#include "processing/pluginTypes/NullAEffect.h"


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
typedef processing::OS_VSTPlugNode2x::Module Module;
//------------------------------------------------------------------------------------------------------------
void unloadModule ( Module module ) ;
//------------------------------------------------------------------------------------------------------------
AEffect * getAEffect( Module module ) {
	PluginEntryProc mainProc = NULL;
	mainProc = (PluginEntryProc)CFBundleGetFunctionPointerForName ( module, CFSTR("VSTPluginMain"));
	if (!mainProc)
		mainProc = (PluginEntryProc)CFBundleGetFunctionPointerForName ( module, CFSTR("main_macho"));	
	if ( !mainProc ) return NULL;
	AEffect *aEff = NULL; 
	aEff = mainProc ( pluginCallToPlugNode );
	return aEff;
}
//------------------------------------------------------------------------------------------------------------
static void loadModule ( const char *filename, Module *module, AEffect **aEff ) {
	TOLOG ("::loadModule()");
	*module = NULL;
	*aEff = NULL;
	// build carbon string
	CFStringRef fileNameString = CFStringCreateWithCString (NULL, filename, kCFStringEncodingUTF8);
	if (fileNameString == 0) return;
	// get url
	CFURLRef url = CFURLCreateWithFileSystemPath (NULL, fileNameString, kCFURLPOSIXPathStyle, false);
	CFRelease (fileNameString);
	if (url == 0) return;
	// get bundle ref
	TOLOG ("getBundleRef");
	*module = CFBundleCreate (NULL, url);
	CFRelease (url);
	if (*module) {
		/*TOLOG ("CFBundleLoadExecutable");
		if ( !CFBundleLoadExecutable (*module)  ) {
			TOLOG ("CFBundleLoadExecutable FAILED!");
			*module = NULL;
			return;
		}*/
	} else {
		TOLOG ("getBundleRef FAILED!");
		return;
	}

	TOLOG ("getAEffect()");
	*aEff = getAEffect ( *module );
	
	if ( *aEff ) {
		if ( (*aEff)->magic != kEffectMagic ) *aEff = NULL;
	}
}
//------------------------------------------------------------------------------------------------------------
void unloadModule ( Module module ) {
	if ( ! module ) return;
	//CFBundleUnloadExecutable (module); // verursacht abstuerze da unload(fuer alle) erzwungen wird
	CFRelease (module);
	module = NULL;
}
} // namepsace


namespace processing {
//============================================================================================================
// Klasse: OS_VSTPlugNode2x.
// Laed plugin handler.
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
OS_VSTPlugNode2x::HostCallBackOnInit OS_VSTPlugNode2x::callBkOnInit = HostCallBackOnInit( NULL, NULL);
//------------------------------------------------------------------------------------------------------------
com::Mutex OS_VSTPlugNode2x::onInitLock;
//------------------------------------------------------------------------------------------------------------
bool OS_VSTPlugNode2x::loadModule( const HostCallBackOnInit &_callBkOnInit ) {
	if ( moduleLocation.length() == 0 ) return false;
	{ // lock scope
		TRY_TO_LOCK_TIMED (onInitLock)
		OS_VSTPlugNode2x::callBkOnInit = _callBkOnInit;
		::loadModule ( moduleLocation.c_str(), &module, &aEff );
		OS_VSTPlugNode2x::callBkOnInit = HostCallBackOnInit(NULL, NULL);
	}
	if ( aEff ) return true;
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



