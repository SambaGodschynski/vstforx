/*
 * ===========================================================================================================
 * mac_Window.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */

#include "mac_Window.h"
#include "gui/ppiGui.h"
#include "com/Settings.h"
#include "com/PPIError.h"
#include "com/Log.h"
#include "cfileselector.h"
#include "gui/Resources.h"

static pascal OSStatus windowHandler (EventHandlerCallRef inHandlerCallRef, EventRef inEvent, void* inUserData);

namespace ppiGui {
//------------------------------------------------------------------------------------------------------------
//============================================================================================================
// OS createWindow
//============================================================================================================
map< Window::HWND, IWindow::Ptr > Window::hwnd2Object;
//------------------------------------------------------------------------------------------------------------
Window::Window ( const VSTGUI::CRect &size, const WindowFlags &flags, void *parentHandle  ) : 
	IWindow( size, flags ), hwnd(NULL), posChanging(false)
{
	com::MethodMessage<Window> ("construct Window");
	Rect mRect = { size.top, size.left, size.bottom, size.right };
	OSStatus err = CreateNewWindow ( 
									kFloatingWindowClass,
									kWindowCloseBoxAttribute | 
									kWindowAsyncDragAttribute | 
									kWindowCompositingAttribute | //causes crash with battery3
									kWindowStandardHandlerAttribute, 
									&mRect, &hwnd
	);
	
	WindowGroupRef grp = GetWindowGroup( (HWND) parentHandle );
	
	if (err != (OSStatus)noErr || grp == NULL) { 
		throw com::ppiError::SystemError("could not create Window", __FILE__, __LINE__);
		hwnd = NULL;
	}
	err = SetWindowGroup ( hwnd, grp );
	if (err != (OSStatus)noErr) throw com::ppiError::SystemError("could handle not assign to group", __FILE__, __LINE__);
}
//------------------------------------------------------------------------------------------------------------
void Window::showWindow( const CPoint &p ) {
	if (!hwnd) return;
	com::MethodMessage<Window> ("show Window");
	EventSender<OnOpen>::notifyEventListeners ( this, OnOpen() );
	static EventTypeSpec eventTypes[] = {
		{ kEventClassWindow, kEventWindowClose },
		{ kEventClassWindow, kEventWindowBoundsChanging },
		{ kEventClassWindow, kEventWindowBoundsChanged }
	};
	
	InstallWindowEventHandler (hwnd, 
							   NewEventHandlerUPP(windowHandler), 
							   GetEventTypeCount (eventTypes), 
							   eventTypes, 
							   hwnd, 
							   NULL);
	ShowWindow (hwnd);
	RepositionWindow (hwnd, NULL, kWindowCenterOnMainScreen);
	setPos(p);
	hwnd2Object.insert ( pair< HWND, Ptr> ( hwnd, self.lock() ) ) ;
}
//------------------------------------------------------------------------------------------------------------
void Window::setPos( const CPoint &pos ) {
    Rect bounds;
	GetWindowBounds (hwnd, kWindowContentRgn, &bounds);
	CPoint p( pos.x - bounds.left, pos.y - bounds.top );
	bounds.left   += p.x;
	bounds.top    += p.y;
	bounds.right  += p.x;
	bounds.bottom += p.y;
	posChanging = true;
	if (hwnd) SetWindowBounds (hwnd, kWindowContentRgn, &bounds); 
	size = CRect ( bounds.left, bounds.top, bounds.right, bounds.bottom );
	BringToFront(hwnd); 
	posChanging = false;
}
//------------------------------------------------------------------------------------------------------------
CPoint Window::getPos() const {
	Rect bounds;
	GetWindowBounds (hwnd, kWindowContentRgn, &bounds);
	return CPoint( bounds.left, bounds.top );
}
//------------------------------------------------------------------------------------------------------------
void Window::setSize( const VSTGUI::CRect &size ) {
	Window::size.setWidth  ( size.getWidth()  );
	Window::size.setHeight ( size.getHeight() );
	if (!hwnd) return;
	Rect bounds;
	GetWindowBounds (hwnd, kWindowContentRgn, &bounds);
	bounds.right = bounds.left + size.getWidth();
	bounds.bottom = bounds.top + size.getHeight();
	SetWindowBounds (hwnd, kWindowContentRgn, &bounds); 
}
//------------------------------------------------------------------------------------------------------------
void Window::closeWindow() {
	if ( !hwnd ) return;
	destroyWindow();
	hwnd = NULL;
}
//------------------------------------------------------------------------------------------------------------
void Window::destroyWindow() {
	if (!hwnd) return;
	EventSender<OnClose>::notifyEventListeners ( this, OnClose() );	
	map<HWND, Ptr>::iterator it = hwnd2Object.find ( hwnd );
	if ( it == hwnd2Object.end() ) return;
	HideWindow(hwnd);
	DisposeWindow (hwnd);
	hwnd2Object.erase ( it );
}	
//------------------------------------------------------------------------------------------------------------
pascal OSStatus Window::messageFromOs (EventHandlerCallRef inHandlerCallRef, EventRef inEvent, void *inUserData) {
	UInt32 eventClass = GetEventClass (inEvent);
	if ( eventClass != (UInt32)kEventClassWindow ) return noErr;
	UInt32 eventKind = GetEventKind (inEvent);
	Rect bounds;
	/*
	 if you want to add a new eventtype don't forget to register event with InstallWindowEventHandler call
	 see showWindow().
	 */
	switch (eventKind) {
		case kEventWindowClose :
			closeWindow();
			break;
		case kEventWindowBoundsChanging:
		case kEventWindowBoundsChanged:
			if ( posChanging ) break;
				// left the code for live-resizing, but it is not used, because of window-refreshing issues...
				GetEventParameter( inEvent, 
					kEventParamCurrentBounds, 
					typeQDRectangle, 
					NULL, 
					sizeof(Rect), 
					NULL, 
					&bounds 
				);    
                EventSender<OnMoving>::notifyEventListeners ( 
					this, OnMoving( CPoint ( bounds.left, bounds.top ) ) 
				);
                break;
	}	
	return noErr;
}
//------------------------------------------------------------------------------------------------------------
Window::~Window() {
}
//------------------------------------------------------------------------------------------------------------
pascal OSStatus Window::osCallback (EventHandlerCallRef inHandlerCallRef, EventRef inEvent, void *inUserData){
	map<HWND, Ptr>::iterator it = hwnd2Object.find ( (HWND)inUserData );
	if ( it != hwnd2Object.end() )
		return ( (Window*)it->second.get() )->messageFromOs ( inHandlerCallRef, inEvent, inUserData );
	return noErr;
}	
//============================================================================================================
// OS createWindow
//============================================================================================================
IWindow::Ptr createWindow ( const CRect &size, const IWindow::WindowFlags &flags, void *parentHandle ) {
	IWindow::Ptr neu( new Window(size, flags, parentHandle) ); 	
	neu->__setPtr_ ( neu );
	return neu;
}
//============================================================================================================
// OS getScreenSize
//============================================================================================================
VSTGUI::CRect getScreenSize() {
	::Rect rect;
	OSStatus res = 
		GetAvailableWindowPositioningBounds ( NULL, &rect );
	if ( res == (OSStatus)noErr )
		return VSTGUI::CRect ( rect.left, rect.top, rect.right, rect.bottom );
	return VSTGUI::CRect();

}
//============================================================================================================
// Klasse : CDrawContextPlus
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
CDrawContextPlus::CDrawContextPlus ( CDrawContext &c ) : cD ( c ) {
}
//------------------------------------------------------------------------------------------------------------
void CDrawContextPlus::drawLine ( const CPoint &p1, const CPoint &p2 )  {
	cD.setDrawMode(kAntialias);
	cD.moveTo ( p1 );
	cD.lineTo ( p2 );
	cD.setDrawMode(kCopyMode);
}
//------------------------------------------------------------------------------------------------------------
void CDrawContextPlus::drawRect ( const CRect &rect )  {
	cD.drawRect(rect); 
}
//------------------------------------------------------------------------------------------------------------
void CDrawContextPlus::fillRect ( const CRect &r )  {
}
//------------------------------------------------------------------------------------------------------------
void CDrawContextPlus::drawArc ( const CRect &rect, const CPoint &p, const CPoint &p2 )  {
}
//------------------------------------------------------------------------------------------------------------
void CDrawContextPlus::fillArc ( const CRect &r, const CPoint &p, const CPoint &p2 )  {
}
//------------------------------------------------------------------------------------------------------------
CDrawContextPlus::~CDrawContextPlus() {
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
//	Methode selectDirectory:
//============================================================================================================
string selectDirectory ( const string &wndTitle, 
						 const string &startPath, 
						 AudioEffectX *audioEffectX      // audioEffectX ruft host-fileselector auf ansonsten system
						)
{
	VstFileSelect vstFileSelect;
	memset (&vstFileSelect, 0, sizeof (VstFileSelect));
	vstFileSelect.command     = kVstDirectorySelect;
	vstFileSelect.type        = kVstFileType;
	strcpy ( vstFileSelect.title, wndTitle.c_str() );
	vstFileSelect.nbFileTypes = 0;
	vstFileSelect.fileTypes   = NULL;
	vstFileSelect.returnPath  = new char[4026];
	vstFileSelect.initialPath = (startPath.length() > 0) ? (char*)startPath.c_str() : NULL;
	vstFileSelect.future[0] = 1;	// utf-8 path on macosx
	CFileSelector cFile(audioEffectX); 
	cFile.run (&vstFileSelect);
	string ret(vstFileSelect.returnPath);
	delete []vstFileSelect.returnPath;
	return ret;
}
} // ppiGui


//============================================================================================================
// Callback(s)
//============================================================================================================
static pascal OSStatus windowHandler (EventHandlerCallRef inHandlerCallRef, EventRef inEvent, void *inUserData)
{
	
	return ppiGui::Window::osCallback ( inHandlerCallRef, 
									   inEvent, 
									   (WindowRef) inUserData );
}
