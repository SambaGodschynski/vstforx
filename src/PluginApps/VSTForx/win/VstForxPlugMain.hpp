#ifndef VSTFORXPLUGMAIN
#define VSTFORXPLUGMAIN

/*
	==============================================================
	*			      	VSTForx-Windows 						 *
	==============================================================
*/
#define MEMORY_TRACKING_STD 
// memory tracking :
#ifdef MEMORY_TRACKING_STD
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif
#ifdef MEMORY_TRACKING_VLD
#include "vld.h" // for detailed leak info
#endif



#endif
