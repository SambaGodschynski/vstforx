/*
 * ===========================================================================================================
 * PpiGui.h
 *      Author: Johannes Unger
 * ===========================================================================================================
 */

#ifndef PPIGUI_H
#define PPIGUI_H
#include <list>
#include "vstgui.h"
#include "com/one4all.h"
#include "ViewEvents.h"
#include "Resources.h"
using namespace std;

namespace ppiGui{
//============================================================================================================
/**
 * Ein paar Farben:
 */
extern const CColor red;
extern const CColor white;
extern const CColor darkRed;
extern const CColor darkGrey;
extern const CColor black;
extern const CColor blue;
extern const CColor lightBlue;
extern const CColor darkBlue;
extern const CColor green;
extern const CColor yellow;
extern const CColor grey;
extern const CColor bk;
extern const CColor grid;
extern const CColor alphaGrey;
extern const CColor alphaBlue;
//============================================================================================================

//============================================================================================================
/**
 * macht uebergebene Frabe heller oder dunkler
 * @param c
 * @param fak
 */
extern void brightness ( CColor &c, float fak );
//============================================================================================================
/**
 * ein paar Vorwaertz-Deklarationen
 */
//============================================================================================================
struct Vector2D;
class GObject;
class GIONode;
class GLine;
class GRect;
class GCircle;
class CircuidView;
class Connectable;
class GProcessorNode;
template < class GOBJECT >
class CViewWrapper;
class GCView;
}// namespace ppiGui

