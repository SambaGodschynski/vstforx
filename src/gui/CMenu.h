/*
 * ===========================================================================================================
 * CMenu.h
 *      Author: Johannes Unger
 * ===========================================================================================================
 */

#ifndef CMENU_H
#define CMENU_H

#include "vstgui.h"
#include "ViewEvents.h"
#include "IdleListener.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "boost/thread.hpp"
#include <list>
#include "processing/DspTools.h"
#include "com/Command.h"
#include <boost/function.hpp>
#include "Font.h"

#define ADD_MENU_LABEL(menulist, label, cmd) \
	(menulist).push_back ( menu::CMenuLabel::create ( (label), Command::Ptr( (cmd) ) ) )

#define ADD_MENU_PARAMETER_ENTRY(menulist, par, cmd) \
	(menulist).push_back ( menu::CParameterEntry::create ( (par), Command::Ptr( (cmd) ) ) )

#define ADD_MENU_TITLE(menulist, title) \
	(menulist).push_back ( menu::CMenuTitle::create ( (title) ) );

#define ADD_SUB_MENU(menulist, title, submenu) \
	(menulist).push_back ( menu::CSubMenuEntry::create ( (title), CMenu::Ptr ( (submenu) ) ) );

#define ADD_DYNSUB_MENU(menulist, title, callbackType, callbackObject, func, frame)   \
    (menulist).push_back ( menu::CDynamicSubMenu<callbackType>::              \
    create ( (title), callbackObject, frame, func ) );                              
	

namespace ppiGui {
class GConnection;
namespace menu{
//============================================================================================================
//	Klasse CMenu:
//============================================================================================================
class CMenu;
//============================================================================================================
//	Klasse CMenuEntry:
//============================================================================================================
class CMenuEntry;
//============================================================================================================
//	Klasse MenuEntryContainer:
//============================================================================================================
class CMenuEntryContainer;
//============================================================================================================
//	Def. MenuEntryPtr
//============================================================================================================
typedef boost::shared_ptr<CMenuEntry> MenuEntryPtr;
//============================================================================================================
//	Def. MenuEntryList
//============================================================================================================
typedef list<MenuEntryPtr> MenuEntryList;
//============================================================================================================
//	Klasse CMenuLabel
//============================================================================================================
class CMenuLabel;
//============================================================================================================
//	Klasse CParameterEntry
//============================================================================================================
class CParameterEntry;
//============================================================================================================
//	Klasse CMenuTitle
//============================================================================================================
class CMenuTitle;
//============================================================================================================
//	Klasse CSubMenu
//============================================================================================================
class CSubMenu;
//============================================================================================================
//	Klasse CSubMenuEntry
//============================================================================================================
class CSubMenuEntry;
//============================================================================================================
//	Klasse CDynamicSubMenu
//============================================================================================================
template < typename CallbackType >
class CDynamicSubMenu;
//============================================================================================================
//	Klasse CMenuScrollSwitch
//  Ein MenuEintrag der einen Scroll Rocker Switch enthaelt. Verschiebt bei betaetigung das Menu Offset.
//============================================================================================================
class CMenuScrollSwitch;
//============================================================================================================
//	Klasse CMenuScrollSwitch
//  Leerer Menueintrag.
//============================================================================================================
class CNullEntry;
//============================================================================================================
//	Event ShowSubMenu
//============================================================================================================
struct ShowSubMenu : public Event {
	CMenu *menu;
	ShowSubMenu ( CMenu *menu ) : menu(menu) {}
};
//============================================================================================================
//	Event HideSubMenu
//============================================================================================================
struct HideSubMenu : public Event {
	CMenu *menu;
	HideSubMenu ( CMenu *menu ) : menu(menu) {}
};

} // namespace menu 
} // namespace ppiGui

