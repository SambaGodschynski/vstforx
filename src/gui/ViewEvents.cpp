/*
 * ===========================================================================================================
 * ViewEvents.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */

#include "ViewEvents.h"
#include "PpiGui.h"

namespace ppiGui {
//============================================================================================================
// Methode: sendMouseEvents.
// sendet OnDragEvent oder OnClickEvent.
//============================================================================================================
void sendMouseEvents ( CDrawContext *cc, 
							  CPoint &p, 
							  long btn, 
							  EventSender<OnMouseDrag>  *dragSender, 
							  EventSender<OnMouseClick> *clickSender, 
							  GObject *eventSrc 
){
	if ( cc->waitDrag() ){ // entweder drag ...
		OnMouseDrag mD( eventSrc, cc, p, btn );
		mD.state = OnMouseDrag::DRAG_START;
		mD.dragDirection = p;
		dragSender->notifyEventListeners ( dragSender, mD ); // sende drag start
		mD.state = OnMouseDrag::DRAG;	
		while ( cc->waitDrag() ) {
			cc->getMouseLocation ( mD.p );
			dragSender->notifyEventListeners ( dragSender, mD ); // sende drag solange maus gehalten
			mD.dragDirection = mD.p;
		}
		mD.state = OnMouseDrag::DRAG_STOP;
		dragSender->notifyEventListeners ( dragSender, mD ); // sende drag stop
	} // ... oder click 
	else clickSender->notifyEventListeners ( clickSender, OnMouseClick ( eventSrc, cc, p, btn ) );
}
} // namespace ppiGui