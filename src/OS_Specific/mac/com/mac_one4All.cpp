/*
 * ===========================================================================================================
 * mac_one4All.cpp
 *      Author: Johannes Unger
 * ============================================================================
 */

#ifdef FRX_OS_MAC

// ONE4ALL MAC
#include "mac_one4All.h"
#include "com/one4All.h"
#include "CoreFoundation/CoreFoundation.h"
#include <sstream>
#include "OS_Specific/mac/CocoaImpl.h"


namespace com {
const char * FRX_VST_EXT = ".vst";
const char * FRX_LUA_EXT = ".lua";
const char * FRX_APP_EXT = ".app";
//-----------------------------------------------------------------------------
bool isPlugFilename ( const std::string &filename ) {
    std::string ext = Filename(filename).extension().string();
	return ext == std::string(FRX_VST_EXT) ||
           ext == std::string(FRX_LUA_EXT);
} 
//-----------------------------------------------------------------------------
bool isDirectory ( const std::string &filename ) {
	sambag::com::Location p(filename);
	boost::filesystem::file_status s = boost::filesystem::status(p); 
	// in osx (vst) bundles are directories
    // but we want to treat them as file
    std::string ext = Filename(filename).extension().string();
	return is_directory (s) &&
           ext != std::string(FRX_VST_EXT) &&
           ext != std::string(FRX_APP_EXT);
} 	
//-----------------------------------------------------------------------------
MessageBoxReturn osMessageBox ( const std::string &title, const std::string &text, const MessageBoxType &type ) {
    CFStringRef header_ref = CFStringCreateWithCString( NULL, title.c_str(), kCFStringEncodingUTF8 );
    CFStringRef message_ref = CFStringCreateWithCString( NULL, text.c_str(), kCFStringEncodingUTF8 );
    CFStringRef btn01 = NULL;
	CFStringRef btn02 = NULL;
	CFOptionFlags result;  //result code from the message box
	
	int level = 0;
	switch ( type ) {
		case com::MSG_HINT :
			level = kCFUserNotificationCautionAlertLevel;
			break;
		case com::MSG_QUESTION :
			level =  kCFUserNotificationCautionAlertLevel;
			btn01 = CFSTR("yes");
			btn02 = CFSTR("no");
			break;
		case com::MSG_ALERT :
			level =  kCFUserNotificationNoteAlertLevel;
			break;
	}
	
    //launch the message box
    CFUserNotificationDisplayAlert(
        0, // no timeout
		level, //change it depending message_type flags ( MB_ICONASTERISK.... etc.)
		NULL, //icon url, use default, you can change it depending message_type flags
		NULL, //not used
		NULL, //localization of strings
		header_ref, //header text 
		message_ref, //message text
		btn01, //default "ok" text in button
		btn02, //alternate button title
		NULL, //other button title, null--> no other button
		&result //response flags
	);
	
    //Clean up the strings
    CFRelease( header_ref );
    CFRelease( message_ref );
	
	if ( type == com::MSG_QUESTION ) {
		//Convert the result
		if( result == (CFOptionFlags)kCFUserNotificationDefaultResponse )
			return MSG_RET_YES;
		else
			return MSG_RET_NO;
	}
	return MSG_RET_NONE;
}
//-----------------------------------------------------------------------------
std::string osSelectDirectory ( const std::string &wndTitle, const std::string &startPath, void *parentWindow)
{
	return frx::com::CocoaImpl::selectDirectory(wndTitle, startPath);
}
//-----------------------------------------------------------------------------
std::string osSelectFile ( const std::string &wndTitle, const std::string &startPath, void *parentWindow)
{
	return frx::com::CocoaImpl::selectFile(wndTitle, startPath);
}
//-----------------------------------------------------------------------------
void startProcess(const char *path, int argc, const char **argv) {
    frx::com::CocoaImpl::startProcess(path, argc, argv);
}
//-----------------------------------------------------------------------------
void osOpenLink (const std::string &url) {
    frx::com::CocoaImpl::openLink(url);
}
//-----------------------------------------------------------------------------
void osShowInputTextDlg(const std::string &title,
    std::string &inOutTxt, void *parentWnd)
{
    frx::com::CocoaImpl::showInputTextDlg(title, inOutTxt);
}
} // namespace com

#endif //#ifdef FRX_OS_MAC

