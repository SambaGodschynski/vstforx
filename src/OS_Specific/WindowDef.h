#ifndef WINDOW_DEF_H
#define WINDOW_DEF_H

#include "OS_Specific/OS_com.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "gui/ViewEvents.h"
#include "vstgui.h"
#include "audioeffectx.h"
struct CRect;

namespace ppiGui {
struct OnPaint : public events::Event {};
struct OnMoving : public events::Event {
	CPoint newPos;
	OnMoving ( const CPoint &newPos ) : newPos( newPos ) {}
};
//============================================================================================================
// Klasse : IWindow
//============================================================================================================
class IWindow : 
	public EventSender<OnOpen>, 
	public EventSender<OnClose>, 
	public EventSender<OnPaint>,
	public EventSender<OnMoving>
{
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<IWindow> Ptr;
	//--------------------------------------------------------------------------------------------------------
	enum {
		TITLE_BAR       = 0x0001,
		SYSTEM_MENU     = 0x0002,
		RESIZEABLE      = 0x0004,
		ALWAYS_ONTOP    = 0x0008,
	};
	typedef long WindowFlags;
    static const int DEFAULT_WINDOW = TITLE_BAR | SYSTEM_MENU;
protected:
	//--------------------------------------------------------------------------------------------------------
	boost::weak_ptr<IWindow> self;
	//--------------------------------------------------------------------------------------------------------
	VSTGUI::CRect size;
	//--------------------------------------------------------------------------------------------------------
	WindowFlags flags;
public:
	//--------------------------------------------------------------------------------------------------------
	bool haveFlag( const WindowFlags &flag ) { return ( flag & flags ) == flag; }
	//--------------------------------------------------------------------------------------------------------
	void __setPtr_ ( const Ptr &ptr ) {
		if ( !self.lock() ) self = ptr;
		else throw "IWindow::__setPtr_() is not for external use.";
	}
	//--------------------------------------------------------------------------------------------------------
	virtual void showWindow( const CPoint &p = CPoint() ) = 0;
	//--------------------------------------------------------------------------------------------------------
	virtual void setCaption ( const string& ) = 0;
	//--------------------------------------------------------------------------------------------------------
	virtual void closeWindow() = 0;
	//--------------------------------------------------------------------------------------------------------
	virtual bool isOpen() const = 0;
	//--------------------------------------------------------------------------------------------------------
	IWindow ( const VSTGUI::CRect &size, const WindowFlags &flags ) : flags(flags), size(size) {}
	//--------------------------------------------------------------------------------------------------------
	virtual void setPos( const CPoint &p ) = 0;
	//--------------------------------------------------------------------------------------------------------
	virtual CPoint getPos() const = 0;
	//--------------------------------------------------------------------------------------------------------
	virtual void * getHandle() const = 0 ;
	//--------------------------------------------------------------------------------------------------------
	virtual ~IWindow(){}
	//--------------------------------------------------------------------------------------------------------
	virtual const VSTGUI::CRect & getSize() const { return size; }
	//--------------------------------------------------------------------------------------------------------
	virtual void setSize ( const VSTGUI::CRect& ) = 0;
};
//============================================================================================================
extern IWindow::Ptr createWindow ( const VSTGUI::CRect &size, 
								   const IWindow::WindowFlags &flags,
								   void *parentHandle = NULL);
//============================================================================================================
extern VSTGUI::CRect getScreenSize();
//============================================================================================================
extern void setHasContextCursor();
//============================================================================================================
// Klasse : CDrawContextPlus
//============================================================================================================
class CDrawContextPlus;
//============================================================================================================
extern string selectFile ( VstFileType *filetype, 
						   const string &wndTitle, 
						   const string &startPath, 
						   AudioEffectX *audioEffectX = NULL // audioEffectX ruft host-fileselector auf ansonsten system
						   );
//============================================================================================================
extern string selectDirectory ( const string &wndTitle, 
							    const string &startPath, 
								AudioEffectX *audioEffectX = NULL // audioEffectX ruft host-fileselector auf ansonsten system
							   );
} //namespace ppiGui

#endif