#include "ModalView.h"
#include "com/one4All.h"
#include "OS_Specific/OS_gui.h"
#include "ppiGui.h"
#include "MainCtrl.h"
#include <boost/foreach.hpp>

const CColor colBk = { 144, 162, 191, 255 };
const CColor colSysMenBk = { 170, 170, 170, 255 };
static const int SYS_MENU_HEIGHT = 25;
static const int SCANLISTBOX_MAX_NUM_ENTRIES = 31;

namespace ppiGui {
//============================================================================================================
// Klasse ModalView:
//============================================================================================================
ModalView::ModalView ( const VSTGUI::CRect &size, CFrame *frame )  : CViewContainer( size, frame ) {
	setTransparency( true );
	//setBackgroundColor ( lightBlue );

	MainCtrl *mCtrl = static_cast<MainCtrl*> ( frame );
	mCtrl->EventSender<OnResize>::addEventListener ( this );
}
//------------------------------------------------------------------------------------------------------------
void ModalView::addWindowView ( WindowView *win ) {
	addView ( win );
}
//------------------------------------------------------------------------------------------------------------
void ModalView::removeWindowView ( WindowView *win ) {
	removeView ( win, false );
}
//------------------------------------------------------------------------------------------------------------
void ModalView::eventHandler(void *src, const ppiGui::OnResize &ev) {
	VSTGUI::CRect neu = ev.size;
	setViewSize ( neu );
	setMouseableArea ( neu );
}
//============================================================================================================
// Klasse WindowView::WindowFrame
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void WindowView::WindowFrame::draw ( CDrawContext *cD ) {
	VSTGUI::CRect r; getViewSize(r);
	if ( bmp ) {
		bmp->drawTransparent ( cD, r );
		return;
	}
	r.left = 1;
	r.top = 1;
	// aeusserer rahmen mit fuellung
	cD->setFillColor ( colBk );
	cD->fillRect ( r );
	cD->setFrameColor ( ppiGui::black );
	cD->setLineWidth ( 2 );
	cD->drawRect ( r );
	// sys. menu rahmen mit fuellung
	r = VSTGUI::CRect ( 1,1, getWidth(), SYS_MENU_HEIGHT );
	cD->setFillColor ( colSysMenBk );
	cD->fillRect ( r );
	cD->drawRect ( r );
}
//============================================================================================================
// Klasse WindowView:
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
WindowView::WindowView ( CFrame *parent, const VSTGUI::CRect &size ) : 
	CViewContainer( size, parent ), 
	parent(parent)
{
	windowFrame = new WindowFrame ( size );
	// repos WindowView ( mittig )
	CCoord x = SETTINGS->getWindowWidth()/2 - getWidth()/2;
	CCoord y = SETTINGS->getWindowHeight()/2 - getHeight()/2;
	VSTGUI::CRect r = size;
	r.offset ( x, y );
	setViewSize ( r );
	setMouseableArea ( r );
	initControls();
}
//------------------------------------------------------------------------------------------------------------
WindowView::WindowView ( CFrame *parent, CBitmap *bk ) : 
	CViewContainer( VSTGUI::CRect(0,0,bk->getWidth(), bk->getHeight() ), parent ), 
	parent(parent)
{
	windowFrame = new WindowFrame ( bk );
	// repos WindowView ( mittig )
	CCoord x = SETTINGS->getWindowWidth()/2 - getWidth()/2;
	CCoord y = SETTINGS->getWindowHeight()/2 - getHeight()/2;
	VSTGUI::CRect r = size;
	r.offset ( x, y );
	setViewSize ( r );
	setMouseableArea ( r );
	initControls();
}
//------------------------------------------------------------------------------------------------------------
void WindowView::initControls() {
	// WindowFrame ========--------........
	addView ( windowFrame ); // 
	// Exit button========--------........
	CBitmap * bmp = resources->getResourceBitmap( Resources::DLG_SYSM_EXIT );
	exitBtnPos = VSTGUI::CRect ( 0, 0, bmp->getWidth(), bmp->getHeight()/2 ); 
	exitBtnPos.offset (10,8);
	CPoint p = CPoint();
	CView *v = new CMovieButton ( exitBtnPos, this, tSysMExit,  bmp->getHeight()/2, bmp, p );
	addView ( v );
}
//------------------------------------------------------------------------------------------------------------
WindowView::~WindowView() {
	this->removeView ( windowFrame );
}
//------------------------------------------------------------------------------------------------------------
void WindowView::valueChanged(CDrawContext *cD, CControl *pControl) {
	CPoint p; 
	switch ( pControl->getTag() ) {
		case tSysMExit:
			while ( cD->waitDrag() ) {} // warten bis maus losgelassen
			pControl->setValue(0.0f);
			pControl->setDirty();
			EventSender<OnClose>::notifyEventListeners( this, OnClose() );
			break;
		default:
			_valueChanged(cD, pControl);
	}
}
//------------------------------------------------------------------------------------------------------------
void WindowView::mouse( VSTGUI::CDrawContext *cD, VSTGUI::CPoint &p, long btn ) {
	VSTGUI::CRect r; getViewSize(r);
	CPoint lP = MyPoint(p) - MyPoint( r.left, r.top );
	if ( lP.y > SYS_MENU_HEIGHT || exitBtnPos.pointInside(lP) ) {
		CViewContainer::mouse ( cD, p, btn );
		return;
	}
	// hit im header => drag WindowView
	CPoint p2;
	while ( cD->waitDrag() ) {
		cD->getMouseLocation ( p2 );
		VSTGUI::CRect area; parent->getViewSize( area );
		if ( !area.pointInside(p2) ) continue;
		CPoint n = MyPoint (p2) - MyPoint(p);
		r.offset ( n.x, n.y );
		p = p2;
		setViewSize (r);
		setMouseableArea (r);
		parent->setDirty();
		parent->update(cD);
	}
}
//============================================================================================================
// Klasse ListBoxString:
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
ListBoxString::ListBoxString( const string &txt ) : col( black ) {
	if ( txt.length() > 0 ) setString(txt);
}
//------------------------------------------------------------------------------------------------------------
void ListBoxString::setString ( const string &str ) {
	static const int w = 10;
	static const int h = 14;
	txt = str;
	bBox = VSTGUI::CRect(0, 0, w * txt.length(), h );
}
//------------------------------------------------------------------------------------------------------------
void ListBoxString::draw( VSTGUI::CDrawContext *cD ) {
	VSTGUI::CRect r = bBox;
	r.offset( _offset.x, _offset.y ); 
	cD->setFontColor ( col );
	cD->setFont ( kNormalFont );
	cD->drawString ( txt.c_str(), r, false, kLeftText );
}
//============================================================================================================
// Klasse ListBox::ListBoxView
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
ListBox::ListBoxView::ListBoxView(const VSTGUI::CRect &rect) : CView ( rect ), selection(-1) {
}
//------------------------------------------------------------------------------------------------------------
ListBox::ListBoxView::~ListBoxView() {
}
//------------------------------------------------------------------------------------------------------------
void ListBox::ListBoxView::clear() {
	cntCol.clear();
}
//------------------------------------------------------------------------------------------------------------
void ListBox::ListBoxView::draw(VSTGUI::CDrawContext *cD) {
	VSTGUI::CRect r; getViewSize(r);
	cD->setFillColor ( white );
	cD->fillRect ( r );
	// draw entries
	CPoint p = CPoint( r.left, r.top );
	size_t i = 0;
	BOOST_FOREACH( ListBoxContent::Ptr cnt, cntCol ) {
		VSTGUI::CRect eSize = cnt->getViewSize();
		eSize.setWidth ( getWidth() );
		//selection
		if ( i == selection ) {
			eSize.offset( p.x, p.y );
			cD->setFillColor ( grey );
			cD->fillRect ( eSize );
		}
		
		cnt->offset( p.x, p.y );
		cnt->draw( cD );
		p.y+= eSize.getHeight();
		++i;
	}
}
//------------------------------------------------------------------------------------------------------------
void ListBox::ListBoxView::findSelection ( const CPoint & mP ) {
	VSTGUI::CRect r; getViewSize(r);
	// draw entries
	CPoint p = CPoint( r.left, r.top );
	size_t i = 0;
	BOOST_FOREACH( ListBoxContent::Ptr cnt, cntCol ) {
		VSTGUI::CRect eSize = cnt->getViewSize();
		eSize.setWidth ( getWidth() );
		eSize.offset( p.x, p.y );
		if ( eSize.pointInside ( mP ) ) {
			selection = i;
			break;
		}
		p.y+= eSize.getHeight();
		++i;
	}
}
//------------------------------------------------------------------------------------------------------------
void ListBox::ListBoxView::mouse( VSTGUI::CDrawContext *cD, VSTGUI::CPoint &p, long btn ) {
	findSelection ( p );
	setDirty();
}
//------------------------------------------------------------------------------------------------------------
void ListBox::ListBoxView::addListBoxContent ( const ListBoxContent::Ptr &ptr ) {
	cntCol.push_back ( ptr );
	// workaround for issue #116
	if ( cntCol.size() > SCANLISTBOX_MAX_NUM_ENTRIES ) {
		cntCol.pop_front();
	}
	// update view size
	VSTGUI::CRect r; getViewSize(r);
	r.bottom+= ptr->getViewSize().getHeight();
	int w = ptr->getViewSize().getWidth();
	r.right = ( w > r.right ) ? w : r.right;
	setViewSize( r );
	setMouseableArea( r );
}
//------------------------------------------------------------------------------------------------------------
ListBoxContent::Ptr ListBox::ListBoxView::getListBoxContent(size_t index) const {
	if ( index > cntCol.size() ) return ListBoxContent::Ptr();
	size_t i = 0;
	BOOST_FOREACH( ListBoxContent::Ptr cnt, cntCol ) {
		if ( i++ == index )
			return cnt;
	}
	return ListBoxContent::Ptr();
}
//------------------------------------------------------------------------------------------------------------
void ListBox::ListBoxView::removeEntry(size_t index) {
	if ( index > cntCol.size() ) return;
	ContentCollection::iterator it = cntCol.begin();
	size_t c = 0;
	while ( true ) {
		if ( c++ == index ) break;
		++it;
	}
	cntCol.erase ( it );

	selection = -1;
}
//------------------------------------------------------------------------------------------------------------
void ListBox::ListBoxView::changeEntry( size_t index, const ListBoxContent::Ptr &ptr ) {
	if ( index > cntCol.size() ) return;
	ContentCollection::iterator it = cntCol.begin();
	size_t c = 0;
	while ( true ) {
		if ( c++ == index ) break;
		++it;
	}
	cntCol.insert( it, ptr );
	cntCol.erase ( it );
	selection = -1;
}
//------------------------------------------------------------------------------------------------------------
VSTGUI::CRect ListBox::ListBoxView::calcSize() {
	VSTGUI::CRect r; getViewSize(r);
	r.bottom = 0;
	BOOST_FOREACH( ListBoxContent::Ptr cnt, cntCol ) {
		r.bottom+= cnt->getViewSize().getHeight();
		int w = cnt->getViewSize().getWidth();
		r.right = ( w > r.right ) ? w : r.right;
	}
	return r;
}
//============================================================================================================
// Klasse ListBox:
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
ListBox::ListBox(const VSTGUI::CRect &rect, CFrame *parent ) : 
	MyScrollView( rect, rect, parent, kHorizontalScrollbar | kVerticalScrollbar )
{
	lBView = new ListBoxView ( rect );
	addView ( lBView );
}
//------------------------------------------------------------------------------------------------------------
ListBox::~ListBox() {	
	lBView->clear();
	removeAll(); // !!
}
//------------------------------------------------------------------------------------------------------------
void ListBox::drawBackgroundRect(VSTGUI::CDrawContext *pContext, VSTGUI::CRect &_updateRect) {
	/*VSTGUI::CRect r = _updateRect;
	// aeusserer rahmen mit fuellung
	//pContext->setFillColor ( white );
	//pContext->fillRect ( r );
	pContext->setFrameColor ( ppiGui::black );
	pContext->setLineWidth ( 1 );
	pContext->drawRect ( r );*/
}
//------------------------------------------------------------------------------------------------------------
int ListBox::find(const std::string &str) {
	const ContentCollection &lCnt = lBView->getContentCollection();
	ContentCollection::const_iterator it = lCnt.begin();
	int c=0;
	for ( ; it!=lCnt.end(); ++it ) {
		ListBoxString *cntStr = dynamic_cast<ListBoxString*>( it->get() );
		if (!cntStr) continue;
		if ( cntStr->getString() == str ) return c;
		++c;
	}
	return -1;
}
//------------------------------------------------------------------------------------------------------------
void ListBox::scrollToVEnd() {
	CPoint offset;
	float value = 1.0f;
	VSTGUI::CRect vsize = sc->getViewSize (vsize);
	VSTGUI::CRect csize = sc->getContainerSize ();
	offset.y = csize.top + (CCoord)((csize.height () - vsize.height ()) * value);
	sc->setScrollOffset (offset, false);
	vsb->setValue ( value );
}
//------------------------------------------------------------------------------------------------------------
void ListBox::addString( const string &str ) {
	sc->setScrollOffset (CPoint(), false);
	lBView->addListBoxContent ( ListBoxString::Ptr( new ListBoxString( str ) ) );
	setContainerSize ( lBView->calcSize() );
	scrollToVEnd();
}
//------------------------------------------------------------------------------------------------------------
string ListBox::getString( size_t index ) const {
	ListBoxContent::Ptr cnt = lBView->getListBoxContent( index );
	ListBoxString *str =  dynamic_cast<ListBoxString*>( cnt.get() );
	if ( str ) return str->getString();
	return "";
}
//------------------------------------------------------------------------------------------------------------
void ListBox::changeEntry ( size_t index, const string &str ) {
	lBView->changeEntry ( index, ListBoxString::Ptr( new ListBoxString( str ) ) );
}
}// ppiGui