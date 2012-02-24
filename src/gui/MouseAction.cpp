/*
 * ===========================================================================================================
 * MouseAction.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */

#include "MouseAction.h"
#include "ConcreteGObjects.h"
#include "CircuidView.h"
#include "CircuidControl.h"

#define PARENT_VIEW gObj->getParentView()

namespace ppiGui{
//============================================================================================================
//	Klasse MouseAction:
//  Alle GObjects enthalten keine oder mehrere Kontextassoziierte MouseAction Instanzen.
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
MouseAction::MouseAction(){}
//------------------------------------------------------------------------------------------------------------
MouseAction::~MouseAction(){}
//============================================================================================================
//	Klasse MAMove:
//  Drag bewegt Src GObject. Nach beendeten Drag vorgang wird Command zu DRAG_FINISHED aufgerufen.
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void MAMove::onMouse ( const OnMouseDrag &ev ){
	if ( ev.btn != kLButton ) return;
	if ( ev.state == ev.DRAG ){
		ev.src->moveTo ( ev.p );
		ev.src->setObjectDirty();
	}
}
//============================================================================================================
//	Klasse MANull:
//  Null Aktion.
//============================================================================================================

//============================================================================================================
//	Klasse MAConnect:
//  Drag zieht eine Line solange wie Dragvorgang. Danach wird linie wieder entfernt und  
//  Command zu DRAG_FINISHED aufgerufen.
//  Funktioniert nur mit Objekten vom Typ GNode.
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
MAConnect::MAConnect ( CircuidView *view ) {
	MAConnect::view = view;
	line = GLine::create ( view );
	line->setVisible (false);
	line->setWidth ( 2 );
	line->setColor ( black );
	view->addGObject ( line, 3 );
}
//------------------------------------------------------------------------------------------------------------
MAConnect::~MAConnect (){}
//------------------------------------------------------------------------------------------------------------
void MAConnect::onMouse ( const OnMouseDrag &ev ){
	if ( ev.btn != kLButton ) return;
	if ( ev.state == OnMouseDrag::DRAG_START ){
		line->lineTo ( ev.src->getPos(), ev.p );
		line->setVisible();
	}
	if ( ev.state == OnMouseDrag::DRAG ){
		CPoint s = ev.src->getPos();
		line->lineTo ( s, ev.p );
		line->setObjectDirty();
	}
	if ( ev.state == OnMouseDrag::DRAG_STOP ){
		line->setVisible ( false );
		line->setObjectDirty();
		GObject::Ptr dst = view->getGObjectAt( ev.p, CircuidView::DEFAULT );
		if ( ev.src != dst.get() && dst ) ev.src->connect ( dst.get() );
	}
}
} //namespace ppiGui


