/*
 * ===========================================================================================================
 * OS_com.h
 *      Author: Johannes Unger
 * ===========================================================================================================
 */

#ifndef OS_COM_H
#define OS_COM_H

#include "OS_Specific/OS_Specific.h"
#include <string>

#ifdef OS_WINDOWS
#include "windows/com/win_one4All.h"
#else ifdef OS_MAC
#include "mac/com/mac_one4All.h"
#endif

struct VstFileType;

class AudioEffectX;

namespace com {
//--------------------------------------------------------------------------------------------------------
// testet ob filename == plugfilename ( zb.: *.dll )
extern bool isPlugFilename ( const string &filename );
//--------------------------------------------------------------------------------------------------------
extern bool isDirectory ( const string &path );
} //namespace
#endif 