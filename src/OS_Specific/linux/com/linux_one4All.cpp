/*
 * ===========================================================================================================
 * mac_one4All.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */

#ifdef FRX_OS_LINUX

#include <string>
#include <filesystem>
#include "com/one4All.h"
#include <sambag/disco/components/Window.hpp>

// Required by VSTPlugin2x.cpp and VST2xImpl.cpp: return native window handle
// for embedding VST editor. On Linux this is unused in standalone mode.
void * __getHandlerForVstPlugins_(void *ptr) {
    return ptr;
}

namespace frx { namespace gui {
// No-op on Linux standalone: host will not try to resize our window.
namespace sdc = sambag::disco::components;
void osHostWontResizeFix(sdc::Window::Ptr, int, int) {}
}} // namespace frx::gui

namespace com {
const char * FRX_VST_EXT  = ".so";
const char * FRX_VST3_EXT = ".vst3";
const char * FRX_LUA_EXT  = ".lua";
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
	std::filesystem::file_status s = std::filesystem::status(p); 
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
//------------------------------------------------------------------------------------------------------------
std::string osSelectFile(const std::string &, const std::string &, void *) {
    return "";
}
//------------------------------------------------------------------------------------------------------------
std::string osSaveFile(const std::string &, const std::string &, void *) {
    return "";
}
//------------------------------------------------------------------------------------------------------------
void osOpenLink(const std::string &) {}
//------------------------------------------------------------------------------------------------------------
void osShowInputTextDlg(const std::string &, std::string &, void *) {}
} // namespace com

#endif //#ifdef FRX_OS_LINUX

