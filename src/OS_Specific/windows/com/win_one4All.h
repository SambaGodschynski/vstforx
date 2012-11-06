/*
 * ===========================================================================================================
 * win_one4All.h
 *      Author: Johannes Unger
 * ===========================================================================================================
 */

// ONE4ALL WINDOWS
#ifndef ONE_4ALL_WIN_H
#define ONE_4ALL_WIN_H

#include <string>
#include "OS_Specific/MessageBoxDef.h"
#include <map>

//extern enum MessageBoxType;

namespace com {
//============================================================================================================
// MessageBox
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
MessageBoxReturn osMessageBox ( const std::string &title, const std::string &text, const MessageBoxType &type );
//------------------------------------------------------------------------------------------------------------
std::string osSelectDirectory ( const std::string &wndTitle, 
						    const std::string &startPath);
} // namespace com
#endif
