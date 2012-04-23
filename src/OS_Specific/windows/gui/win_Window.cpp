/*
 * ===========================================================================================================
 * win_Window.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */

#include "win_Window.h"
#include "gui/ppiGui.h"
#include "com/Settings.h"
#include "com/PPIError.h"
#include "com/Log.h"
#include <gdiplus.h> 
#include "gui/Resources.h"
#include "cfileselector.h"
#include "Shlobj.h"
//#include <multimon.h>

extern HINSTANCE GetInstance();

namespace ppiGui {
//============================================================================================================
// Klasse: Window
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
WNDCLASSEX Window::wndClass = { 
	sizeof(WNDCLASSEX),																// cbSize
	CS_HREDRAW | CS_VREDRAW,														// style
	Window::osCallback,																// lpfnWndProc
	0,																				// cbClsExtra	
	0,																				// cbWndExtra
	NULL,																			// hInstance
	LoadIcon(wndClass.hInstance, MAKEINTRESOURCE( CHILDW_ICON )),			        // hIcon
	0,																				// hCursor
	(HBRUSH)(COLOR_WINDOW),  														// hbrBackground
	0,																				// lpszMenuName
	"PPIWindow",																	// lpszClassName
	0,																				// hIconSm
};
//------------------------------------------------------------------------------------------------------------
map< HWND, IWindow::Ptr > Window::hwnd2Object;
//------------------------------------------------------------------------------------------------------------
Window::Window ( const VSTGUI::CRect &size, const WindowFlags &flags, void *parentHandle  ) : 
hwnd(NULL),
caption("unnamed window"),
IWindow ( size, flags ),
posChanging(false)
{
	static WNDCLASSEX dummy;
	if ( !wndClass.hInstance ) wndClass.hInstance = GetInstance();
	if ( GetClassInfoEx ( NULL, wndClass.lpszClassName, (LPWNDCLASSEXA)&dummy ) ); 
	else RegisterClassEx(&wndClass);
	// Initalisiere Window
	hwnd = CreateWindow ( wndClass.lpszClassName,  caption.c_str(),
						  createWinStyleFlags() ,
						  0, 0, 0, 0,
						  (HWND)parentHandle, 0, wndClass.hInstance, NULL
	);
	if ( !hwnd ) throw ppiError::SystemError ("Could not create window.", __FILE__, __LINE__ );
}
//------------------------------------------------------------------------------------------------------------
Window::~Window() {
	UnregisterClass ( wndClass.lpszClassName, wndClass.hInstance );
}
//------------------------------------------------------------------------------------------------------------
inline UINT Window::createWinStyleFlags() {
	UINT f =  WS_VISIBLE | WS_POPUP;
	if ( haveFlag(TITLE_BAR)   ) f |= WS_CAPTION;
	if ( haveFlag(SYSTEM_MENU) ) f |= WS_SYSMENU;
	if ( haveFlag(RESIZEABLE)  ) f |= WS_SIZEBOX;
	return f;
}
//------------------------------------------------------------------------------------------------------------
void Window::showWindow( const CPoint &p ) {
	if ( !hwnd ) return;
	EventSender<OnOpen>::notifyEventListeners ( this, OnOpen() );	
	//setzte Window pos&groesse
	SetWindowPos ( hwnd, ( haveFlag(ALWAYS_ONTOP)?HWND_TOPMOST:HWND_TOP ), 
		           size.left, size.top, size.right, size.bottom, 0 );
	setPos ( p );
	// hwnd in ass. map speichern
	hwnd2Object.insert ( pair< HWND, Ptr> ( hwnd, self.lock() ) ) ;
}
//------------------------------------------------------------------------------------------------------------
void Window::setPos( const CPoint &pos ) {
	if ( !hwnd ) return;
	posChanging = true;
	MoveWindow ( hwnd, pos.x, pos.y, size.getWidth(), size.getHeight(), true );
	posChanging = false;
}
//------------------------------------------------------------------------------------------------------------
CPoint Window::getPos() const {
	RECT rect;
	GetWindowRect (hwnd, &rect);
	return CPoint(rect.left, rect.top);
}
//------------------------------------------------------------------------------------------------------------
void Window::setSize( const VSTGUI::CRect &size ) {
	if (!hwnd) {
		this->size = size;
		return;
	}
	//MoveWindow ( hwnd, size.left, size.top, size.getWidth(), size.getHeight(), true );
	RECT wRect;
	SetRect ( &wRect, size.x, size.y, size.getWidth(), size.getHeight() );
	AdjustWindowRectEx (&wRect, GetWindowLong (hwnd, GWL_STYLE), FALSE, GetWindowLong (hwnd, GWL_EXSTYLE));
	int width = wRect.right - wRect.left;
	int height = wRect.bottom - wRect.top;
	this->size.setWidth ( width );
	this->size.setHeight ( height );
	SetWindowPos (hwnd, HWND_TOP, 0, 0, width, height, SWP_NOMOVE);
}
//------------------------------------------------------------------------------------------------------------
void Window::closeWindow() {
	if ( !hwnd ) return;
	SendMessage ( hwnd, WM_CLOSE, 0, 0 );
}
//------------------------------------------------------------------------------------------------------------
void Window::destroyWindow() {
	if ( !hwnd ) return;
	EventSender<OnClose>::notifyEventListeners ( this, OnClose() );	
	map<HWND, Ptr>::iterator it = hwnd2Object.find ( hwnd );
	hwnd = NULL;
	if ( it == hwnd2Object.end() ) return;
	hwnd2Object.erase ( it );
}
//------------------------------------------------------------------------------------------------------------
void Window::messageFromOs(UINT message, WPARAM wp, LPARAM lp) {
	LPWINDOWPOS bounds;
	switch ( message ) {
		case WM_CLOSE :
			destroyWindow();
			break;
		case WM_PAINT :
			EventSender<OnPaint>::notifyEventListeners( this, OnPaint() );
			break;
		case WM_WINDOWPOSCHANGING:
			if ( posChanging ) break;
			bounds = (LPWINDOWPOS)lp;
			if ( bounds->flags & SWP_NOMOVE == SWP_NOMOVE ) break; //no move
			EventSender<OnMoving>::notifyEventListeners ( this, OnMoving( CPoint ( bounds->x, bounds->y ) ) );
			break;
	}
}
//------------------------------------------------------------------------------------------------------------
LRESULT CALLBACK Window::osCallback(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	map<HWND, Ptr>::iterator it = hwnd2Object.find ( hWnd );
	if ( it != hwnd2Object.end() )
		( (Window*)it->second.get() )->messageFromOs ( message, wParam, lParam );
	return DefWindowProc(hWnd, message, wParam, lParam);
}
//============================================================================================================
// Klasse : CDrawContextPlus
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
CDrawContextPlus::CDrawContextPlus ( CDrawContext &c ) : cD ( c ) {
}
//------------------------------------------------------------------------------------------------------------
void CDrawContextPlus::drawLine ( const CPoint &p1, const CPoint &p2 )  {
	using namespace Gdiplus;
	Graphics graphics( (HDC)cD.getSystemContext() );   
	graphics.SetSmoothingMode(SmoothingModeHighQuality);
	// color
	CColor c = cD.getFrameColor();
	Pen pen( Color(c.alpha, c.red, c.green, c.blue) );   
	// width
	pen.SetWidth ( (REAL)cD.getLineWidth() );
	// style
	CLineStyle style = cD.getLineStyle();
	if ( style==kLineOnOffDash ) {
		float p[] = {5,5};
		pen.SetDashPattern ( p, 2 );
	}
	else pen.SetDashStyle (DashStyleSolid);
	graphics.DrawLine( &pen, (INT)p1.x, (INT)p1.y, (INT)p2.x, (INT)p2.y );
}
//------------------------------------------------------------------------------------------------------------
void CDrawContextPlus::drawRect ( const VSTGUI::CRect &rect )  {
	using namespace Gdiplus;
	VSTGUI::CRect r = ppiGui::normalizeRect(rect);
	Graphics graphics( (HDC)cD.getSystemContext() );   
	graphics.SetSmoothingMode(SmoothingModeHighQuality);
	// color
	CColor c = cD.getFrameColor();
	Pen pen( Color(c.alpha, c.red, c.green, c.blue) ); 
	// width
	pen.SetWidth ( (REAL)cD.getLineWidth() );
	// style
	CLineStyle style = cD.getLineStyle();
	if ( style==kLineOnOffDash ) {
		float p[] = {5,5};
		pen.SetDashPattern ( p, 2 );
	}
	else pen.SetDashStyle (DashStyleSolid);
	graphics.DrawRectangle( &pen, (INT)r.x, (INT)r.y, (INT)r.getWidth(), (INT)r.getHeight() );
}
//------------------------------------------------------------------------------------------------------------
void CDrawContextPlus::fillRect ( const VSTGUI::CRect &r ) {
	using namespace Gdiplus;
	Graphics graphics((HDC)cD.getSystemContext());   
	graphics.SetSmoothingMode(SmoothingModeHighQuality);
	// color
	CColor c = cD.getFrameColor();
	SolidBrush brush ( Color(c.alpha, c.red, c.green, c.blue) ); 
	graphics.FillRectangle( &brush, (INT)r.x, (INT)r.y, (INT)r.getWidth(), (INT)r.getHeight() );
}
//------------------------------------------------------------------------------------------------------------
void CDrawContextPlus::drawArc ( const VSTGUI::CRect &rect, const CPoint &p, const CPoint &p2 ) {
	using namespace Gdiplus;
	VSTGUI::CRect r = ppiGui::normalizeRect(rect);
	Graphics graphics((HDC)cD.getSystemContext());   
	graphics.SetSmoothingMode(SmoothingModeHighQuality);
	// color
	CColor c = cD.getFrameColor();
	Pen pen( Color(c.alpha, c.red, c.green, c.blue) ); 
	// width
	pen.SetWidth ( (REAL)cD.getLineWidth() );
	// style
	CLineStyle style = cD.getLineStyle();
	if ( style==kLineOnOffDash ) {
		float p[] = {5,5};
		pen.SetDashPattern ( p, 2 );
	}
	else pen.SetDashStyle (DashStyleSolid);
	graphics.DrawArc ( &pen, (INT)r.x, (INT)r.y, (INT)r.getWidth(), (INT)r.getHeight(), 0.0, 360.0 );
}
//------------------------------------------------------------------------------------------------------------
void CDrawContextPlus::fillArc ( const VSTGUI::CRect &r, const CPoint &p, const CPoint &p2 ) {
	using namespace Gdiplus;
	Graphics graphics((HDC)cD.getSystemContext());   
	graphics.SetSmoothingMode(SmoothingModeHighQuality);
	// color
	CColor c = cD.getFillColor();
	SolidBrush brush ( Color(c.alpha, c.red, c.green, c.blue) ); 
	graphics.FillPie( &brush, (INT)r.x, (INT)r.y, (INT)r.getWidth(), (INT)r.getHeight(), 0.0, 360.0 );
}
//------------------------------------------------------------------------------------------------------------
CDrawContextPlus::~CDrawContextPlus() {}
//============================================================================================================
// OS createWindow
//============================================================================================================
IWindow::Ptr createWindow ( const VSTGUI::CRect &size, const IWindow::WindowFlags &flags, void *parentHandle ) {
	IWindow::Ptr neu = IWindow::Ptr ( new Window( size, flags, parentHandle ) );
	neu->__setPtr_ ( neu );
	return neu;
}

//============================================================================================================
// OS getScreenSize
//============================================================================================================
VSTGUI::CRect getScreenSize() {
	return VSTGUI::CRect ( 
		0, 
		0, 
		GetSystemMetrics(SM_CXVIRTUALSCREEN),
		GetSystemMetrics(SM_CYVIRTUALSCREEN)
	);
}
//============================================================================================================
// setHasContextCursor
//============================================================================================================
void setHasContextCursor() {
	SetCursor (LoadCursor ( GetInstance(), MAKEINTRESOURCE( CONTEXT_CURSOR )));
}
//============================================================================================================
//	Methode selectFile:
//============================================================================================================
string selectFile ( VstFileType *filetype, 
				    const string &wndTitle, 
				    const string &startPath, 
					AudioEffectX *audioEffectX      // audioEffectX ruft host-fileselector auf ansonsten system
				   )
{
	VstFileSelect vstFileSelect;
	memset (&vstFileSelect, 0, sizeof (VstFileSelect));
	vstFileSelect.command     = kVstFileLoad;
	vstFileSelect.type        = kVstFileType;
	strcpy ( vstFileSelect.title, wndTitle.c_str() );
	vstFileSelect.nbFileTypes = 1;
	vstFileSelect.fileTypes   = filetype;
	vstFileSelect.returnPath  = new char[4026];
	vstFileSelect.initialPath = (startPath.length() > 0) ? (char*)startPath.c_str() : NULL;
	vstFileSelect.future[0] = 1;	// utf-8 path on macosx
	CFileSelector cFile( audioEffectX ); 
	cFile.run (&vstFileSelect);
	string ret(vstFileSelect.returnPath);
	delete []vstFileSelect.returnPath;
	return ret;
}
//============================================================================================================
//	globale string: SHBrowseForFolder startpfad ueber BrowseCallbackProc
//============================================================================================================
string _startPath;
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
//============================================================================================================
//	Methode selectDirectoryOwn:
//============================================================================================================
string selectDirectoryOwn ( const string &wndTitle, 
						    const string &startPath,
							HWND owner )
{
	string ret;
	BROWSEINFO bi = { 0 };
	bi.lpfn = &BrowseCallbackProc;
	bi.lpszTitle = ( wndTitle.c_str() );
	bi.hwndOwner = owner;
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

//============================================================================================================
//	Methode selectDirectory:
//============================================================================================================
string selectDirectory ( const string &wndTitle, 
						 const string &startPath, 
						 AudioEffectX *audioEffectX
						)
{
	
	if (!audioEffectX) return "";
	AEffEditor *aEffEd = static_cast<AudioEffectX*>(audioEffectX)->getEditor();
	if (!aEffEd) return "";
	AEffGUIEditor *ppiEd = static_cast<AEffGUIEditor*>( aEffEd );
	HWND owner = static_cast<HWND>( ppiEd->getFrame()->getSystemWindow() );
	if (!owner) return "";
	return selectDirectoryOwn ( wndTitle, startPath, owner );
	
}
} // ppiGui
