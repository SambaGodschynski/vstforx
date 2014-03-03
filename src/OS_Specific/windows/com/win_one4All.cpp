/*
 * ===========================================================================================================
 * win_one4All.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */

#ifdef FRX_OS_WINDOWS

// ONE4ALL WINDOWS
#include "win_one4All.h"
#include "com/one4All.h"
#include <Shlobj.h>
#include <sstream>
#include <windows.h>
#include <exception>
#include <sambag/com/Thread.hpp>
extern void* hInstance;

namespace com {
namespace {
	void __startProcess(const std::string &path, const std::string &argv) {
		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		ZeroMemory( &si, sizeof(si) );
		si.cb = sizeof(si);
		ZeroMemory( &pi, sizeof(pi) );
		// Start the child process. 
		if( !CreateProcess( path.c_str(),   // module name
			const_cast<LPSTR>(argv.c_str()),        // Command line
			NULL,           // Process handle not inheritable
			NULL,           // Thread handle not inheritable
			FALSE,          // Set handle inheritance to FALSE
			0,              // No creation flags
			NULL,           // Use parent's environment block
			NULL,           // Use parent's starting directory 
			&si,            // Pointer to STARTUPINFO structure
			&pi )           // Pointer to PROCESS_INFORMATION structure
		) 
		{
			SAMBAG_LOG_ERR<<"starting process "<<path<<" failed.("<<GetLastError()<<")";
		}
		// Wait until child process exits.
		WaitForSingleObject( pi.hProcess, INFINITE );
		// Close process and thread handles. 
		CloseHandle( pi.hProcess );
		CloseHandle( pi.hThread );
	}
}
//------------------------------------------------------------------------------------------------------------
void startProcess(const char *path, int argc, const char **argv) {
	std::stringstream ss;
	if (argc>0) {
		ss<<argv[0];
		for (int i=1; i<argc; ++i) {
			ss<<" "<<argv[i];
		}
	}
	boost::thread(
		boost::bind(&__startProcess, std::string(path), ss.str())
	);
}
//------------------------------------------------------------------------------------------------------------
std::string getRootDirectory() {
	using namespace com;
	std::string home_dir;
	const size_t N = 512; 
	char _d[N];
	DWORD r = GetModuleFileName ( NULL, &_d[0], N );
	com::Filename f( _d  );
	if ( is_regular_file(f) ) home_dir = f.remove_filename().string();
	else home_dir = f.string();
	return home_dir;
}
//------------------------------------------------------------------------------------------------------------
bool isPlugFilename ( const std::string &filename ) { 
	return Filename(filename).extension() == ".dll"; 
} 
//------------------------------------------------------------------------------------------------------------
bool isDirectory ( const std::string &filename ) { 
	boost::filesystem::file_status s = boost::filesystem::status( sambag::com::Location(filename) ); 
	return is_directory (s);
} 
//------------------------------------------------------------------------------------------------------------
MessageBoxReturn osMessageBox ( const std::string &title, const std::string &text, const MessageBoxType &type ) {
	int ret = 0;
	UINT flags = MB_TASKMODAL;
	switch ( type ){ 
		case MSG_HINT :
			::MessageBox ( NULL, text.c_str(), title.c_str(), flags | MB_OK );
			return MSG_RET_NONE;
		case MSG_QUESTION :
			ret = ::MessageBox ( NULL, text.c_str(), title.c_str(), flags | MB_YESNO | MB_ICONQUESTION );
			switch ( ret ) {
				case IDYES : return MSG_RET_YES; 
				case IDNO : return MSG_RET_NO;
				default: return MSG_RET_NONE;
			}
		case MSG_ALERT :
			::MessageBox ( NULL, text.c_str(), title.c_str(), flags | MB_ICONWARNING );
			return MSG_RET_NONE;
	}
	return MSG_RET_NONE;
}
//============================================================================================================
//	globale string: SHBrowseForFolder startpfad ueber BrowseCallbackProc
//============================================================================================================
std::string _startPath;
//============================================================================================================
//	Methode BrowseCallbackProc:
//============================================================================================================
int CALLBACK BrowseCallbackProc(
    HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
    switch(uMsg) {
		case BFFM_INITIALIZED:
			SendMessage(hwnd, BFFM_SETSELECTION, TRUE, reinterpret_cast<LPARAM>( _startPath.c_str() ) );
			break;
	}
	return 0;
}
//--------------------------------------------------------------------------------------------------------
std::string osSelectDirectory ( const std::string &wndTitle, 
						    const std::string &startPath,
							void *parentWindow)
{
	std::string ret;
	BROWSEINFO bi = { 0 };
	bi.lpfn = &BrowseCallbackProc;
	bi.lpszTitle = ( wndTitle.c_str() );
	bi.hwndOwner = (HWND)parentWindow;
	bi.ulFlags = BIF_USENEWUI;
    _startPath = startPath;
	LPITEMIDLIST pidl = SHBrowseForFolder ( &bi );
    if ( pidl != 0 )
    {
        // get the name of the folder
        char path[MAX_PATH];
        if ( SHGetPathFromIDList ( pidl, path ) ) {
			ret = std::string(path);
        }

        // free memory used
        IMalloc * imalloc = 0;
        if ( SUCCEEDED( SHGetMalloc ( &imalloc )) ) {
            imalloc->Free ( pidl );
            imalloc->Release ( );
        }
    }
	return ret;
}
} // namespace com

#endif //#ifdef FRX_OS_WINDOWS

