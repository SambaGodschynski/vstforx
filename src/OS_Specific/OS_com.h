/*
 * ============================================================================
 * OS_com.h
 *      Author: Johannes Unger
 * ============================================================================
 */

#ifndef OS_COM_H
#define OS_COM_H

#include "OS_Specific/MessageBoxDef.h"
#include <string>

#if defined(FRX_OS_WINDOWS)
#include "windows/com/win_one4All.h"
#elif defined(FRX_OS_MAC)
#include "mac/com/mac_one4All.h"
#elif defined(FRX_OS_LINUX)
#include "linux/com/linux_one4All.h"
#endif


namespace com {
//-----------------------------------------------------------------------------
// testet ob filename == plugfilename ( zb.: *.dll )
extern bool isPlugFilename ( const string &filename );
//-----------------------------------------------------------------------------
extern bool isDirectory ( const string &path );
//-----------------------------------------------------------------------------
extern std::string getRootDirectory();
} //namespace
#endif 
