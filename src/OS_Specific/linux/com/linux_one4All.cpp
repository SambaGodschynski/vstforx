/*
 * ===========================================================================================================
 * mac_one4All.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */

#ifdef FRX_OS_LINUX

#include <string>
#include <boost/filesystem.hpp>
#include "com/one4All.h"

namespace com {
//------------------------------------------------------------------------------------------------------------
std::string getRootDirectory() {
	return ".";
}
//------------------------------------------------------------------------------------------------------------
bool isPlugFilename ( const std::string &filename ) { 
	return false; 
} 
//------------------------------------------------------------------------------------------------------------
bool isDirectory ( const std::string &filename ) {
	sambag::com::Location p(filename);
	boost::filesystem::file_status s = boost::filesystem::status(p); 
	return is_directory (s);
} 	
//------------------------------------------------------------------------------------------------------------
MessageBoxReturn osMessageBox ( const std::string &title, const std::string &text, const MessageBoxType &type ) {
	return MSG_RET_NONE;
}
//------------------------------------------------------------------------------------------------------------
std::string osSelectDirectory ( const std::string &wndTitle, const std::string &startPath, void *parentWindow)
{
	return "";
}
} // namespace com

#endif //#ifdef FRX_OS_LINUX

