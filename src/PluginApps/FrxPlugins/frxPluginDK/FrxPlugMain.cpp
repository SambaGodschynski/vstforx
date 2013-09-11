/*
 * FrxPlugMain.cpp
 * FrxPlugin Main Entry
 *
 *  Created on: Tue Sep  10 12:58:07 2013
 *      Author: Johannes Unger
 */

#include "FrxPlugInterface.h"
///////////////////////////////////////////////////////////////////////////////
extern RawFrxPlugin* createFrxPluginInstance (FrxHostCallback host);

extern "C" {
#if defined (__GNUC__) && ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1)))
	#define FRX_EXPORT	__attribute__ ((visibility ("default")))
#else
	#define FRX_EXPORT
#endif

//-----------------------------------------------------------------------------
/** Prototype of the export function main */
//-----------------------------------------------------------------------------
FRX_EXPORT RawFrxPlugin* FrxPluginMain (FrxHostCallback host)
{
    // Create the AudioEffect
	return createFrxPluginInstance(host);
}

} // extern "C"

//-----------------------------------------------------------------------------
#if WIN32
#include <windows.h>
void* frxPluginInstance;

extern "C" {
BOOL WINAPI DllMain (HINSTANCE hInst, DWORD dwReason, LPVOID lpvReserved)
{
	frxPluginInstance = hInst;
	return 1;
}
} // extern "C"
#endif
