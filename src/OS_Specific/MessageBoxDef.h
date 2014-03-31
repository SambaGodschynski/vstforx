/*
 * ===========================================================================================================
 * MessageBoxDef.h
 *      Author: Johannes Unger
 * ===========================================================================================================
 */

#ifndef MESSAGEBOX_DEF_H
#define MESSAGEBOX_DEF_H

#include <string>

namespace com {
//------------------------------------------------------------------------------------------------------------
// Message Box
enum MessageBoxType {
	MSG_HINT,
	MSG_QUESTION,
	MSG_ALERT
};
enum MessageBoxReturn {
	MSG_RET_NONE,
	MSG_RET_YES,
	MSG_RET_NO
};
//------------------------------------------------------------------------------------------------------------
//============================================================================================================
// MessageBox
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
MessageBoxReturn osMessageBox ( const std::string &title, const std::string &text, const MessageBoxType &type );
//------------------------------------------------------------------------------------------------------------
std::string osSelectDirectory ( const std::string &wndTitle, const std::string &startPath, void *parent);
//------------------------------------------------------------------------------------------------------------
std::string osSelectFile ( const std::string &wndTitle, const std::string &startPath, void *parent);
//------------------------------------------------------------------------------------------------------------
void osOpenLink (const std::string &url);
//------------------------------------------------------------------------------------------------------------
 void osShowInputTextDlg(const std::string &title, std::string &inOutTxt, void *parentWnd);
// Message Box
} //namespace com

#endif