namespace ppiGui {
namespace menu{
using namespace com;
//============================================================================================================
//	Klasse CMenu:
//============================================================================================================
class CMenu : 
	public CView, 
	public EventSender<OnMouseLeave>,
	public EventSender<OnMouseEnter>,
	public EventSender<OnMouseClick>,
	public EventListener<OnMouseClick>,
	public EventListener<OnIdle>,
	public EventListener<OnMouseEnter>,
	public EventListener<ShowSubMenu>,
	public EventListener<HideSubMenu>
{
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<CMenu> Ptr;
private:
friend struct RemoveMenu;
friend class CSubMenu;
	//--------------------------------------------------------------------------------------------------------
	struct SubMenuListener : 
		public EventListener<OnMouseEnter>,
		public EventListener<OnMouseLeave>,
		public EventListener<OnMouseClick> 
	{
		CMenu *parent;
		void eventHandler ( void *src, const OnMouseEnter &ev );
		void eventHandler ( void *src, const OnMouseLeave &ev );
		void eventHandler ( void *src, const OnMouseClick &ev );
	} subMenuListener; // SubMenuListener
	//--------------------------------------------------------------------------------------------------------
	struct MenuView : public CView {
		CMenu &parent;
		MenuView ( CMenu &parent ) : parent(parent), CView ( VSTGUI::CRect() ) {};
		void draw ( CDrawContext *cD );
	} *menuView;
	//--------------------------------------------------------------------------------------------------------
	bool hold;
	//--------------------------------------------------------------------------------------------------------
	CPoint lastMenuPos;
	//--------------------------------------------------------------------------------------------------------
	CMenu *subMenu; // jedes menu obj. darf ein submenu anzeigen
	//--------------------------------------------------------------------------------------------------------
	processing::ClockEdge focusTrigger;
protected:
	//--------------------------------------------------------------------------------------------------------
	boost::weak_ptr<CMenu> self;
	//--------------------------------------------------------------------------------------------------------
	inline void setOutline ( const VSTGUI::CRect &size, const CPoint &p ); 
	//--------------------------------------------------------------------------------------------------------
	inline CPoint calcMenuPos ( const VSTGUI::CRect &size, const CPoint &_where );
	//--------------------------------------------------------------------------------------------------------
	inline void drawEntries (CDrawContext *cc);
	//--------------------------------------------------------------------------------------------------------
	inline void drawFrame (CDrawContext *cc, VSTGUI::CRect&);
	//--------------------------------------------------------------------------------------------------------
	// max. gleichzeitig angezeigte eintraege
	static const int maxEntries = 15;
	//--------------------------------------------------------------------------------------------------------
	CPoint currMousePos;
	//--------------------------------------------------------------------------------------------------------
	CMenuScrollSwitch *scrollSwitch;
	//--------------------------------------------------------------------------------------------------------
	MenuEntryList menuEntries;
	//--------------------------------------------------------------------------------------------------------
	CMenu ( CFrame *frame );
	//--------------------------------------------------------------------------------------------------------
	CNullEntry *nullEntry;
	//--------------------------------------------------------------------------------------------------------
	// Ist entweder scrollSwitch oder nullEntry je nachdem ob benoetigt oder nicht.
	CMenuEntry *scroller;
	//--------------------------------------------------------------------------------------------------------
	bool visible;
	//--------------------------------------------------------------------------------------------------------
	int entr_offset;
public:
	//--------------------------------------------------------------------------------------------------------
	// berechnet menue frame.
	inline VSTGUI::CRect calcSize();
	//--------------------------------------------------------------------------------------------------------
	MenuEntryList & getMenuEntries() { return menuEntries; }
	//--------------------------------------------------------------------------------------------------------
	static Ptr create( CFrame *frame );
	//--------------------------------------------------------------------------------------------------------
	Ptr getPtr() { return self.lock(); }
	//--------------------------------------------------------------------------------------------------------
	virtual ~CMenu();
	//--------------------------------------------------------------------------------------------------------
	virtual void draw ( CDrawContext *cc );
	//--------------------------------------------------------------------------------------------------------
	virtual void showAt ( const CPoint &p );
	//--------------------------------------------------------------------------------------------------------
	virtual void hide();
	//--------------------------------------------------------------------------------------------------------
	bool isVisible() { return visible; }
	//--------------------------------------------------------------------------------------------------------
	virtual bool hitTest ( CPoint &p ) { return ( isVisible() ) ? CView::hitTest(p) : false; }
	//--------------------------------------------------------------------------------------------------------
	bool entriesEmpty() { return menuEntries.empty(); }
	//--------------------------------------------------------------------------------------------------------
	bool numEntries() { return menuEntries.size(); }
	//--------------------------------------------------------------------------------------------------------
	void moveTo ( CPoint &p );
	//--------------------------------------------------------------------------------------------------------
	void clearEntries ();
	//--------------------------------------------------------------------------------------------------------
	void addMenuEntry ( MenuEntryPtr entry );
	//--------------------------------------------------------------------------------------------------------
	void addMenuEntryList ( MenuEntryList &list );
	//--------------------------------------------------------------------------------------------------------
	void setMenuEntryList ( MenuEntryList &list ) { clearEntries(); addMenuEntryList ( list ); }
	//--------------------------------------------------------------------------------------------------------
	virtual void eventHandler ( void *src, const OnIdle &ev ); 
	//--------------------------------------------------------------------------------------------------------
	void scrollDwn();
	//--------------------------------------------------------------------------------------------------------
	void scrollUp();
	//--------------------------------------------------------------------------------------------------------
	virtual void mouse ( CDrawContext *cc, CPoint &p, long btn = -1 );
	//--------------------------------------------------------------------------------------------------------
	virtual void eventHandler ( void *src, const OnMouseClick &ev );
	//--------------------------------------------------------------------------------------------------------
	virtual void eventHandler ( void *src, const OnMouseEnter &ev );
	//--------------------------------------------------------------------------------------------------------
	virtual void eventHandler ( void *src, const ShowSubMenu &ev );
	//--------------------------------------------------------------------------------------------------------
	virtual void eventHandler ( void *src, const HideSubMenu &ev );
};
//============================================================================================================
//	Klasse CSubMenu:
//============================================================================================================
class CSubMenu : public CMenu {
friend class CSubMenuEntry;
public:
private:
protected:
public:
	//--------------------------------------------------------------------------------------------------------
	CSubMenu ( CFrame *parent ) : CMenu (parent)  {}
	//--------------------------------------------------------------------------------------------------------
	static Ptr create( CFrame *frame );
};
//============================================================================================================
//	Klasse CMenuEntry:
//============================================================================================================
class CMenuEntry : public EventSender<OnMouseClick> {
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<CMenuEntry> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	bool init;
protected:
	//--------------------------------------------------------------------------------------------------------
	VSTGUI::CRect size;
	//--------------------------------------------------------------------------------------------------------
	boost::weak_ptr<CMenuEntry> self;
	//--------------------------------------------------------------------------------------------------------
	Command::Ptr action;
	//--------------------------------------------------------------------------------------------------------
	CPoint subMenuPos;
	//--------------------------------------------------------------------------------------------------------
	CMenuEntry( VSTGUI::CRect size, Command::Ptr action ) : action(action), init(false) {}
public:
	//--------------------------------------------------------------------------------------------------------
	CPoint getSubMenuPos() const { return subMenuPos; }
	//--------------------------------------------------------------------------------------------------------
	void setPtr ( const Ptr &ptr ) { if (init) return; self = ptr; init = true; }
	//--------------------------------------------------------------------------------------------------------
	const VSTGUI::CRect & getViewSize() const { return size; }
	//--------------------------------------------------------------------------------------------------------
	void getViewSize( VSTGUI::CRect &rect ) const { rect = size; }
	//--------------------------------------------------------------------------------------------------------
	void setViewSize ( const VSTGUI::CRect &_size ) { size = _size; } 
	//--------------------------------------------------------------------------------------------------------
	CCoord getWidth() const { return size.getWidth(); } 
	//--------------------------------------------------------------------------------------------------------
	CCoord getHeight() const { return size.getHeight(); }
	//--------------------------------------------------------------------------------------------------------
	virtual ~CMenuEntry() {}
	//--------------------------------------------------------------------------------------------------------
	// Zeichnet eintrag. mouseOver = true wenn Maus uber Eintrag. 
	virtual void draw ( CDrawContext *cc, const CPoint &mousePos ) = 0;
	//--------------------------------------------------------------------------------------------------------
	Command::Ptr getAction() { return action; }
	//--------------------------------------------------------------------------------------------------------
	virtual void mouse ( CDrawContext *cc, CPoint &p, long btn = -1 );
	//--------------------------------------------------------------------------------------------------------
	virtual bool hitTest ( const CPoint &p ) { return size.pointInside(p); } 
};
//============================================================================================================
//	Klasse CMenuLabel
//============================================================================================================
class CMenuLabel : public CMenuEntry {
private:
	//--------------------------------------------------------------------------------------------------------
	MyString text;
protected:
	//--------------------------------------------------------------------------------------------------------
	CColor color;
	//--------------------------------------------------------------------------------------------------------
	ppiGui::Font font;
	//--------------------------------------------------------------------------------------------------------
	CMenuLabel ( const MyString &text, Command::Ptr action, const ppiGui::Font & font  );
	//--------------------------------------------------------------------------------------------------------
	void setColor ( const CColor &col ) { color = col; }
public:
	//--------------------------------------------------------------------------------------------------------
	static Ptr create ( const MyString &text, Command::Ptr action, const ppiGui::Font & font = ppiGui::NORMAL_FONT ) {
		Ptr neu = Ptr ( new CMenuLabel ( text, action, font ) );
		neu->setPtr ( neu );
		return neu;
	}
	//--------------------------------------------------------------------------------------------------------
	virtual void setText ( const MyString & str );
	//--------------------------------------------------------------------------------------------------------
	virtual MyString getText () { 
		return text;
	}
	//--------------------------------------------------------------------------------------------------------
	virtual void draw ( CDrawContext *cc, const CPoint &mousePos );
	//--------------------------------------------------------------------------------------------------------
	virtual ~CMenuLabel() {}
};
//============================================================================================================
//	Klasse CParameterEntry
//============================================================================================================
class CParameterEntry : public CMenuLabel {
private:
	//--------------------------------------------------------------------------------------------------------
	CParameterEntry ( processing::parameter::Parameter::Ptr p, Command::Ptr action, const ppiGui::Font & font  );
	//--------------------------------------------------------------------------------------------------------
	processing::parameter::Parameter::Ptr par;
	//--------------------------------------------------------------------------------------------------------
	bool onDrag;
	//--------------------------------------------------------------------------------------------------------
	CPoint dragPos;
public:
	//--------------------------------------------------------------------------------------------------------
	static Ptr create ( processing::parameter::Parameter::Ptr p, Command::Ptr action, const ppiGui::Font & font = ppiGui::NORMAL_FONT ) {
		Ptr neu = Ptr ( new CParameterEntry ( p, action, font ) );
		neu->setPtr ( neu );
		return neu;
	}
	//--------------------------------------------------------------------------------------------------------
	inline void drawBar ( CDrawContext *cc, const CColor &col );
	//--------------------------------------------------------------------------------------------------------
	virtual void draw ( CDrawContext *cc, const CPoint &mousePos );
	//--------------------------------------------------------------------------------------------------------
	virtual ~CParameterEntry();
	//--------------------------------------------------------------------------------------------------------
	virtual void mouse ( CDrawContext *cc, CPoint &p, long btn = -1 );
	//--------------------------------------------------------------------------------------------------------
	virtual MyString getText () { return par->getName() + " :: " + par->getDisplay() + par->getLabel(); }
};
//============================================================================================================
//	Klasse CSubMenuEntry
//============================================================================================================
class CSubMenuEntry : public CMenuLabel, 
				 public EventListener<OnMouseEnter>, 
				 public EventListener<OnMouseLeave>,
				 public EventListener<OnMouseClick>,
				 public EventSender<OnMouseEnter>, 
				 public EventSender<OnMouseLeave>,
				 public EventSender<ShowSubMenu>,
				 public EventSender<HideSubMenu>
{
private:
	//--------------------------------------------------------------------------------------------------------
	processing::DelayedClockEdge delayedClockEdge;
protected:
	//--------------------------------------------------------------------------------------------------------
	CMenu::Ptr subMenu;
	//--------------------------------------------------------------------------------------------------------
	MenuEntryList mL;
	//--------------------------------------------------------------------------------------------------------
	CSubMenuEntry ( const MyString &text, const CMenu::Ptr &subMenu, const ppiGui::Font & font );
	//--------------------------------------------------------------------------------------------------------
	inline CPoint calcMenuPos ();
public:
	//--------------------------------------------------------------------------------------------------------
	void resetDelay();
	//--------------------------------------------------------------------------------------------------------
	static Ptr create ( const MyString &text, const CMenu::Ptr &subMenu, const ppiGui::Font & font = ppiGui::NORMAL_FONT ) {
		Ptr neu = Ptr ( new CSubMenuEntry ( text, subMenu, font ) );
		neu->setPtr ( neu );
		return neu;
	}
	//--------------------------------------------------------------------------------------------------------
	virtual void draw ( CDrawContext *cc, const CPoint &mousePos );
	//--------------------------------------------------------------------------------------------------------
	CMenu::Ptr getSubMenu() { return subMenu; }
	//--------------------------------------------------------------------------------------------------------
	virtual ~CSubMenuEntry();
	//--------------------------------------------------------------------------------------------------------
	virtual void eventHandler ( void *scr, const OnMouseEnter &ev );
	//--------------------------------------------------------------------------------------------------------
	virtual void eventHandler ( void *scr, const OnMouseLeave &ev );
	//--------------------------------------------------------------------------------------------------------
	virtual void eventHandler ( void *scr, const OnMouseClick &ev );
};
//============================================================================================================
//	Klasse CDynamicSubMenu
//  Ruft bei OnMouseEntry SubMenuCallbackFunction um MenuEntryList zu fuellen.
//  ( lazy init. )
//============================================================================================================
template < typename CallbackType >
class CDynamicSubMenu : public CSubMenuEntry {
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::function< void ( MenuEntryList&, const CallbackType& ) > CallbackFunction; 
private:
	//--------------------------------------------------------------------------------------------------------
	CDynamicSubMenu ( const MyString &text, const ppiGui::Font & font, CallbackType callbackObject, CFrame *frame );
	//--------------------------------------------------------------------------------------------------------
	CallbackType callbackObject;
public:
	//--------------------------------------------------------------------------------------------------------
	CallbackFunction callbackFunction; 
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr< CDynamicSubMenu< CallbackType > > Ptr;
	//--------------------------------------------------------------------------------------------------------
	void setCallbackObject ( const CallbackType &id ) { callbackObject = id; }
	//--------------------------------------------------------------------------------------------------------
	const CallbackType & getCallbackObject() { return callbackObject; }
	//--------------------------------------------------------------------------------------------------------
	virtual void draw ( CDrawContext *cc, const CPoint &mousePos );
	//--------------------------------------------------------------------------------------------------------
	static Ptr create ( const MyString &text, 
		                CallbackType callbackObject, 
						CFrame *frame, 
						const CallbackFunction &_callbackFunction,
						const ppiGui::Font & font = ppiGui::NORMAL_FONT )
	{
		Ptr neu = Ptr ( new CDynamicSubMenu ( text, font, callbackObject, frame ) );
		neu->setPtr ( neu );
		neu->callbackFunction = _callbackFunction;
		return neu;
	}
};
//============================================================================================================
//	Klasse CDynamicSubMenu
//  Templ. Meth. defs:
//============================================================================================================
template < typename CallbackType >
CDynamicSubMenu<CallbackType>::CDynamicSubMenu 
( 
	const MyString &text, 
	const ppiGui::Font & font, CallbackType callbackObject, 
	CFrame *frame 
	) : CSubMenuEntry ( text, CMenu::create( frame ), font ), callbackObject(callbackObject)
{
}
//------------------------------------------------------------------------------------------------------------
template < typename CallbackType >
void CDynamicSubMenu<CallbackType>::draw( CDrawContext *cc, const CPoint &mousePos ) {
	
	if ( mL.empty() && hitTest(mousePos) ) { 
		// call function
		callbackFunction( mL, callbackObject );
	}
	// submenu zeichnen
	CSubMenuEntry::draw ( cc, mousePos );
}
//============================================================================================================
//	Klasse CMenuTitle
//============================================================================================================
class CMenuTitle : public CMenuLabel {
protected:
	//--------------------------------------------------------------------------------------------------------
	CMenuTitle ( const MyString &text, const ppiGui::Font & font = ppiGui::NORMAL_FONT );
public:
	//--------------------------------------------------------------------------------------------------------
	static Ptr create ( const MyString &text, const ppiGui::Font & font = ppiGui::NORMAL_FONT ) {
		Ptr neu = Ptr ( new CMenuTitle ( text, font ) );
		neu->setPtr ( neu );
		return neu;
	}
	//--------------------------------------------------------------------------------------------------------
	virtual void draw ( CDrawContext *cc, const CPoint &mousePos );
	//--------------------------------------------------------------------------------------------------------
	virtual void mouse ( CDrawContext *cc, CPoint &p, long btn = -1 ) {}
};
//============================================================================================================
//	Klasse CMenuScrollSwitch
//  Ein MenuEintrag der einen Scroll Rocker Switch enthaelt. Verschiebt bei betaetigung das Menu Offset.
//============================================================================================================
class CMenuScrollSwitch : public CMenuEntry, public CControlListener {
private:
	//--------------------------------------------------------------------------------------------------------
	CRockerSwitch *rSwitch;
	//--------------------------------------------------------------------------------------------------------
	CMenu *parent;
protected:
public:
	//--------------------------------------------------------------------------------------------------------
	CMenuScrollSwitch ( CMenu *parent );
	//--------------------------------------------------------------------------------------------------------
	virtual void valueChanged ( CDrawContext *cd, CControl *cc );
	//--------------------------------------------------------------------------------------------------------
	virtual ~CMenuScrollSwitch ();
	//--------------------------------------------------------------------------------------------------------
	virtual void draw ( CDrawContext *cc, const CPoint &mousePos );
	//--------------------------------------------------------------------------------------------------------
	virtual void mouse ( CDrawContext *cc, CPoint &p, long btn );
};

//============================================================================================================
//	Klasse CNullEntry
//  Leerer Menueintrag.
//============================================================================================================
class CNullEntry : public CMenuEntry {
private:
protected:
public:
	//--------------------------------------------------------------------------------------------------------
	CNullEntry () : CMenuEntry ( VSTGUI::CRect(0,0,0,0) , Command::Ptr( new CmdNull() ) ){ /*this->setMouseEnabled(false);*/ };
	//--------------------------------------------------------------------------------------------------------
	CNullEntry ( VSTGUI::CRect &size ) : CMenuEntry ( size, Command::Ptr( new CmdNull() ) ){};
	//--------------------------------------------------------------------------------------------------------
	virtual void draw ( CDrawContext *cc, const CPoint &mousePos ){};
	//--------------------------------------------------------------------------------------------------------
	virtual ~CNullEntry(){}
};
} // namespace menu
} // namespace ppiGui

#endif


