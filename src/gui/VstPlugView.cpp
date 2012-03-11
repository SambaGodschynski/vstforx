/*
 * ===========================================================================================================
 * VstPlugView.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */

#include "VstPlugView.h"
#include "ConcreteGObjects.h"
#include "CircuidView.h"
#include "com/Settings.h"
#include "PpiEditor.h"


namespace ppiGui{
//============================================================================================================
//	Klasse VSTPlugView:
//  Oeffnet ein neues Fenster und zeigt externen AEffGUIEditor Editor an. 
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
VSTPlugView::VSTPlugView( PpiEditor *ed, processing::VSTPlugin::Ptr vstPlugNode ) : 
vstPlugNode(vstPlugNode),
editor (ed)
{
	using namespace processing::parameter;
	aEff = vstPlugNode->getAEffect();
	if (!vstPlugNode->can (effFlagsHasEditor)) 
		throw ppiError::IllegalOperation ("Effect has no Editor.", __FILE__, __LINE__ );
}
//------------------------------------------------------------------------------------------------------------
void VSTPlugView::initListener() {
	vstPlugNode->EventSender<processing::ResizeEditorEvent>::addTrackedEventListener (this, self);
}
//------------------------------------------------------------------------------------------------------------
VSTPlugView::Ptr VSTPlugView::create( PpiEditor *ed, processing::VSTPlugin::Ptr vstPlugNode ) {
	Ptr neu = Ptr ( new VSTPlugView ( ed, vstPlugNode ) );
	neu->self = neu;
	neu->initListener();
	return neu;
}
//------------------------------------------------------------------------------------------------------------
VSTPlugView::~VSTPlugView(){
}
//------------------------------------------------------------------------------------------------------------
void VSTPlugView::openWindow( size_t posX, size_t posY ) {
	TRY_TO_LOCK_TIMED(mutex);
	if ( isOpen() ) return;
	// problems with absynth3 editor:
	// effEditGetRect returns invalid rect, sizing runs with ResizeEditorEvent.
	// solution:
	// first call effEditGetRect then effEditOpen.
	
	// create window
	win = createWindow ( 
		VSTGUI::CRect (0,0,300,300), 
		Window::TITLE_BAR | Window::SYSTEM_MENU,
		editor->getSystemWindow()
	);
	whileOpen = com::events::TrackingDummy::create();
	win->EventSender<OnOpen> ::addTrackedEventListener (this, whileOpen);
	win->EventSender<OnClose>::addTrackedEventListener (this, whileOpen);
	win->EventSender<OnMoving>::addTrackedEventListener (this, whileOpen);
	win->setCaption ( vstPlugNode->getPlugName() );
	ERect *size = NULL;
	// get editor size
	aEff->dispatcher ( aEff, effEditGetRect, 0, 0, &size, 0);
	// set size
	if ( size )
		win->setSize ( VSTGUI::CRect ( size->left, size->top, size->right, size->bottom ) );
	// show window
	win->showWindow( CPoint(posX, posY) );
	// call effect: open 
	aEff->dispatcher ( aEff, effEditOpen, 0, 0, win->getHandle(), 0);
	// add event listener: IDLE
	editor->EventSender<OnIdle>::addTrackedEventListener (this, whileOpen);
}
//------------------------------------------------------------------------------------------------------------
void VSTPlugView::closeWindow() {
	if (win) win->closeWindow(); // loest Event::OnClose auf 
}
//------------------------------------------------------------------------------------------------------------
void VSTPlugView::eventHandler( void *scr, const OnIdle &ev ) {
	boost::unique_lock<boost::timed_mutex> lock(mutex, boost::try_to_lock);
	if ( !lock.owns_lock() ) return; 
	aEff->dispatcher ( aEff, effEditIdle, 0, 0, 0, 0);
}
//------------------------------------------------------------------------------------------------------------
void VSTPlugView::eventHandler( void *scr, const OnOpen &ev ) {
	// notify OnOpen
	EventSender<OnOpen>::notifyEventListeners( this, OnOpen() );
}
//------------------------------------------------------------------------------------------------------------
void VSTPlugView::eventHandler( void *scr, const OnClose &ev ) {
	TRY_TO_LOCK_TIMED(mutex);
	if ( !isOpen() ) return;
	// listener entfernen via tracking-objekt
	whileOpen.reset();
	// EditClose
	aEff->dispatcher ( aEff, effEditClose, 0, 0, win->getHandle(), 0);
	// notify OnClose
	EventSender<OnClose>::notifyEventListeners( this, OnClose() );
}
//------------------------------------------------------------------------------------------------------------
void VSTPlugView::eventHandler( void *scr, const processing::ResizeEditorEvent &ev ) {
	VSTGUI::CRect r = win->getSize();
	r.setWidth( ev.w );
	r.setHeight( ev.h );
	CPoint pos = win->getPos();
	win->setSize( r );
	win->setPos ( pos );
	EventSender<OnMoving>::notifyEventListeners ( this, OnMoving(pos) );
}
//------------------------------------------------------------------------------------------------------------
void VSTPlugView::eventHandler( void *scr, const OnMoving &ev ) {
	EventSender<OnMoving>::notifyEventListeners ( this, ev );
}
} //namespace ppiGui
