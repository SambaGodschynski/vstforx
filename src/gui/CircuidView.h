#ifndef CIRCUID_VIEW_H
#define CIRCUID_VIEW_H


#include <vector>
#include "vstgui.h"
#include "com/one4all.h"
#include "ConcreteGObjects.h"
#include "ViewEvents.h"
#include <map>
#include "CMenu.h"
#include "processing/Graph.h"
using namespace std;

namespace ppiGui{
Graph::Ptr getRelatedGraph ( CircuidView* );
//============================================================================================================
//	Klasse CircuidView:
//  Haupt View in der die ProcessorNode Schaltung erstellt, bearbeitet, entfernt werden kann.
//============================================================================================================
class CircuidView : public CView, 
	public EventSender<OnMouseClick>,
	public EventSender<OnMouseDrag>,
	public EventListener<OnMouseClick>
{
friend class CircuidControl;
friend class CmdAddObjectToView;
friend class CmdRemoveObjectFromView;
friend class boost::serialization::access;
BOOST_SERIALIZATION_SPLIT_MEMBER()
public:
private:
	//--------------------------------------------------------------------------------------------------------
	CBitmap *background;
	//--------------------------------------------------------------------------------------------------------
	typedef int U; typedef GObject::Ptr V;
	//--------------------------------------------------------------------------------------------------------
	typedef multimap< U, V > GObjectStageBuffer; 
	//--------------------------------------------------------------------------------------------------------
	// Enthaelt GObjects.
	GObjectStageBuffer gObjectBuffer;
	//--------------------------------------------------------------------------------------------------------
	// Zeichnet Hintergrund
	void drawBackground( CDrawContext *pContext );
	//--------------------------------------------------------------------------------------------------------
	// Nullpunkt
	CPoint offset;
	//--------------------------------------------------------------------------------------------------------
	Mutex updateLock;
	//--------------------------------------------------------------------------------------------------------
	Command::Container toDoList;
	//--------------------------------------------------------------------------------------------------------
	void save ( oArchive &ar, const unsigned int version ) const {}
	//--------------------------------------------------------------------------------------------------------
	void load ( iArchive &ar, const unsigned int version ) {}
public:
	//--------------------------------------------------------------------------------------------------------
	void clear();
	//--------------------------------------------------------------------------------------------------------
	// Standart Stages
	enum Stages { BOTTOM, CONNECTIONS, DEFAULT, SELECTION, TOP };
	//--------------------------------------------------------------------------------------------------------
	CircuidView ( const VSTGUI::CRect &size );
	//--------------------------------------------------------------------------------------------------------
	virtual ~CircuidView();
	//--------------------------------------------------------------------------------------------------------
	void getMouseLocation ( CPoint &p ){
		CDrawContext *cD = getFrame()->createDrawContext();
		cD->getMouseLocation (p);
		cD->forget();
	}
	//--------------------------------------------------------------------------------------------------------
	// Fuegt GObject der View hinzu.
	// Alle hinzugefuegten Objects werden beim release
	// von CircuidView released.
	void addGObject ( const GObject::Ptr &gObj, int stage = DEFAULT );
	//--------------------------------------------------------------------------------------------------------
	void removeGObject ( const GObject::Ptr &gObj );
	//--------------------------------------------------------------------------------------------------------
	// liefert dass GObject dass am Punkt p auf der Stage stage liegt - wenn vorhanden - Ansonsten NULL.  
	GObject::Ptr getGObjectAtStage ( CPoint &p, int stage );
	//--------------------------------------------------------------------------------------------------------
	// liefert alle GObjects auf ebene: stage  
	void getGObjectsAtStage ( list<V> &l, int stage );
	//--------------------------------------------------------------------------------------------------------
	// liefert dass GObject dass am Punkt p auf den Stages von start bis ende liegt - wenn vorhanden -   
	// Ist end hoeher als start wird rueckwaerts gesucht.
	GObject::Ptr getGObjectAt ( CPoint &p, int start, int end );
	//--------------------------------------------------------------------------------------------------------
	// liefert dass GObject dass am Punkt p auf den Stages von start bis highestStage liegt - wenn vorhanden -   
	GObject::Ptr getGObjectAt ( CPoint &p, int start ) { return getGObjectAt ( p, start, getHighestStage() ); }
	//--------------------------------------------------------------------------------------------------------
	// liefert dass GObject dass am Punkt p auf allen Stages 
	// von lowestStage bis highestStage liegt - wenn vorhanden -   
	GObject::Ptr getGObjectAt ( CPoint &p ){ return getGObjectAt ( p, getLowestStage(), getHighestStage() ); }
	//--------------------------------------------------------------------------------------------------------
	// liefert die Stage-ID der Stage mit der niedrigsten ID
	int getLowestStage(){ return (*(gObjectBuffer.begin() )).first; }
	//--------------------------------------------------------------------------------------------------------
	// liefert die Stage-ID der Stage mit der hoechsten ID
	int getHighestStage(){ return (*(--gObjectBuffer.end() )).first; }
	//--------------------------------------------------------------------------------------------------------
	// Verschiebt alle Objekte in View
	void offsetView ( const CPoint &p );
	//--------------------------------------------------------------------------------------------------------
	virtual void draw( CDrawContext *pContext );
	//--------------------------------------------------------------------------------------------------------
	void setDirty ( GObject *gObj );
	//--------------------------------------------------------------------------------------------------------
	void getObjectsInArea ( const VSTGUI::CRect &rect, list<GObject::Ptr> &v );
	//--------------------------------------------------------------------------------------------------------
	virtual void mouse ( CDrawContext *cc, CPoint &p, long btn );
	//--------------------------------------------------------------------------------------------------------
	void eventHandler ( void *src, const OnMouseClick &ev ); // kontextmenu click
};
//============================================================================================================
// Frei stehende Methoden fuer boost archive:
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
template<class Archive>
inline void save_construct_data( Archive & ar, const CircuidView * t, const unsigned int file_version ){
	VSTGUI::CRect size;
	t->getViewSize( size );
	long top, left, right, bottom;
	top = size.top; left = size.left; right = size.right; bottom = size.bottom;
	ar << left;
	ar << top;
	ar << right;
	ar << bottom;
}
//------------------------------------------------------------------------------------------------------------
template<class Archive>
inline void load_construct_data( Archive & ar, CircuidView * t, const unsigned int file_version ){
	long top, left, right, bottom;
	ar >> left;
	ar >> top;
	ar >> right;
	ar >> bottom;
	::new(t)ppiGui::CircuidView ( VSTGUI::CRect( left, top, right, bottom ) ); 
}
} // namespace ppiGui
#endif


