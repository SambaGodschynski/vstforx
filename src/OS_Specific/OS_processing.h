/*
 * ===========================================================================================================
 * OS_processing.h
 *      Author: Johannes Unger
 * ===========================================================================================================
 */

#ifndef OS_PROCESSING_H
#define OS_PROCESSING_H


#if defined(FRX_OS_WINDOWS)
#include "windows/processing/OS_VSTPlugin2x.h"
#elif defined(FRX_OS_MAC)
#include "mac/processing/OS_VSTPlugin2x.h"
#elif defined(FRX_OS_LINUX)
#include "linux/processing/OS_VSTPlugin2x.h"
#endif


#endif 
