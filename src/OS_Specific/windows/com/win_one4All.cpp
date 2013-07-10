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

extern void* hInstance;

namespace com {
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
bool isPlugFilename ( const string &filename ) { 
	return Filename(filename).extension() == ".dll"; 
} 
//------------------------------------------------------------------------------------------------------------
bool isDirectory ( const string &filename ) { 
	boost::filesystem::file_status s = boost::filesystem::status( sambag::com::Location(filename) ); 
	return is_directory (s);
} 
//------------------------------------------------------------------------------------------------------------
MessageBoxReturn osMessageBox ( const string &title, const string &text, const MessageBoxType &type ) {
	int ret = 0;
	UINT flags = MB_SYSTEMMODAL;
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
	string ret;
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
			ret = string(path);
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

