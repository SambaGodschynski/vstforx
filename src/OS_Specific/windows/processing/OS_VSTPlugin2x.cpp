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
#include <fstream>
#include <boost/integer.hpp>
#include <sambag/com/Config.h>
#include <processing/Plugin.h>

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
void __checkArch(const std::string &filename) {
	// source: 
    // http://stackoverflow.com/questions/495244/how-can-i-test-a-windows-dll-to-determine-if-it-is-32bit-or-64bit#495305
	typedef boost::int_t<32>::exact Integer;
	typedef boost::uint_t<16>::exact Short;
	struct {
		unsigned char magic[2];
		unsigned char skip[58];
		Integer offset;
	} header;
	struct {
		unsigned char sig[2];
		unsigned char skip[2];
		Short machine;
	} pe;
	std::fstream f(filename.c_str(), std::ios_base::binary | std::ios_base::in);
	f.read((char*)&header, sizeof(header));

	if (header.magic[0]!='M' || header.magic[1]!='Z') {
		SAMBAG_LOG_WARN<<filename<<" no valid dll";
		f.close();
		return;
	}

	f.seekg(header.offset);
	f.read((char*)&pe, sizeof(pe));
	f.close();
	if (pe.sig[0]!='P' || pe.sig[1]!='E') {
		SAMBAG_LOG_WARN<<filename<<" missing PE header";
		return;
	}
	if (pe.machine==0x014c) { // 32
		#ifdef SAMBAG_64
			SAMBAG_THROW(
				frx::processing::PluginArchitectureMissmatch,
				"arch missmatch");
		#endif
	}
	if (pe.machine==0x8664 || pe.machine==0x0200) { // 64
		#ifdef SAMBAG_32
			SAMBAG_THROW(
				frx::processing::PluginArchitectureMissmatch,
				"arch missmatch");
		#endif
	}
}
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
		std::cout<<"Err "<<GetLastError()<<std::endl;
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
	__checkArch(filename);
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


