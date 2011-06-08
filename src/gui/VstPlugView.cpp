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
VSTPlugView::VSTPlugView( PpiEditor *ed, processing::VSTPlugNode::Ptr vstPlugNode ) : 
vstPlugNode(vstPlugNode),
editor (ed)
{
	using namespace processing::parameter;
	aEff = vstPlugNode->getAEffect();
	if (!vstPlugNode->can (effFlagsHasEditor)) 
		throw ppiError::IllegalOperation ("Effect has no Editor.", __FILE__, __LINE__ );

	vstPlugNode->EventSender<processing::ResizeEditorEvent>::addEventListener ( this );

}
//------------------------------------------------------------------------------------------------------------
VSTPlugView::Ptr VSTPlugView::create( PpiEditor *ed, processing::VSTPlugNode::Ptr vstPlugNode ) {
	Ptr neu = Ptr ( new VSTPlugView ( ed, vstPlugNode ) );
	neu->self = neu;
	return neu;
}
//------------------------------------------------------------------------------------------------------------
VSTPlugView::~VSTPlugView(){
	vstPlugNode->EventSender<processing::ResizeEditorEvent>::removeEventListener ( this );
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
	win->EventSender<OnOpen> ::addEventListener ( this );
	win->EventSender<OnClose>::addEventListener ( this );
	win->EventSender<OnMoving>::addEventListener ( this );
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
	editor->EventSender<OnIdle>::addEventListener ( this );
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
	// entf. movinglistener
	win->EventSender<OnMoving>::removeEventListener (this);
	// close
	aEff->dispatcher ( aEff, effEditClose, 0, 0, win->getHandle(), 0);
	// Idle Listener unregist.
	editor->EventSender<OnIdle>::removeEventListener (this);
	win->EventSender<OnOpen> ::removeEventListener ( this );
	win->EventSender<OnClose>::removeEventListener ( this );
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
