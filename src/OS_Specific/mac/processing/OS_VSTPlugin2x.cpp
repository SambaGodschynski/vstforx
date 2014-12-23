/*
 * ===========================================================================================================
 * OS_VSTPlugin2x.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */

#ifdef FRX_OS_MAC

#include "OS_VSTPlugin2x.h" 
#include "processing/pluginTypes/NullAEffect.h"
#include <sambag/com/Config.h>
#include <processing/pluginTypes/PluginImpl.hpp>

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

namespace frx {
    //--------------------------------------------------------------------------------------------------------
    long getCurrentArch() {
    #ifdef SAMBAG_32
        return kCFBundleExecutableArchitectureI386;
    #elif defined SAMBAG_64
        return kCFBundleExecutableArchitectureX86_64;
    #else
        #error "VSTForx: architecture missmatch"
    #endif
    }
    //--------------------------------------------------------------------------------------------------------
    bool checkArchitecture(CFBundleRef module) {
        CFArrayRef archs = CFBundleCopyExecutableArchitectures(module);
        if (!archs) {
            return false;
        }
        long current = getCurrentArch();
        for (int i=0; i<CFArrayGetCount(archs); ++i) {
            CFNumberRef archCode = (CFNumberRef)CFArrayGetValueAtIndex(archs, i);
            long arch = 0;
            CFNumberGetValue(archCode, kCFNumberLongType, &arch);
            if (arch==current) {
                CFRelease(archs);
                return true;
            }
        }
        CFRelease(archs);
        return false;
    }
}

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
void loadModule ( const char *filename, Module *module, AEffect **aEff ) {
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
	*module = CFBundleCreate (NULL, url);
	CFRelease (url);
	if (*module) {
        if (!frx::checkArchitecture(*module)) {
            SAMBAG_THROW(
                frx::processing::PluginArchitectureMissmatch,
                "Plugin architecture missmatch."
        );
    }
	} else {
		TOLOG ("getBundleRef FAILED!");
		return;
	}

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
int OS_VSTPlugNode2x::shellPlugIdOnInit = 0;
//------------------------------------------------------------------------------------------------------------
com::Mutex OS_VSTPlugNode2x::onInitLock;
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
	if ( aEff ) 
		return true;
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

#endif // #ifdef FRX_OS_MAC


