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
#include <Shellapi.h>
#include <sstream>
#include <windows.h>
#include <exception>
#include <sambag/com/Thread.hpp>
#include <map>
#include <Commdlg.h>

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
const char * FRX_VST_EXT = ".dll";
const char * FRX_VST3_EXT = ".vst3";
const char * FRX_LUA_EXT = ".lua";
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
    std::string ext = Filename(filename).extension().string();
	return ext == std::string(FRX_VST_EXT) ||
           ext == std::string(FRX_VST3_EXT) ||
           ext == std::string(FRX_LUA_EXT);
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
//--------------------------------------------------------------------------------------------------------
std::string osSelectFile ( const std::string &wndTitle,
						    const std::string &startPath,
							void *parentWindow)
{
	//TCHAR szFilters[] = _T("Scribble Files (*.dat)\0*.dat\0\0");
	char szFilePathName[_MAX_PATH] = "";
	OPENFILENAME ofn = {0};
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = (HWND)parentWindow;
	//ofn.lpstrFilter = szFilters;
	ofn.lpstrFile = szFilePathName;
	//ofn.lpstrDefExt = _T("dat");
	ofn.nMaxFile = _MAX_PATH;
	ofn.lpstrTitle = wndTitle.c_str();
	ofn.Flags = OFN_OVERWRITEPROMPT;
	ofn.lpstrInitialDir = startPath.c_str();
	GetOpenFileName(&ofn);
	return std::string(ofn.lpstrFile);
}
//--------------------------------------------------------------------------------------------------------
std::string osSaveFile ( const std::string &wndTitle,
						    const std::string &startPath,
							void *parentWindow)
{
	//TCHAR szFilters[] = _T("Scribble Files (*.dat)\0*.dat\0\0");
	char szFilePathName[_MAX_PATH] = "";
	OPENFILENAME ofn = {0};
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = (HWND)parentWindow;
	//ofn.lpstrFilter = szFilters;
	ofn.lpstrFile = szFilePathName;
	//ofn.lpstrDefExt = _T("dat");
	ofn.nMaxFile = _MAX_PATH;
	ofn.lpstrTitle = wndTitle.c_str();
	ofn.Flags = OFN_OVERWRITEPROMPT;
	ofn.lpstrInitialDir = startPath.c_str();
	GetSaveFileName(&ofn);
	return std::string(ofn.lpstrFile);
}
//--------------------------------------------------------------------------------------------------------
void osOpenLink(const std::string &url) {
    ShellExecute(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
}
//--------------------------------------------------------------------------------------------------------
namespace {
	std::map<sambag::com::ThreadId, std::pair<std::string,std::string> > __dlgIO;
	sambag::com::RecursiveMutex __dlgMutex;
	enum{DLG_ID=100, // hardcoded in resources.rc
		 DLG_TEXT=101};
	LRESULT CALLBACK dlgProc(HWND hWndDlg, UINT msg, WPARAM wParam, LPARAM lParam) {
		sambag::com::ThreadId id = sambag::com::getThreadId();
		switch(msg) {
		case WM_INITDIALOG:
			SetWindowText(hWndDlg,__dlgIO[id].first.c_str());
			SAMBAG_BEGIN_SYNCHRONIZED(__dlgMutex)
				SetDlgItemText(hWndDlg,DLG_TEXT,__dlgIO[id].second.c_str());
			SAMBAG_END_SYNCHRONIZED
			return TRUE;
		case WM_COMMAND:
			switch(wParam) {
			case IDOK: {
				TCHAR szBuffer[512];
				GetDlgItemText(hWndDlg,DLG_TEXT,szBuffer,512);
				SAMBAG_BEGIN_SYNCHRONIZED(__dlgMutex)
					__dlgIO[id].second = std::string(szBuffer);
				SAMBAG_END_SYNCHRONIZED
				EndDialog(hWndDlg, 0);
				return TRUE;
			}
			case IDCANCEL:
				EndDialog(hWndDlg, 1);
				return TRUE;
			}
		}
		return FALSE;
	}
} // namespace
void osShowInputTextDlg(const std::string &title, std::string &inOut, void *parentWindow) {
	
	sambag::com::ThreadId id = sambag::com::getThreadId();
	//prepare data
	SAMBAG_BEGIN_SYNCHRONIZED(__dlgMutex)
		__dlgIO[id].first = title;
		__dlgIO[id].second = inOut;
	SAMBAG_END_SYNCHRONIZED
	//show dlg box
	int res = DialogBox((HINSTANCE)hInstance, MAKEINTRESOURCE(DLG_ID),
	          (HWND)parentWindow, (DLGPROC)dlgProc);
	if (res!=0) {
		inOut="";
		return;
	}
	//get result
	SAMBAG_BEGIN_SYNCHRONIZED(__dlgMutex)
		inOut = __dlgIO[id].second;
		__dlgIO.erase(id);
	SAMBAG_END_SYNCHRONIZED
}
} // namespace com

#endif //#ifdef FRX_OS_WINDOWS

