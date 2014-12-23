/*
 * ===========================================================================================================
 * OS_VSTPlugin2x.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#ifdef FRX_OS_WINDOWS

#include "OS_VSTPlugin3x.h" 

namespace {
//------------------------------------------------------------------------------------------------------------
void unloadModule ();
//------------------------------------------------------------------------------------------------------------
static void loadModule ( const char *filename,  processing::OS_VSTPlugNode3x::Module *module ) {
	//*aEff = NULL;
	//// Lade Plugin (.dll datei)
	//__try {
	//	*module = LoadLibrary ( filename );
	//	std::cout<<"Err "<<GetLastError()<<std::endl;
	//}  __except ( EXCEPTION_EXECUTE_HANDLER ) {
	//	*module = NULL;
	//	return;
	//}
	//if ( ! (*module) ) return;
	//__try {
	//	*aEff = getAEffect ( *module );
	//} __except ( EXCEPTION_EXECUTE_HANDLER ) {
	//	*aEff = NULL;
	//	return;
	//}
	//if ( *aEff ) {
	//	if ( (*aEff)->magic != kEffectMagic ) *aEff = NULL;
	//}
}
//------------------------------------------------------------------------------------------------------------
void unloadModule (  processing::OS_VSTPlugNode3x::Module module ) {
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
bool OS_VSTPlugNode3x::loadModule() {
	//if ( moduleLocation.length() == 0 ) return false;
	//std::string filename;
	//boost::tie(filename, shellPlugId) = com::extractVSTPluginFilename(moduleLocation);
	//__checkArch(filename);
	//{ // lock scope
	//	TRY_TO_LOCK_TIMED (onInitLock)
	//	shellPlugIdOnInit = shellPlugId;
	//	OS_VSTPlugNode2x::callBkOnInit = _callBkOnInit;
	//	::loadModule ( filename.c_str(), &module, &aEff );
	//	OS_VSTPlugNode2x::callBkOnInit = HostCallBackOnInit(NULL, NULL);
	//	shellPlugIdOnInit = 0;
	//}
	//if ( aEff ) {
	//	return true;
	//}
	//aEff = &nullAEff;
	//::unloadModule ( module );
	return false;
}
//------------------------------------------------------------------------------------------------------------
bool OS_VSTPlugNode3x::unloadModule() {
	try {
		::unloadModule ( module );
	} catch (...) {
		return false;
	}
	return true;
}
} //namespace

#endif //#ifdef FRX_OS_WINDOWS


