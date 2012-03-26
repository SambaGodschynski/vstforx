/*
 * ===========================================================================================================
 * CMenu.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */

#include "CMenu.h"
#include "ConcreteGObjects.h"
#include "MainCtrl.h"
#include "ppiGui.h"
#include "PpiEditor.h"
#include "boost/shared_ptr.hpp"

extern void releaseCReference ( CReferenceCounter *ref );

static const double MENU_DELAY_SEC=0.5;

namespace { //colors
	const VSTGUI::CColor COLOR_SELECTION = { 155, 0, 0, 255 };
	const VSTGUI::CColor COLOR_TITLE_BK = { 144, 162, 191, 255 };
	const VSTGUI::CColor COLOR_MENU_BK = { 100, 100, 100, 255 };
	const VSTGUI::CColor COLOR_TEXT = {180, 180, 200, 255};
	const VSTGUI::CColor COLOR_TITLE_TEXT = { 12, 12, 12, 255 };
	const VSTGUI::CColor COLOR_PARAMETER_BAR = { 155, 155, 155, 255 };
	const VSTGUI::CColor COLOR_MENU_FRAME = { 12, 12, 12, 255 };
}


namespace ppiGui{
namespace menu{
//============================================================================================================
//	Klasse CMenu:
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
CMenu::CMenu ( CFrame *frame ) : CView(VSTGUI::CRect ()), hold(false), subMenu(NULL) {
	CView::pParentFrame = frame;
	entr_offset = 0;
	scrollSwitch = new CMenuScrollSwitch( this );
	nullEntry = new CNullEntry();
	subMenuListener.parent = this;
	scroller = nullEntry;
}
//------------------------------------------------------------------------------------------------------------
CMenu::Ptr CMenu::create ( CFrame *frame ) {
	CMenu::Ptr _new ( new CMenu ( frame ) );
	_new->self = _new;
	return _new;
}
//------------------------------------------------------------------------------------------------------------
void CMenu::showAt(const CPoint &p){
	if (menuEntries.empty()) 
		return;
	if ( isVisible() ) 
		return;
	getFrame()->addView ( this );
	MenuEntryList::iterator it = menuEntries.begin();
	whenVisible = com::events::TrackingDummy::create();
	for (; it!=menuEntries.end(); ++it) {
		(*it)->EventSender<OnMouseClick>::addTrackedEventListener (this, whenVisible);
		CSubMenuEntry *sub = dynamic_cast <CSubMenuEntry*> ( (*it).get() );
		if ( sub ) { //wenn entry == submenuEntry
			sub->EventSender<OnMouseEnter>::addTrackedEventListener ( &subMenuListener, whenVisible );
			sub->EventSender<OnMouseLeave>::addTrackedEventListener ( &subMenuListener, whenVisible );
			sub->EventSender<ShowSubMenu>::addTrackedEventListener ( this, whenVisible );
			sub->EventSender<HideSubMenu>::addTrackedEventListener ( this, whenVisible );
		}
	}
	//moveTo ( p );
	VSTGUI::CRect tmp = calcSize();
	setOutline( tmp, calcMenuPos(tmp, p) );
	( (PpiEditor*)getFrame()->getEditor() )->EventSender<OnIdle>::addTrackedEventListener ( this, whenVisible );
	setDirty();
}
//------------------------------------------------------------------------------------------------------------
void CMenu::hide() {
	if ( !isVisible() ) 
		return;
	whenVisible.reset(); // tracking dummy removes listener on dispose
	//boost::lock_guard<boost::mutex> lock(onClickMutex);
	MenuEntryList::iterator it = menuEntries.begin();
	for (; it!=menuEntries.end(); ++it) {
		CSubMenuEntry *sub = dynamic_cast <CSubMenuEntry*> ( (*it).get() );
		if ( sub ) { // wenn: CSubMenuEntry
			sub->resetDelay();
			sub->getSubMenu()->hide();
		}
	}
	hold = false;
	currMousePos = CPoint();
	focusTrigger = ClockEdge();
	entr_offset = 0;
	clearEntries();
	scroller = nullEntry;
	PpiEditor *ed = (PpiEditor*)getFrame()->getEditor();
	ed->addCommand ( Command::Ptr ( new CmdRemoveMenu( getPtr() ) ) );
	
}
//------------------------------------------------------------------------------------------------------------
void CMenu::draw ( CDrawContext *cc ) {
	if (!isVisible()) 
		return;
	drawFrame ( cc, size );
	drawEntries ( cc );
	setDirty(false);
}
//------------------------------------------------------------------------------------------------------------
VSTGUI::CRect CMenu::calcSize() {
	if (menuEntries.empty()) 
		VSTGUI::CRect();
	if (menuEntries.size() > (maxEntries+1) ) 
		scroller = scrollSwitch;
	else scroller = nullEntry;
	MenuEntryList::iterator it = menuEntries.begin();
	int i=0; //entr_offset cnt 
	int n=0; //max entrie cnt
	VSTGUI::CRect tmp;
	tmp.setWidth ( 85 ); // minimale breite
	for ( ; it!=menuEntries.end(); ++it ) {
		if ( i++ < entr_offset ) 
			continue; // ueberspringe eintraege < offset
		CMenuEntry *view = (*it).get();
		if ( tmp.width() < view->getWidth() ) 
			tmp.setWidth ( view->getWidth() ); // akt. max. breite
		// addiere Hoehe wenn maxEntries noch nicht erreicht.
		if ( n++ <= maxEntries ) 
			tmp.setHeight ( view->getHeight() + tmp.height() + 1 );
	}
	tmp.setHeight ( tmp.getHeight() + scroller->getHeight() ); 
	return tmp;
}
//------------------------------------------------------------------------------------------------------------
CPoint CMenu::calcMenuPos( const VSTGUI::CRect &menuSize, const CPoint &_where ) {
	// moegl. rel. positionen	
	enum { LEFT=1, RIGHT=2, UP=4, DOWN=8 };
	int pos;
	// find raus wo platz ist:
	if ( _where.x + menuSize.width() > SETTINGS->getWindowWidth() ) { // => links
		pos = LEFT;
	} else {
		pos = RIGHT;
	}
	if ( _where.y + menuSize.height() > SETTINGS->getWindowHeight() ) { // => oben
		pos |= UP;
	} else { // unten
		pos |= DOWN;
	}
	CPoint p;
	int right = _where.x - 5;
	int left  = _where.x - menuSize.width() + 5;
	int up    = _where.y - menuSize.height();
	int down  = _where.y;
	switch ( pos ) {
		case RIGHT | DOWN  :  p = CPoint( right, down ); break;
		case LEFT  | DOWN  :  p = CPoint( left, down ); break;
		case RIGHT | UP    :  p = CPoint( right, up ); break;
		case LEFT  | UP    :  p = CPoint( left, up ); break;
	}
	return p;
}
//------------------------------------------------------------------------------------------------------------
void CMenu::setOutline ( const VSTGUI::CRect &sz, const CPoint &point ) {
	VSTGUI::CRect size(sz);
	size.offset ( point.x - size.x, point.y - size.y ); // bewege nach point
	setViewSize ( size );
	setMouseableArea (size);
}
//------------------------------------------------------------------------------------------------------------
void CMenu::scrollDwn(){
	int c =  ( menuEntries.size()-1 ) - maxEntries;
	if ( entr_offset  < c ) {
		entr_offset++;
	}
	setDirty();
}
//------------------------------------------------------------------------------------------------------------
void CMenu::scrollUp(){
	if ( entr_offset > 0 ) {
		entr_offset--;
	}
	setDirty();
}
//------------------------------------------------------------------------------------------------------------
void CMenu::drawEntries(CDrawContext *cc){
	static const int margin = 0;
	// zeichne eintraege
	MenuEntryList::iterator it = menuEntries.begin();
	VSTGUI::CRect tmp;
	CPoint p = CPoint (size.x, size.top); 
	int i=0;
	int n=0;
	for ( ; it!=menuEntries.end(); ++it ) {
		if ( i++ < entr_offset ) continue;
		if ( n++ > maxEntries ) break;
		CMenuEntry *view = (*it).get();
		view->getViewSize ( tmp );
		tmp.offset ( p.x - tmp.x, p.y - tmp.y );
		tmp.setWidth ( size.width() );
		p.y = tmp.bottom + 1; // eins weiter
		view->setViewSize ( tmp );
		if ( view->hitTest ( currMousePos ) ) lastMenuPos = currMousePos;
		view->draw ( cc, currMousePos );
		cc->setLineStyle ( kLineOnOffDash );
		cc->moveTo ( CPoint ( size.left + margin , tmp.top ) ); // trenner
		cc->lineTo ( CPoint ( size.right - margin , tmp.top ) );
		cc->setLineStyle ( kLineSolid );
	}
	CMenuEntry *view = scroller;
	view->getViewSize ( tmp );
	tmp.offset ( p.x - tmp.x, p.y - tmp.y );
	tmp.setWidth ( size.width() );
	p.y = tmp.bottom + 1; // eins weiter
	view->setViewSize ( tmp );
	view->draw ( cc, currMousePos );
}
//------------------------------------------------------------------------------------------------------------
void CMenu::drawFrame ( CDrawContext *cc, VSTGUI::CRect &size ){
	cc->setLineStyle ( kLineSolid );
	cc->setLineWidth ( 1 );
	cc->setFrameColor ( COLOR_MENU_FRAME );
	cc->setFillColor ( COLOR_MENU_BK );
	cc->fillRect ( size );
	cc->drawRect ( size );
}
//------------------------------------------------------------------------------------------------------------
void CMenu::eventHandler ( void *src, const OnIdle &ev ) { // OnIdle
	if ( !isVisible() ) return;
	if ( getFrame()->getCurrentView() != this ) return;
	boost::shared_ptr<CDrawContext> cD ( getFrame()->createDrawContext(), releaseCReference );
	CPoint p;
	cD->getMouseLocation( p );
	VSTGUI::CRect tmp; getViewSize(tmp);
	const int OVERLAP = -5; 
	tmp.inset ( OVERLAP, OVERLAP );
	ClockEdge::EdgeValue trigger = focusTrigger.in ( tmp.pointInside ( p ) );
	// OnEnter, OnLeave events:
	switch ( trigger ) {
		case ClockEdge::HIGH :
			hold = false;
			EventSender<OnMouseEnter>::notifyEventListeners 
				( this, OnMouseEnter( NULL, cD.get(), currMousePos, -1 ) );
			break;
		case ClockEdge::LOW  :
			hold = true;
			EventSender<OnMouseLeave>::notifyEventListeners 
				( this, OnMouseLeave( NULL, cD.get(), currMousePos, -1 ) );
			break;
	}
	if ( !hold ) currMousePos = p;
	else currMousePos = lastMenuPos;
	setDirty();
}
//------------------------------------------------------------------------------------------------------------
void CMenu::eventHandler ( void *src, const OnMouseClick &ev ) {
	//if ( ev.btn
	CMenuEntry *m = (CMenuEntry*) src;
	Command::Ptr cmd = m->getAction();
	Ptr hold = self.lock(); 
	if (!hold) return;
	EventSender<OnMouseClick>::notifyEventListeners ( this, ev );
	hide();
	cmd->execute();
	getFrame()->setDirty();
}
//------------------------------------------------------------------------------------------------------------
void CMenu::eventHandler ( void *src, const OnMouseEnter &ev ) { 
}
//------------------------------------------------------------------------------------------------------------
void CMenu::eventHandler ( void *src, const ShowSubMenu &ev ) { 
	CSubMenuEntry *se = static_cast<CSubMenuEntry*>(src);
	//subMenu = ev.menu;
	ev.menu->showAt( se->getSubMenuPos() );
}
//------------------------------------------------------------------------------------------------------------
void CMenu::eventHandler ( void *src, const HideSubMenu &ev ) { 
	ev.menu->hide();
}
//------------------------------------------------------------------------------------------------------------
void CMenu::moveTo ( CPoint &p ){
	VSTGUI::CRect tmp;
	getViewSize ( tmp );
	tmp.offset ( p.x - size.x, p.y - size.y );
	setViewSize ( tmp );
	setMouseableArea ( tmp );
}
//------------------------------------------------------------------------------------------------------------
CMenu::~CMenu(){
	//hide();
	delete scrollSwitch;
	delete nullEntry;
}
//------------------------------------------------------------------------------------------------------------
void CMenu::SubMenuListener::eventHandler ( void *src, const OnMouseEnter &ev ) {
	CSubMenuEntry *sb = static_cast<CSubMenuEntry*>(src);
	VSTGUI::CRect r; sb->getViewSize(r);
	parent->currMousePos = CPoint( r.x+1, r.y+1 );
}
//------------------------------------------------------------------------------------------------------------
void CMenu::SubMenuListener::eventHandler ( void *src, const OnMouseLeave &ev ) {
}
//------------------------------------------------------------------------------------------------------------
void CMenu::SubMenuListener::eventHandler ( void *src, const OnMouseClick &ev ) {
	parent->hide();
}
//------------------------------------------------------------------------------------------------------------
void CMenu::addMenuEntry ( MenuEntryPtr entry ){ 
	menuEntries.push_back ( entry );
    entr_offset = 0;
}
//------------------------------------------------------------------------------------------------------------
void CMenu::addMenuEntryList( MenuEntryList &list ){
	MenuEntryList::iterator it = list.begin();
	for ( ; it!=list.end(); ++it ) {
		addMenuEntry (*it);
	}
}
//------------------------------------------------------------------------------------------------------------
void CMenu::clearEntries(){
	MenuEntryList::iterator it = menuEntries.begin();
	while ( it!=menuEntries.end() ) {
		it = menuEntries.erase( it );
	}
	assert ( menuEntries.empty() );
	size = VSTGUI::CRect(0,0,0,0);
	this->setMouseableArea ( size );
}
//------------------------------------------------------------------------------------------------------------
void CMenu::mouse(CDrawContext *cc, CPoint &p, long btn ) {
	Ptr hold = self.lock();
	if ( !isVisible() ) return;
	if ( scrollSwitch->hitTest ( p ) ) {
		scrollSwitch->mouse(cc,p,btn);
		return;
	}
	MenuEntryList holding_copy = menuEntries; // <=!!menu kann verschwinden waehrend noch iteriert wird
	MenuEntryList::iterator it = holding_copy.begin();
	for ( ; it!=holding_copy.end(); ++it ){
		(*it)->mouse( cc, p, btn );
	}
}
//============================================================================================================
//	Klasse CSubMenu:
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
CMenu::Ptr CSubMenu::create ( CFrame *frame ) {
	Ptr _new ( (CMenu*)new CSubMenu ( frame ) );
	_new->self = _new;
	return _new;
}
//============================================================================================================
//	Klasse CMenuEntry:
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void CMenuEntry::mouse(CDrawContext *cc, CPoint &p, long btn ){
	if ( btn==kLButton && hitTest (p) ) 
		EventSender<OnMouseClick>::notifyEventListeners ( this, OnMouseClick ( NULL, cc, p, btn ) );
}
//============================================================================================================
//	Klasse CMenuLabel:
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
CMenuLabel::CMenuLabel(const com::MyString &text, Command::Ptr action, const ppiGui::Font & font ): 
CMenuEntry ( VSTGUI::CRect(), action ), font ( font )
{
	setText ( text );
	color = COLOR_TEXT;
}
//------------------------------------------------------------------------------------------------------------
void CMenuLabel::setText ( const MyString &_text ) {
	text = _text.trim();
	text = text.shorten(Settings::MAX_STR_MENU_LABEL, MyString::kRight);
	int w = 0;
	int h = 0;
	switch ( font ){
		case kSystemFont : w = 7; h = 19;  break;
		case kNormalFontVeryBig  : w = 10; h = 21; break;
		case kNormalFontBig  : w = 9; h = 19; break;
		case kNormalFont  : w = 8; h = 19; break;
		case kNormalFontSmall : w = 6; h = 15; break;
		case kNormalFontSmaller : w = 5; h = 13; break;
		case kNormalFontVerySmall : w = 6; h = 15; break;
		case kSymbolFont : w = 6; h = 15; break;
	}
	VSTGUI::CRect vZ (0, 0, w * text.length(), h );
	setViewSize (vZ);
}
//------------------------------------------------------------------------------------------------------------
void CMenuLabel::draw( CDrawContext *cc, const CPoint &mousePos ){
	if (hitTest(mousePos)){ // eintrag selektiert
		cc->setFillColor ( COLOR_SELECTION );
		VSTGUI::CRect tmp = size;
		tmp.setWidth ( tmp.width() - 1 );
		tmp.setHeight ( tmp.height() + 1 );
		cc->fillRect ( tmp );
	}
	cc->setFontColor ( color );
	cc->setFont ( font );
	cc->drawString ( (" #" + text).c_str(), size, false, kLeftText );
}
//============================================================================================================
//	Klasse CParameterEntry:
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
CParameterEntry::CParameterEntry(Parameter::Ptr par, Command::Ptr action, const ppiGui::Font & font ): 
CMenuLabel ( MyString(), action, font ), par (par), onDrag ( false )
{
	setText ( getText() );
}
//------------------------------------------------------------------------------------------------------------
CParameterEntry::~CParameterEntry() {
}
//------------------------------------------------------------------------------------------------------------
void CParameterEntry::mouse(CDrawContext *cc, CPoint &p, long btn ){
	if ( btn != VSTGUI::kLButton ) return;
	if (!hitTest(p)) return;
	if ( !par ) return;
	if ( cc->waitDrag() ) {
		VSTGUI::CRect tmp = size;
		dragPos = p;
		onDrag = true;
		while ( cc->waitDrag() ) {
			cc->getMouseLocation(p);
			*par = (float)( p.x - tmp.left ) / tmp.width();
		}
		onDrag = false;
	}
	else { // es hatt klick gemacht 
		CMenuEntry::mouse ( cc, p, btn );
		par.reset();
	}
}
//------------------------------------------------------------------------------------------------------------
void CParameterEntry::drawBar( CDrawContext *cc, const CColor &col ) {
	VSTGUI::CRect tmp = size;
	cc->setFillColor ( col );
	CCoord w = (*par==0.0) ? 1 : (CCoord)( (tmp.width() - 1) * *par );
	tmp.setWidth ( w );
	tmp.setHeight ( tmp.height() + 1 );
	cc->fillRect ( tmp );
}
//------------------------------------------------------------------------------------------------------------
void CParameterEntry::draw( CDrawContext *cc, const CPoint &mousePos ){
	if ( !par ) return;
	CColor barCol = COLOR_PARAMETER_BAR;
	CColor textCol = color;
	//if ( onDrag ) mousePos = dragPos;
	if (hitTest(mousePos)){ // eintrag selektiert
		VSTGUI::CRect tmp = size;
		cc->setFillColor ( ppiGui::darkRed );
		tmp.setWidth ( tmp.width() - 1 );
		tmp.setHeight ( tmp.height() + 1 );
		cc->fillRect ( tmp );
		barCol = COLOR_SELECTION;
		brightness ( textCol, 2.5 );
	}
	brightness ( barCol, 0.5 );
	drawBar ( cc, barCol );
	cc->setFontColor ( textCol );
	cc->setFont ( font );
	cc->drawString ( (" #" + getText() ).c_str(), size, false, kLeftText );
}
//============================================================================================================
//	Klasse CSubMenuEntry:
//============================================================================================================
// CSubMenuEntry ( const MyString &text, const CMenu::Ptr &subMenu, const ppiGui::Font & font );
//------------------------------------------------------------------------------------------------------------
CSubMenuEntry::CSubMenuEntry ( const MyString &text, const CMenu::Ptr &subMenu, const ppiGui::Font & font  ) : 
	CMenuLabel ( text, Command::Ptr(new CmdNull()), font ),
	subMenu (subMenu),
	delayedClockEdge ( MENU_DELAY_SEC )
{
	mL = subMenu->getMenuEntries();
}
//------------------------------------------------------------------------------------------------------------
CSubMenuEntry::~CSubMenuEntry() {
}
//------------------------------------------------------------------------------------------------------------
void CSubMenuEntry::eventHandler(void *scr, const ppiGui::OnMouseEnter &ev) {
	EventSender<OnMouseEnter>::notifyEventListeners ( this, ev );
}
//------------------------------------------------------------------------------------------------------------
void CSubMenuEntry::eventHandler(void *scr, const ppiGui::OnMouseLeave &ev) {
	EventSender<OnMouseLeave>::notifyEventListeners ( this, ev );
}
//------------------------------------------------------------------------------------------------------------
void CSubMenuEntry::eventHandler(void *scr, const ppiGui::OnMouseClick &ev) {
	Ptr hold = self.lock();
	EventSender<OnMouseClick>::notifyEventListeners ( this, ev );
}
//------------------------------------------------------------------------------------------------------------
CPoint CSubMenuEntry::calcMenuPos() {
	// moegl. rel. positionen	
	enum { LEFT=1, RIGHT=2, UP=4, DOWN=8 };
	int pos;
	VSTGUI::CRect entrySize; getViewSize( entrySize );
	VSTGUI::CRect menuSize = subMenu->calcSize();
	// find raus wo platz ist:
	if ( entrySize.right + menuSize.width() > SETTINGS->getWindowWidth() ) { // => links
		pos = LEFT;
	} else {
		pos = RIGHT;
	}
	if ( entrySize.bottom + menuSize.height() > SETTINGS->getWindowHeight() ) { // => oben
		pos |= UP;
	} else { // unten
		pos |= DOWN;
	}
	CPoint p;
	int right = entrySize.right - 5;
	int left  = entrySize.left - menuSize.width() + 10;
	int up    = entrySize.top - menuSize.height();
	int down  = entrySize.top;
	switch ( pos ) {
		case RIGHT | DOWN  :  p = CPoint( right, down ); break;
		case LEFT  | DOWN  :  p = CPoint( left, down ); break;
		case RIGHT | UP    :  p = CPoint( right, up ); break;
		case LEFT  | UP    :  p = CPoint( left, up ); break;
	}
	return p;
}
//------------------------------------------------------------------------------------------------------------
void CSubMenuEntry::resetDelay() {
	delayedClockEdge = DelayedClockEdge( MENU_DELAY_SEC );
}
//------------------------------------------------------------------------------------------------------------
void CSubMenuEntry::draw( CDrawContext *cc, const CPoint &mousePos ) {
	// mouse entry
	bool hit = hitTest(mousePos);
	if (hit){ // eintrag selektiert
		cc->setFillColor ( COLOR_SELECTION );
		VSTGUI::CRect tmp = size;
		tmp.setWidth ( tmp.width() - 1 );
		tmp.setHeight ( tmp.height() + 1 );
		cc->fillRect ( tmp );
	}

	// mouseEntry flanken wechsel nach HIGH
	ClockEdge::EdgeValue state = delayedClockEdge.in ( hit );

	if ( state == ClockEdge::HIGH ) {
		if ( !subMenu->isVisible() ) {
			if ( subMenu->entriesEmpty() ) {
				subMenu->addMenuEntryList ( mL );
			}
			subMenuPos = calcMenuPos();
			EventSender<ShowSubMenu>::notifyEventListeners ( this, ShowSubMenu( subMenu.get() ) );
			subMenu->EventSender<OnMouseEnter>::addTrackedEventListener(this, subMenu->whenVisible);
			subMenu->EventSender<OnMouseLeave>::addTrackedEventListener(this, subMenu->whenVisible);
			subMenu->EventSender<OnMouseClick>::addTrackedEventListener(this, subMenu->whenVisible);
		}
	}
	
	// mouseEntry flanken wechsel nach LOW
	if ( state == ClockEdge::LOW ) {
		EventSender<HideSubMenu>::notifyEventListeners ( this, HideSubMenu( subMenu.get() ) );
	}
	
	// zeichne eintrag
	cc->setFontColor ( color );
	cc->setFont ( font );
	cc->drawString ( (" +" + getText() ).c_str(), size, false, kLeftText );
	cc->setFont ( kNormalFontBig );
	cc->drawString ( ">  " , size, false, kRightText );
	//setDirty (false);
}
//============================================================================================================
//	Klasse CMenuTitle:
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
CMenuTitle::CMenuTitle(const com::MyString &text, const ppiGui::Font & font ) : 
	CMenuLabel ( text, Command::Ptr(new CmdNull()), font ) 
{
	setColor ( COLOR_TITLE_TEXT );
}
//------------------------------------------------------------------------------------------------------------
void CMenuTitle::draw( CDrawContext *cc, const CPoint &mousePos ){
	cc->setFillColor ( COLOR_TITLE_BK );
	cc->setFrameColor ( COLOR_MENU_FRAME );
	cc->setLineWidth ( 1 );
	cc->setLineStyle ( kLineSolid );
	VSTGUI::CRect tmp = size;
	tmp.setHeight( tmp.getHeight() + 2 );
	cc->fillRect ( tmp );
	cc->drawRect ( tmp );
	cc->setFontColor ( color );
	cc->setFont ( font );
	cc->drawString ( getText().c_str(), size, false, kCenterText );
}
//============================================================================================================
//	Klasse CMenuScrollSwitch
//  Ein MenuEintrag der einen Scroll Rocker Switch enthaelt. Verschiebt bei betaetigung das Menu Offset.
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void CMenuScrollSwitch::mouse ( CDrawContext *cc, CPoint &p, long btn ){
	ppiGui::GCView *hack = (ppiGui::GCView*)rSwitch;
	hack->setFrame ( parent->getFrame() );
	hack->mouse(cc, p, btn);
}
//------------------------------------------------------------------------------------------------------------
CMenuScrollSwitch::CMenuScrollSwitch( CMenu *parent ) : 
	parent(parent),
	CMenuEntry ( VSTGUI::CRect(), Command::Ptr(new CmdNull()) ) 
{
	CBitmap *bmp = resources->getResourceBitmap(Resources::MENU_ROCKER);
	VSTGUI::CRect sz ( 0, 0, bmp->getWidth(), bmp->getHeight()/3 );
	CPoint p;
	rSwitch = new CRockerSwitch ( sz, this, 0, 10, bmp, p );
	setViewSize ( sz );
}
//------------------------------------------------------------------------------------------------------------
void CMenuScrollSwitch::draw(CDrawContext *cc, const CPoint &mousePos){
	VSTGUI::CRect sz;
	rSwitch->getViewSize ( sz );
	sz.offset ( size.x - sz.x, size.y - sz.y );
	sz.offset (  (size.getWidth() - sz.width()), 0 );
	rSwitch->setViewSize ( sz );
	rSwitch->setMouseableArea ( sz );
	rSwitch->draw ( cc );
}
//------------------------------------------------------------------------------------------------------------
CMenuScrollSwitch::~CMenuScrollSwitch(){
	delete rSwitch;
}
//------------------------------------------------------------------------------------------------------------
void CMenuScrollSwitch::valueChanged(CDrawContext *cd, CControl *cc){
	rSwitch->setDirty();
	if ( cc->getValue() == -1.0 ){ // runter
		while ( cd->getMouseButtons() == kLButton  ){
			parent->scrollDwn();
			rSwitch->getFrame()->doIdleStuff();
			((AEffGUIEditor*) rSwitch->getEditor())->wait ( 100 );
		}
	}
	if ( cc->getValue() == 1.0 ){ // hoch
		while ( cd->getMouseButtons() == kLButton  ){
			parent->scrollUp();
			rSwitch->getFrame()->doIdleStuff();
			((AEffGUIEditor*) rSwitch->getEditor())->wait ( 100 );
		}
	}
	//parent->getFrame()->setFocusView ( parent );
}
} // namespace menu
} // namespace ppiGui


