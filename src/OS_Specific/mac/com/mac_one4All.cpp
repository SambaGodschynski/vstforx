/*
 * ===========================================================================================================
 * mac_one4All.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */

#ifdef FRX_OS_MAC

// ONE4ALL MAC
#include "mac_one4All.h"
#include "com/one4All.h"
#include "CoreFoundation/CoreFoundation.h"
#include "cfileselector.h"
//#include "CFUserNotification.h"
#include <sstream>

namespace com {
//------------------------------------------------------------------------------------------------------------
bool isPlugFilename ( const string &filename ) { 
	return Filename(filename).extension() == ".vst"; 
} 
//------------------------------------------------------------------------------------------------------------
bool isDirectory ( const string &filename ) {
	sambag::com::Location p(filename);
	boost::filesystem::file_status s = boost::filesystem::status(p); 
	// unter OSX sind plugs und apps verzeichnisse
	return is_directory (s) &&  p.extension() != ".vst" &&  p.extension() != ".app";
} 	
//------------------------------------------------------------------------------------------------------------
MessageBoxReturn osMessageBox ( const string &title, const string &text, const MessageBoxType &type ) {
	//convert the strings from char* to CFStringRef
    CFStringRef header_ref = CFStringCreateWithCString( NULL, title.c_str(), title.length() );
    CFStringRef message_ref = CFStringCreateWithCString( NULL, text.c_str(), text.length() );
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
//------------------------------------------------------------------------------------------------------------
std::string osSelectDirectory ( const std::string &wndTitle, const std::string &startPath)
{
	return "";
}
//============================================================================================================
// Klasse SysTimer.
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
pascal void idleTimerProc (EventLoopTimerRef inTimer, void *inUserData) {
	SysTimer *ptr = static_cast<SysTimer*>( inUserData );
	if (!ptr) return;
	ptr->callBack();
}
//-------------------------------------------------------------------------------------------------------------
void SysTimer::start() {
	static const float ONE_MS = kEventDurationSecond / 1000.0f;
	InstallEventLoopTimer ( GetCurrentEventLoop(), 
						    ONE_MS * (float)callTimeMS, //erstes warten
							ONE_MS * (float)callTimeMS, 
							idleTimerProc, 
							this, 
							&id);
}
//------------------------------------------------------------------------------------------------------------
void SysTimer::stop() {
	if (!id) return;
	RemoveEventLoopTimer (id);
	id = NULL;
}
} // namespace com

#endif //#ifdef FRX_OS_MAC