namespace ppiGui{
//============================================================================================================
/**
 * @class Vector2D.
 * TODO: kann durch sambag::math::Vector ersetzt werden.
 */
struct Vector2D {
//============================================================================================================
	float x;
	float y;
	static float abs( const Vector2D& v ) { return sqrt( v.x*v.x + v.y*v.y ); }
	Vector2D(){}
	Vector2D ( const CPoint &a, const CPoint &b ) : x((float)b.x-a.x), y((float)b.y-a.y) {}
	Vector2D & operator /= ( float v ) { x/=v; y/=v;return *this; }
	Vector2D & operator *= ( float v ) { x*=v; y*=v;return *this; }
	operator CPoint() { return CPoint((CCoord)x, (CCoord)y); }
};
//============================================================================================================
/**
 * @class MyPoint
 * Erweitert CPoint um einige Operatoren
 */
struct MyPoint : public CPoint {
//============================================================================================================
	MyPoint () {}
	MyPoint ( CCoord h, CCoord v ) : CPoint ( h, v ){}
	MyPoint ( const CPoint &p ) : CPoint (p){}
	CPoint & operator += ( const CPoint & p ){ h+=p.h; v+=p.v; return *this; }
	CPoint & operator -= ( const CPoint & p ){ h-=p.h; v-=p.v; return *this; }
	CPoint operator + ( const CPoint & p ) const { 
		MyPoint r = *this;
		r+=p; 
		return r; 
	}
	CPoint operator - ( const CPoint & p ) const { 
		MyPoint r = *this;
		r-=p; 
		return r; 
	}
};
//============================================================================================================
/**
 * @class GObject.
 * Oberklasse fuer alle Graphischen Objekte die
 * von der CircuidView dargestellt werden.
 */
class GObject : 
	public EventSender< OnDestroy<GObject> >, 
	public EventSender<OnMouseClick>,
	public EventSender<OnMouseDrag>,
	public EventSender<OnConnect>,
	public EventSender<OnRemove>
{
//============================================================================================================
friend class CircuidControl;
friend class CircuidView;
friend class boost::serialization::access;
friend class MAUse;
BOOST_SERIALIZATION_SPLIT_MEMBER()
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<GObject> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	static int instances;
protected:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Erster Mausaufruf. wird auf objekt geklickt wird onMouse aufgerufen,
	 * diese Methode benachrichtigt alle listener.
	 * @param cc
	 * @param p
	 * @param btn
	 */
	virtual void onMouse ( CDrawContext *cc, CPoint &p, long btn );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * zweiter Mausaufruf.
	 * Wird von Controller aufgerufen.
	 * @param cc
	 * @param p
	 * @param btn
	 */
	virtual void useByMouse ( CDrawContext *cc, CPoint &p, long btn ){}
	//--------------------------------------------------------------------------------------------------------
	VSTGUI::CRect bBox;
	//--------------------------------------------------------------------------------------------------------
	CircuidView *parentView;
	//--------------------------------------------------------------------------------------------------------
	int objNr;
	//--------------------------------------------------------------------------------------------------------
	// Fest zum GObject zugeordnete weitere
	// GObjects wie Beschriftung etc.
	vector< GObject::Ptr > subObjects;
	//--------------------------------------------------------------------------------------------------------
	// mittelpunkt
	CPoint focus;
	//--------------------------------------------------------------------------------------------------------
	int zPos;
	//--------------------------------------------------------------------------------------------------------
	MyString name;
	//--------------------------------------------------------------------------------------------------------
	MyString description;
	//--------------------------------------------------------------------------------------------------------
	bool visible;
	//--------------------------------------------------------------------------------------------------------
	GObject() : objNr(objNr++), visible(true){}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Serialisiert GObject-Objekt
	 * @param ar boost::Archive-Objekt
	 * @param version
	 */
	void save ( oArchive &ar, const unsigned int version ) const;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Deserialisiert GObject-Objekt
	 * @param ar boost::Archive-Objekt
	 * @param version
	 */
	void load ( iArchive &ar, const unsigned int version );
	//--------------------------------------------------------------------------------------------------------
	boost::weak_ptr<GObject> self;
	//--------------------------------------------------------------------------------------------------------
	GObject ( CircuidView *parentView, string name = "unnamed" );
public:
	//--------------------------------------------------------------------------------------------------------
	void _setSelfPtr ( const Ptr &ptr ) {
		if ( ptr.get() != this ) return;
		self = boost::weak_ptr<GObject> ( ptr );
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * VST-GUI Event
	 * @param pos
	 */
	virtual void onMouseEnter( const CPoint &pos ) {}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * VST-GUI Event
	 * @param pos
	 */
	virtual void onMouseLeave( const CPoint &pos ) {}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return GObject-Objekt Shared-Pointer
	 */
	Ptr getPtr() { return self.lock(); }
	//--------------------------------------------------------------------------------------------------------
	virtual ~GObject();
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Loest OnConnect-Event aus.
	 * @param dst
	 */
	virtual void connect ( GObject *dst ) { 
		EventSender<OnConnect>::notifyEventListeners( this, OnConnect (this, dst) );
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * setzt Objektname
	 * @param name
	 */
	virtual void setName ( const string &name ){ GObject::name = name; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Objektname
	 */
	virtual std::string getName () const { RETURN_NAME(name); }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Setzt optionalen Beschreibungs-String
	 * @param des
	 */
	void setDescription ( const string &des ){ GObject::description = des; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return optionalen Beschreibungs-String
	 */
	string getDescription (){ return description; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * setzt sichtbarkeit
	 * @param vis
	 */
	void setVisible ( bool vis=true ) { visible=vis; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return true, wenn sichtbar
	 */
	bool isVisible() const { return visible; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * zeichnet Objekt.
	 * @param pContext
	 */
	virtual void draw ( CDrawContext *pContext ) = 0;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * setzt Objekt Postion
	 * @param p
	 */
	void moveTo ( const CPoint &p ){
		CCoord w = bBox.width()>>1;
		CCoord h = bBox.height()>>1;
		offset ( (p.x - bBox.x)-w , (p.y - bBox.y)-h );
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return aktuelle Position
	 */
	const CPoint & getPos() const { return focus; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Z-Ordnungsnummer
	 */
	int getZPos() const { return zPos; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * setzt Z-Ordnungsnummer
	 * @param z
	 */
	void setZPos ( int z ){ zPos = z; } 
	//--------------------------------------------------------------------------------------------------------
	/**
	 * verschiebt Objekt
	 * @param x
	 * @param y
	 */
	virtual void offset ( const CCoord &x, const CCoord &y ){
		bBox.offset ( x, y );
		focus.offset ( x, y );
		//unterobjekte verschieben
		for ( unsigned int i=0; i<subObjects.size(); i++ ) {
			subObjects[i]->offset ( x, y );
		}
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param p
	 * @return true wenn p Objekt trifft.
	 */
	virtual bool hitTest ( const CPoint &p ) const { return bBox.pointInside ( p ); }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * gibt an das GObject neu gezeichnet werden muss. (@see VST-GUI::setDirty())
	 */
	virtual void setObjectDirty();
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return umschliessendes Rechteck
	 */
	VSTGUI::CRect getSize() const { return bBox; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Eltern-CircuidView
	 */
	CircuidView * getParentView () const { return parentView; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param index
	 * @return Unterobjekt an index.
	 */
	GObject::Ptr getSubObject ( int index ) const { return subObjects.at( index ); } 
};
//============================================================================================================
/**
 * @class GLine:
 * Implementiert eine Linie.
 */
class GLine : public GObject {
//============================================================================================================
friend class boost::serialization::access;
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<GLine> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	template < typename Archive > 
	void serialize ( Archive &ar, const unsigned int version ){
		ar & boost::serialization::base_object< GObject >( *this );
	}
protected:
	//--------------------------------------------------------------------------------------------------------
	CColor color;
	//--------------------------------------------------------------------------------------------------------
	int width;
	//--------------------------------------------------------------------------------------------------------
	CLineStyle style;
	//--------------------------------------------------------------------------------------------------------
	GLine ( CircuidView *parent ) : GObject ( parent ){
		color  ( 0, 0, 0, 0 );
		width = 1;
		style = kLineSolid;
		name = "GLine";
	}
public:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param parent
	 * @return neues GLine-Objekt
	 */
	static GLine::Ptr create( CircuidView *parent ) {
		GLine::Ptr neu( new GLine(parent) );
		neu->_setSelfPtr ( neu );
		return neu;
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * zeichnet Objekt.
	 * @param pContext
	 */
	virtual void draw( CDrawContext *pContext );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * setzt Linien-Farbe
	 * @param col
	 */
	void setColor ( CColor col ){ color = col; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * setzt Linien-Style (@see VSTGUI::CLineStyle)
	 * @param lineStyle
	 */
	void setStyle ( CLineStyle lineStyle ){ style = lineStyle; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * setzt Linen-Breite
	 * @param lineWidth
	 */
	void setWidth ( int lineWidth ){ width = lineWidth; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Linien-Breite
	 */
	int getWidth() const { return width; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param p
	 * @return true wenn p Objekt trifft.
	 */
	virtual bool hitTest ( const CPoint &p ) const;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Zeichnet Linie von p0 nach p1
	 * @param p0
	 * @param p1
	 */
	void lineTo ( CPoint p0, CPoint p1 ){
		bBox.left =  p0.x;
		bBox.top = p0.y;
		bBox.right = p1.x;
		bBox.bottom = p1.y;
		Vector2D v( p0, p1 );
		v*=0.5f;
		focus = CPoint ( p0.x + (CCoord)v.x, p0.y + (CCoord)v.y ); // mittelpunkt der Linie 
	}
};
typedef list<GObject::Ptr> GObjectList;
//============================================================================================================
/**
 * @class GRect.
 * Implementiert ein Rechteck
 */
class GRect : public GObject {
//============================================================================================================
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<GRect> Ptr;
private:
protected:
	//--------------------------------------------------------------------------------------------------------
	CColor color;
	//--------------------------------------------------------------------------------------------------------
	CColor bgColor;
	//--------------------------------------------------------------------------------------------------------
	int width;
	//--------------------------------------------------------------------------------------------------------
	CDrawStyle drawStyle;
	//--------------------------------------------------------------------------------------------------------
	CLineStyle lineStyle;
	//--------------------------------------------------------------------------------------------------------
	GRect ( CircuidView *parent ) : GObject ( parent ){
		color  ( 0, 0, 0, 0 );
		bgColor ( 255, 255, 255, 0 );
		width = 1;
		drawStyle = kDrawStroked;
		lineStyle = kLineSolid;
		name = "GRect";
	}
public:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param parent
	 * @return neues GRect-Objekt
	 */
	static GRect::Ptr create( CircuidView *parent ) {
		GRect::Ptr neu( new GRect(parent) );
		neu->_setSelfPtr ( neu );
		return neu;
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * zeichnet Objekt.
	 * @param pContext
	 */
	virtual void draw( CDrawContext *pContext );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * setzt Rahmenfarbe
	 * @param col
	 */
	void setColor ( CColor col ){ color = col; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * setzt Hintergrundfarbe
	 * @param col
	 */
	void setBGColor ( CColor col ){ bgColor = col; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * TODO: entfernen
	 * @param style
	 */
	void setDrawStyle ( CDrawStyle style ){ drawStyle = style; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Rahmen-Linien-Style (@see VSTGUI::CLineStyle)
	 * @param style
	 */
	void setLineStyle ( CLineStyle style ){ lineStyle = style; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * setzt Linien-Breite
	 * @param lineWidth
	 */
	void setWidth ( int lineWidth ){ width = lineWidth; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * setzt Rect
	 * @param r
	 */
	void setRect ( VSTGUI::CRect &r ){
		bBox = r;
		focus = CPoint ( r.getWidth()>>1, r.getHeight()>>1 ); // mittelpunkt der Box 
	}
};
//============================================================================================================
/**
 * @class GBitmap.
 * Implementiert eine Bitmapgraphik.
 */
class GBitmap : public GObject {
//============================================================================================================
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<GBitmap> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	Resources::BitmapID bmpID;
	//--------------------------------------------------------------------------------------------------------
	CBitmap *bmp;
protected:
	//--------------------------------------------------------------------------------------------------------
	GBitmap ( CircuidView *parent, Resources::BitmapID bmpID ) : bmpID( bmpID ), GObject( parent ) {
		bmp = resources->getResourceBitmap( bmpID );
		bBox = VSTGUI::CRect ( 0, 0, bmp->getWidth(), bmp->getHeight() );
	}
public:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * zeichnet Objekt.
	 * @param pContext
	 */
	virtual void draw ( CDrawContext *pContext );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param parent
	 * @return neues GBitmap-Objekt
	 */
	static Ptr create( CircuidView *view, Resources::BitmapID bmpID ) {
		GBitmap::Ptr neu( new GBitmap(view, bmpID ) );
		neu->_setSelfPtr ( neu );
		return neu;
	}
};
//============================================================================================================
/**
 * @class GCirlce
 * Implementiert ein Kreis.
 */
class GCircle : public GObject {
//============================================================================================================
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<GCircle> Ptr;
private:
protected:
	//--------------------------------------------------------------------------------------------------------
	CColor color;
	//--------------------------------------------------------------------------------------------------------
	int radius;
	//--------------------------------------------------------------------------------------------------------
	GCircle(){}
	//--------------------------------------------------------------------------------------------------------
	GCircle ( CircuidView *view, const MyString &name = "Circular Object", const CColor &color=black  ) :
	  color (color), GObject ( view, name ){}
public:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param parent
	 * @return neues GCircle-Objekt
	 */
	static GCircle::Ptr create( CircuidView *view, 
								const MyString &name = "Circular Object", 
								const CColor &color=black  ) 
	{
		GCircle::Ptr neu( new GCircle(view, name, color) );
		neu->_setSelfPtr ( neu );
		return neu;
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Setzt Kreis-Radius
	 * @param r
	 */
	void setRadius ( int r ) { radius = r;  }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Kreis-Radius
	 */
	int getRadius () const { return radius; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param p
	 * @return true wenn p Objekt trifft.
	 */
	virtual bool hitTest ( const CPoint &p ) const {
		int x = p.x - focus.x;
		int y = p.y - focus.y;
		return x*x + y*y <= radius*radius;
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * TODO: entfernen
	 */
	void draw ( CDrawContext *cc, int renderRadius  );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * zeichnet Objekt.
	 * @param pContext
	 */
	virtual void draw ( CDrawContext *cc ) { draw(cc, radius); }
	//--------------------------------------------------------------------------------------------------------
	virtual ~GCircle(){}
};
//============================================================================================================
/**
 * @class IONode:
 * Oberklasse fuer Ein bzw. Ausgangs Knoten.
 * Wird als Kreisfoermiges Objekt dargestellt.
 */
class GIONode : public GCircle {
//============================================================================================================
friend class boost::serialization::access;
private:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * (De)Serialisiert GIONode-Objekt
	 * @param ar boost::Archive-Objekt
	 * @param version
	 */
	template < typename Archive >
	void serialize ( Archive &ar, const unsigned int version ){
		ar & boost::serialization::base_object<GObject> ( *this );
		ar & index;
	}
protected:
	//--------------------------------------------------------------------------------------------------------
	int index;
	//--------------------------------------------------------------------------------------------------------
	GIONode ( CircuidView *parent );
public:
	//--------------------------------------------------------------------------------------------------------
	~GIONode ();
	//--------------------------------------------------------------------------------------------------------
	/**
	 * zeichnet Objekt.
	 * @param pContext
	 */
	virtual void draw( CDrawContext *pContext ) = 0;
};

//============================================================================================================
/**
 * @class GProcessorNode:
 * Repraesentiert ein ProcessAdapter Objekt aus dem Graph.
 * Ein GProcessorNode hatt ein oder mehrerer Ein-und Ausgaenge.
 */
class GProcessorNode : public GCircle {
//============================================================================================================
friend class boost::serialization::access;
friend class CmdAddOutput;
friend class CmdAddInput;
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<GProcessorNode> Ptr;
	//--------------------------------------------------------------------------------------------------------
	typedef vector<GObject::Ptr> InputNodeContainer;
	//--------------------------------------------------------------------------------------------------------
	typedef vector<GObject::Ptr> OutputNodeContainer;
private:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * (De)Serialisiert GProcessorNode-Objekt
	 * @param ar boost::Archive-Objekt
	 * @param version
	 */
	template < typename Archive >
	void serialize ( Archive &ar, const unsigned int version ){
		ar & boost::serialization::base_object< GObject > ( *this );
		ar & ins;
		ar & outs;
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * fuegt neuen GInput-Node hinzu
	 * @param p
	 * @return
	 */
	GObject::Ptr  createInNode(CPoint &p);
	//--------------------------------------------------------------------------------------------------------
	/**
	 * fuegt neuen GOutput-Node hinzu
	 * @param p
	 * @return
	 */
	GObject::Ptr createOutNode(CPoint &p);
	//--------------------------------------------------------------------------------------------------------
	InputNodeContainer ins;
	//--------------------------------------------------------------------------------------------------------
	OutputNodeContainer outs;
protected:
	//--------------------------------------------------------------------------------------------------------
	GProcessorNode(){}
	//--------------------------------------------------------------------------------------------------------
	GProcessorNode ( CircuidView *parent );
public:
	//--------------------------------------------------------------------------------------------------------
	void resetIOPosition();
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return InputNode-Container
	 */
	const InputNodeContainer & getInputNodes() const { return ins; } 
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return OutputNode-Container
	 */
	const OutputNodeContainer & getOutputNodes() const { return outs; } 
	//--------------------------------------------------------------------------------------------------------
	/**
	 * erzeugt N Eingangs und M Ausgangsknoten
	 * @param numInputs
	 * @param numOutputs
	 */
	void createIONodes ( int numInputs, int numOutputs ); 
	//--------------------------------------------------------------------------------------------------------
	/**
	 * fuegt neue Eingang/Ausgangsknoten der View hinzu
	 * @param createdConnections
	 */
	void addIONodesOnView ( GObjectList &createdConnections ); 
	//--------------------------------------------------------------------------------------------------------
	virtual ~GProcessorNode(){}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * fuegt Eingang/Ausgangsknoten Container hinzu
	 * @param l
	 */
	template <typename Container>
	void getIOs ( Container &l ) const {
		InputNodeContainer::const_iterator iit = ins.begin();
		for ( ; iit!=ins.end(); ++iit ) {
			l.push_back ( *iit );
		}
		OutputNodeContainer::const_iterator oit = outs.begin();
		for ( ; oit!=outs.end(); ++oit ) {
			l.push_back ( *oit );
		}
	}
};
//============================================================================================================
/**
 * @class GCView:
 * Erweitert CView. Diese Klasse ist noetig um das protected Attribut 'CView::pParentView' zu setzen
 * ohne frame->addView() zu verwenden.
 */
class GCView : public CView {
//============================================================================================================
public:
	//--------------------------------------------------------------------------------------------------------
	void setFrame ( CFrame *frame ) {
		pParentFrame = frame;
	}
};
//============================================================================================================
/**
 * @class CViewWrapper:
 * Huellklasse fuer VSTGUI::CView.
 * CView erbt von Template-Parameter GOBJECT. GOBJECT muss unterobjekt von GObjekt sein.
 */
template < class GOBJECT >
class CViewWrapper : public GOBJECT {
//============================================================================================================
protected:
	//--------------------------------------------------------------------------------------------------------
	CView *cView;
	//--------------------------------------------------------------------------------------------------------
	CViewWrapper ( CircuidView *parent, CView *cView );
public:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * verschiebt Objekt um x, y auf View
	 * @param x
	 * @param y
	 */
	virtual void offset ( const CCoord &x, const CCoord &y ){
		GOBJECT::offset ( x, y );
		cView->setViewSize ( GOBJECT::bBox );
	}
	//--------------------------------------------------------------------------------------------------------
	CView * getCView(){ return cView; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Setzt Objekt Position
	 * @param p
	 */
	void moveTo ( const CPoint &p ){
		GOBJECT::moveTo (p);
		cView->setViewSize ( GOBJECT::bBox );
		cView->setMouseableArea ( GOBJECT::bBox );
	}
	//--------------------------------------------------------------------------------------------------------
	virtual ~CViewWrapper();
	//--------------------------------------------------------------------------------------------------------
	/**
	 * zeichnet Objekt.
	 * @param pContext
	 */
	virtual void draw ( CDrawContext *cc ) {
		for ( unsigned int i = 0; i<GOBJECT::subObjects.size(); i++ ) GOBJECT::subObjects[i]->draw ( cc );
		cView->draw ( cc );
	}
};
//============================================================================================================
//	Klasse CViewWrapper Methoden Definitionen:
//  Huellklasse fuer VSTGUI::CView.
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
template <class GOBJECT>
CViewWrapper<GOBJECT>::CViewWrapper ( CircuidView *parent, CView *cView ) : 
cView ( cView ), 
GOBJECT ( parent, "CViewWrapper" )
{
	cView->getViewSize( GOBJECT::bBox );
	GOBJECT::focus = CPoint ( GOBJECT::bBox.x + GOBJECT::bBox.width()>>1, 
					 GOBJECT::bBox.y + GOBJECT::bBox.height()>>1 );
	GCView *hack = (GCView*) (cView);
	hack->setFrame ( GOBJECT::getParentView()->getFrame() );
	assert ( cView->getFrame() == GOBJECT::getParentView()->getFrame() );
}
//------------------------------------------------------------------------------------------------------------
template < class GOBJECT >
CViewWrapper<GOBJECT>::~CViewWrapper(){
	GOBJECT::parentView->getFrame()->removeView ( cView );
	cView->forget();
};
//============================================================================================================
/**
 * @class CControlWrapper:
 * Huellklasse fuer VSTGUI::CControl
 * CView erbt von Template-Parameter GOBJECT. GOBJECT muss unterobjekt von GObjekt sein.
 */
//============================================================================================================
template < class GOBJECT >
class CControlWrapper : public CViewWrapper<GOBJECT> {
protected:
	//--------------------------------------------------------------------------------------------------------
	CControl *cView;
	//--------------------------------------------------------------------------------------------------------
	virtual void useByMouse ( CDrawContext *cc, CPoint &p, long btn ){
		if ( GOBJECT::bBox.pointInside(p) )
			cView->mouse ( cc, p, btn );
	}
	//--------------------------------------------------------------------------------------------------------
	CControlWrapper ( CircuidView *parent, CControl *cView ) : 
		cView (cView), CViewWrapper<GOBJECT> ( parent, cView )
	{
	}
public:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * setzt CControl-Wert
	 * @param val
	 */
	void setValue ( VstNumber val ){ cView->setValue ( val ); }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return CControl-Wert
	 */
	VstNumber getValue () const { return cView->getValue(); }
};
//============================================================================================================
//  Frei stehende Methoden:
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
/**
 * Punkt Rotieren um einen Mittel-Punkt
 * @param p
 * @param alpha winkel in Grad
 * @param center
 */
extern void rotate(CPoint &p, float alpha, const CPoint &center);
//------------------------------------------------------------------------------------------------------------
typedef list<GObject::Ptr>::const_iterator GObjectIterator;
//------------------------------------------------------------------------------------------------------------
extern VSTGUI::CRect getBoundingBox ( const GObjectIterator &begin, const GObjectIterator &end );
//------------------------------------------------------------------------------------------------------------
/**
 * stellt sicher dass, x1 > x0 UND y1 > y0
 * @param rect Eingabe-Rechteck
 * @return normalisiertes Rechteck
 */
extern VSTGUI::CRect normalizeRect ( const VSTGUI::CRect &rect );
} //namespace ppiGui
#endif


