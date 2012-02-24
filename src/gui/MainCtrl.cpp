/*
 * ===========================================================================================================
 * MainCtrl.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */

#include "MainCtrl.h"
#include "ViewCommand.h"
#include "CircuidView.h"
#include "PpiEditor.h"

namespace ppiGui{
//============================================================================================================
// Klasse MainCtrl:
// HauptControl der GUI enthaelt Menu / ToolBox / KontextMenu 
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
MainCtrl::MainCtrl( VSTGUI::CRect &size, void *pSystemWindow, void *pEditor ) : 
CFrame ( size, pSystemWindow, pEditor )
{
	toolBx = new MToolBox ( this );
	setupCtrl = new SetupCtrl ( this );
	toolBx->EventSender<ToolBoxBtnPressed>::addEventListener( this );
	setupCtrl->EventSender<OnClose>::addEventListener ( this );
}
//------------------------------------------------------------------------------------------------------------
bool MainCtrl::setSize (CCoord width, CCoord height) {
	Settings::Ptr set = SETTINGS;
	set->setWindowWidth ( width );
	set->setWindowHeight ( height );
	bool b = CFrame::setSize ( set->getWindowWidth(), set->getWindowHeight() );
	if ( !b ) return false;
	EventSender<OnResize>::
		notifyEventListeners ( this, VSTGUI::CRect(0,0,set->getWindowWidth(),set->getWindowHeight()) );
	return b;
}
//------------------------------------------------------------------------------------------------------------
MainCtrl::~MainCtrl(){
	delete toolBx;
	delete setupCtrl;
}
//------------------------------------------------------------------------------------------------------------
void MainCtrl::update( CDrawContext *cc ){
	CFrame::update ( cc );
}
//------------------------------------------------------------------------------------------------------------
void MainCtrl::eventHandler(void *src, const ToolBoxBtnPressed &ev) {
	if ( ev.id != MToolBox::tBtnSetup ) return;
	setupCtrl->show();
}
//------------------------------------------------------------------------------------------------------------
void MainCtrl::eventHandler(void *src, const OnClose &ev) {
	CControl *cc = dynamic_cast<CControl*> (toolBx->getObject ( MToolBox::tBtnSetup ) );
	if (!cc) return;
	cc->setValue(0.0f);
	cc->setDirty();
}
//============================================================================================================
// Klasse ButtonGroupListener:
// Es kann nur ein knopf gedreuckt werden.
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void ButtonGroupListener::valueChanged(CDrawContext *cD, CControl *pControl) {
	
	long tag = pControl->getTag();
	EventSender<ToolBoxBtnPressed>::notifyEventListeners ( this, ToolBoxBtnPressed(tag) );

	if (pControl->getValue() == 0.0f ) { //btn darf nicht ausgeschaltet werden
		pControl->setValue(1.0f);
		return;
	}

	Views::iterator it = views.begin();
	// gegenseitiges ausschliessen
	for ( ; it!=views.end(); ++it ) {
		if ( *it == pControl ) continue; 
		(*it)->setValue (0.0);
	}

}
//============================================================================================================
// Klasse MToolBox:
// MainToolBox enthaelt Buttons die die CircuidControl Steuern.
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void MToolBox::initGroups() {
	for ( int i=0; i<groups.size(); ++i ) {
		groups[i].EventSender<ToolBoxBtnPressed>::addEventListener ( this );
	}
}
//------------------------------------------------------------------------------------------------------------
void MToolBox::addControl ( CView *view ) {
	VSTGUI::CRect r; view->getViewSize ( r );
	r.offset( lastXPos, 0 );
	view->setViewSize ( r );
	view->setMouseableArea ( r );
	frame->addView ( view );
	lastXPos+=r.getWidth();
}
//------------------------------------------------------------------------------------------------------------
MToolBox::MToolBox( CFrame *frame ) : 
	CView (VSTGUI::CRect()), 
	lastXPos( 2 ), 
	frame(frame), 
	groups(1)
{
	MainCtrl *mCtrl = static_cast<MainCtrl*>( frame );
	mCtrl->EventSender<OnResize>::addEventListener ( this );
	//--------------------------------------------------------------------------------------------------------
	initGroups();
	//--------------------------------------------------------------------------------------------------------
	views = new CView*[tNum];
	//--------------------------------------------------------------------------------------------------------
	// Background
	CBitmap *bmp = resources->getResourceBitmap( Resources::TLBX_BK );
	VSTGUI::CRect size ( 0, 0, SETTINGS->getWindowWidth(), bmp->getHeight()+1 );
	size.offset( 1, 0 );
	setViewSize (size);
	setMouseableArea(size);
	//setBackground ( bmp );
	frame->addView ( this );
	//--------------------------------------------------------------------------------------------------------
	// Use Btn
	bmp = resources->getResourceBitmap( Resources::TLBX_BTN_USE );
	size = VSTGUI::CRect( 0, 0, bmp->getWidth(), bmp->getHeight()/2 );
	size.offset ( 0, 1 );
	CPoint p;
	views[tBtnUse] = new CMovieButton ( size, &groups[0], tBtnUse, bmp->getHeight()/2, bmp, p );
	( (CControl*)views[tBtnUse] )->setValue ( 1.0f );
	addControl ( views[tBtnUse] );
	groups[0].addControl ( (CControl*)views[tBtnUse] );
	//--------------------------------------------------------------------------------------------------------
	// Connect Btn
	bmp = resources->getResourceBitmap( Resources::TLBX_BTN_CNT );
	p = CPoint();
	views[tBtnConnect] = new CMovieButton ( size, &groups[0], tBtnConnect,  bmp->getHeight()/2, bmp, p );
	addControl ( views[tBtnConnect] );
	groups[0].addControl ( (CControl*)views[tBtnConnect] );
	//--------------------------------------------------------------------------------------------------------
	// Move Btn
	bmp = resources->getResourceBitmap( Resources::TLBX_BTN_MOVE );
	p = CPoint();
	views[tBtnMove] = new CMovieButton ( size, &groups[0], tBtnMove,  bmp->getHeight()/2, bmp, p );
	addControl ( views[tBtnMove] );
	groups[0].addControl ( (CControl*)views[tBtnMove] );
	//--------------------------------------------------------------------------------------------------------
	// Setup Btn
	bmp = resources->getResourceBitmap( Resources::TLBX_BTN_SETUP );
	size = VSTGUI::CRect( 0, 0, bmp->getWidth(), bmp->getHeight()/2 );
	CCoord x = SETTINGS->getWindowWidth() - bmp->getWidth();
	addSpace ( x - lastXPos );
	size.offset ( -1, 1 );
	p = CPoint();
	views[tBtnSetup] = new CMovieButton ( size, this, tBtnSetup,  bmp->getHeight()/2, bmp, p );
	addControl ( views[tBtnSetup] );
	//--------------------------------------------------------------------------------------------------------
}
//------------------------------------------------------------------------------------------------------------
void MToolBox::eventHandler(void *src, const ppiGui::OnResize &ev) {
	// resize view
	VSTGUI::CRect sz; getViewSize( sz );
	sz.setWidth ( ev.size.width() );
	setViewSize ( sz );
	setMouseableArea ( sz );
	// verschiebe settings btn
	CCoord x = SETTINGS->getWindowWidth() - views[tBtnSetup]->getWidth() - 1;
	VSTGUI::CRect r; views[tBtnSetup]->getViewSize (r);
	r.offset ( x - r.x, 0 );
	views[tBtnSetup]->setViewSize ( r );
	views[tBtnSetup]->setMouseableArea ( r );
	views[tBtnSetup]->setDirty();
}
//------------------------------------------------------------------------------------------------------------
MToolBox::~MToolBox() {
	for (int i=0; i<tNum; i++) delete views[i];
	delete views;
}
//--------------------------------------------------------------------------------------------------------
void MToolBox::draw ( CDrawContext *cD ) {
	VSTGUI::CRect sz; getViewSize ( sz );
	CColor c = { 190, 190, 190, 255 };
	cD->setFillColor ( c );
	cD->fillRect ( sz );
	setDirty(false);
}
//------------------------------------------------------------------------------------------------------------
void MToolBox::valueChanged(CDrawContext *cD, CControl *pControl) {
	long tag = pControl->getTag();
	switch ( tag ) {
		case tBtnSetup:
			pControl->setDirty();
			EventSender<ToolBoxBtnPressed>::notifyEventListeners ( this, ToolBoxBtnPressed(tag) );
			break;
	}
}
} // namespace ppiGui


