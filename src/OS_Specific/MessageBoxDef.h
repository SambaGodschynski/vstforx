/*
 * ===========================================================================================================
 * MessageBoxDef.h
 *      Author: Johannes Unger
 * ===========================================================================================================
 */

#ifndef MESSAGEBOX_DEF_H
#define MESSAGEBOX_DEF_H

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
// Message Box
} //namespace com

#endif