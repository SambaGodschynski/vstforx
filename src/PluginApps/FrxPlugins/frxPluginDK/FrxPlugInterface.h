/*
 * FrxPlugMain.cpp
 * FrxPlugin Main Entry
 *
 *  Created on: Tue Sep  10 12:58:07 2013
 *      Author: Johannes Unger
 */

#ifndef _FRX_PLUGIN_DK_INTERFACE_
#define _FRX_PLUGIN_DK_INTERFACE_

#include <sambag/com/Config.h>

#ifdef WIN32
  #define FRX_CALLBACK __cdecl
#else
  #define FRX_CALLBACK
#endif

#ifdef WIN32
  #ifdef SAMBAG_64
    typedef __int64 FrxInteger;                       
  #else
    typedef int FrxInteger;
  #endif
#else
#include <stdint.h>
  #ifdef SAMBAG_64
    typedef int64_t FrxInteger;
  #else
    typedef int32_t FrxInteger;
  #endif
#endif
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------
// Misc. Definition
//------------------------------------------------------------------------------
struct RawFrxPlugin;

typedef	FrxInteger (FRX_CALLBACK *FrxHostCallback) 
  (RawFrxPlugin *effect, 
   FrxInteger opcode, 
   FrxInteger index, 
   FrxInteger value,
   void *ptr,
   float opt);
typedef FrxInteger (FRX_CALLBACK *FrxPluginDispatcher) 
   (RawFrxPlugin *effect,
    FrxInteger opcode, 
    FrxInteger index, 
    FrxInteger value, 
    void *ptr, 
    float opt);
typedef void (FRX_CALLBACK *ProcessFunc) 
    (RawFrxPlugin *effect, double **inputs, double **outputs, FrxInteger sampleFrames);
//==============================================================================
struct RawFrxPlugin
{
//==============================================================================
  FrxPluginDispatcher dispatcher;
  ProcessFunc process;
  
  FrxInteger numInputs;
  FrxInteger numOutputs;
  
  FrxInteger flags;	
  FrxInteger initialDelay;
  
  void* object; // the FrxPluginWrapper<> object
  FrxInteger version;
};

enum FrxEffectFlags
{
  FrxHasEditor     = 1 << 0,
};

//-------------------------------------------------------------------------------------------------------
/** Basic dispatcher Opcodes (Host to Plug-in) */
//-------------------------------------------------------------------------------------------------------
enum FrxEffectDispatcherCodes
{
	FrxEffectInit = 0,
	FrxEffectDestroy,

	FrxEffectGetParamLabel,   ///< [ptr]: char buffer for parameter label, limited to [value]
	Frxeffectgetparamdisplay, ///< [ptr]: char buffer for parameter label, limited to [value]
	FrxEffectGetParamName,    ///< [ptr]: char buffer for parameter label, limited to [value]
	FrxEffectSetSampleRate,   ///< [opt]: new sample rate for audio processing
	FrxEffectSetBlockSize,    ///< [value]: new maximum block size for audio processing
	FrxEffectEditOpen,
	FrxEffectEditClose,
	FrxEffectGetChunk,        ///< [ptr]: void** for chunk data address
	FrxEffectSetChunk,	  ///< [ptr]: chunk data [value]: byte size
 
	effNumOpcodes		
};

//-------------------------------------------------------------------------------------------------------
/** Basic dispatcher Opcodes (Plug-in to Host) */
//-------------------------------------------------------------------------------------------------------
enum FrxHostDispatcherCodes
{
	audioMasterVersion,   ///< [return value]: Host version
};

#endif  // _FRX_PLUGIN_DK_INTERFACE_
