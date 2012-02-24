/*
 * ===========================================================================================================
 * OS_gui.h
 *      Author: Johannes Unger
 * ===========================================================================================================
 */

#ifndef OS_GUI_H
#define OS_GUI_H

#include "OS_Specific/OS_Specific.h"

#ifdef OS_WINDOWS
#include "windows/gui/win_Window.h"
#else ifdef OS_MAC
#include "mac/gui/mac_Window.h"

#endif


#endif 