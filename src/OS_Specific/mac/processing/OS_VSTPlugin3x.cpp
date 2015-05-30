/*
 * ===========================================================================================================
 * OS_VSTPlugin3x.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */

#ifdef FRX_OS_MAC

#include "OS_VSTPlugin3x.h"
#include <sambag/com/Config.h>


namespace frx {
    extern void checkArchitecture(CFBundleRef module);
}

namespace {
typedef processing::OS_VSTPlugNode3x::Module Module;
typedef bool (*BundleEntryPtr)(CFBundleRef);
typedef bool (*BundleExitPtr)(void);

//------------------------------------------------------------------------------------------------------------
void unloadModule ( Module module ) ;
//------------------------------------------------------------------------------------------------------------
void loadModule ( const char *path, Module *module,  Steinberg::IPluginFactory **factory)
{
    CFURLRef url = path ? CFURLCreateFromFileSystemRepresentation (0, (const UInt8*)path, strlen (path), true) : 0;
	if (!url)
	{
        return;
    }
    GetFactoryProc entryProc = NULL;
    *module = CFBundleCreate (NULL, url);
    Module libHandle = *module;
    if (libHandle)
    {
        frx::checkArchitecture((CFBundleRef)libHandle);
        CFErrorRef errorRef = 0;
        if (CFBundleLoadExecutableAndReturnError ((CFBundleRef)libHandle, &errorRef))
        {
            BundleEntryPtr bundleEntry = (BundleEntryPtr)CFBundleGetFunctionPointerForName ((CFBundleRef)libHandle, CFSTR("bundleEntry"));
            if (bundleEntry)
            {
                if (bundleEntry ((CFBundleRef)libHandle) == false)
                {
                    CFRelease ((CFBundleRef)libHandle);
                    libHandle = 0;
                }
            }
            if (libHandle)
                entryProc = (GetFactoryProc)CFBundleGetFunctionPointerForName ((CFBundleRef)libHandle, CFSTR("GetPluginFactory"));
        }
        else if (errorRef)
        {
            CFStringRef failureString = CFErrorCopyFailureReason (errorRef);
            if (failureString)
            {
                CFShow (failureString);
                CFRelease (failureString);
            }
            CFRelease (errorRef);
            throw std::runtime_error(std::string("failed to load ") + path);
        }
    }
    // create factory
	if (entryProc) {
		*factory = entryProc ();
    }
    CFRelease (url);
}
//------------------------------------------------------------------------------------------------------------
void unloadModule ( Module libHandle ) {
    // free library
	if (libHandle)
	{
        BundleExitPtr bundleExit = (BundleExitPtr)CFBundleGetFunctionPointerForName ((CFBundleRef)libHandle, CFSTR("bundleExit"));
		if (bundleExit) {
			bundleExit ();
        }
		CFRelease ((CFBundleRef)libHandle);
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

#endif // #ifdef FRX_OS_MAC


