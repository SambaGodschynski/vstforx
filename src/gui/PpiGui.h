#ifndef PPIGUI_H
#define PPIGUI_H
#include <list>
#include <iterator>
#include "vstgui.h"
#include "com/one4all.h"
#include "ViewEvents.h"
#include "Resources.h"
using namespace std;

namespace ppiGui{
//============================================================================================================
//  Ein paar Farben:
//============================================================================================================
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
extern void brightness ( CColor &c, float fak );
//============================================================================================================
// Strcut Vector:
// 2D Vector.
//============================================================================================================
struct Vector2D;
//============================================================================================================
//	Klasse GObject:
//  Oberklasse fuer alle Graphischen Objekte die in der CircuidView dargestellt werden.
//============================================================================================================
class GObject;
//============================================================================================================
//	Klasse GNode:
//  Repraesentiert Knoten Objekte die frei in der Circuid View bewegt werden kann.
//  Kann mit anderen Knoten verbunden werden. Ein GNode Obj. ist Kreisfoermig.
//============================================================================================================
class GIONode;
//============================================================================================================
//	Klasse GLine:
//  Stellt eine Linie dar.
//============================================================================================================
class GLine;
//============================================================================================================
//	Klasse GRect:
//  Stellt ein Rechteck dar.
//============================================================================================================
class GRect;
//============================================================================================================
//	Klasse GCircle:
//  Stellt ein Kreis dar. ReImplementriert hitTest. 
//============================================================================================================
class GCircle;
//============================================================================================================
//	Klasse CircuidView:
//  Haupt View in der die ProcessorNode Schaltung erstellt, bearbeitet, entfernt werden kann.
//============================================================================================================
class CircuidView;
//============================================================================================================
//	Schnitstellte Connectable:
//  Alle vebindbaren GObjects leiten hier ab.
//============================================================================================================
class Connectable;
//============================================================================================================
//	Klasse GProcessorNode:
//  Repraesentiert Processor Knoten Objekte die frei in der Circuid View bewegt werden kann.
//  Kann mit anderen Knoten verbunden werden. 
//============================================================================================================
class GProcessorNode;
//============================================================================================================
//	Template CViewWrapper:
//  Huellklasse fuer VSTGUI::CView.
//  CView erbt von GOBJECT. GOBJECT muss unterobjekt von GObjekt sein.
//============================================================================================================
template < class GOBJECT >
class CViewWrapper;
//============================================================================================================
//	Klasse GCView:
//  Erbt von CView. Diese Klasse ist leider noetig um das protected Attribut pParentView zu setzen
//  ohne frame->addView() zu verwenden. (CView nach GCView casten. setParentView aufrufen.) 
//============================================================================================================
class GCView;
}// namespace ppiGui

namespace ppiGui{
//============================================================================================================
// Klasse Vector2D:
// 2D Vector.
//============================================================================================================
struct Vector2D {
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
// struct MyPoint
// Erweitert CPoint um einige operatoren
//============================================================================================================
struct MyPoint : public CPoint {
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
//	Klasse GObject:
//  Oberklasse fuer alle Graphischen Objekte die in der CircuidView dargestellt werden.
//============================================================================================================
class GObject : 
	public EventSender< OnDestroy<GObject> >, 
	public EventSender<OnMouseClick>,
	public EventSender<OnMouseDrag>,
	public EventSender<OnConnect>,
	public EventSender<OnRemove>
{
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
	// erster Mausaufruf. wird auf objekt geklickt wird onMouse aufgerufen.
	// diese Methode benachrichtigt standart maessig alle listener.
	// wird in PlaceGObject reImplementiert.
	virtual void onMouse ( CDrawContext *cc, CPoint &p, long btn );
	//--------------------------------------------------------------------------------------------------------
	// zweiter Mausaufruf.
	// wird von Controller aufgerufen.
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
	void save ( oArchive &ar, const unsigned int version ) const;
	//--------------------------------------------------------------------------------------------------------
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
	virtual void onMouseEnter( const CPoint &pos ) {}
	//--------------------------------------------------------------------------------------------------------
	virtual void onMouseLeave( const CPoint &pos ) {}
	//--------------------------------------------------------------------------------------------------------
	Ptr getPtr() { return self.lock(); }
	//--------------------------------------------------------------------------------------------------------
	virtual ~GObject();
	//--------------------------------------------------------------------------------------------------------
	virtual void connect ( GObject *dst ) { 
		EventSender<OnConnect>::notifyEventListeners( this, OnConnect (this, dst) );
	}
	//--------------------------------------------------------------------------------------------------------
	virtual void setName ( const string &name ){ GObject::name = name; }
	//--------------------------------------------------------------------------------------------------------
	virtual string getName () const { RETURN_NAME(name); }
	//--------------------------------------------------------------------------------------------------------
	void setDescription ( const string &des ){ GObject::description = des; }
	//--------------------------------------------------------------------------------------------------------
	string getDescription (){ return description; }
	//--------------------------------------------------------------------------------------------------------
	void setVisible ( bool vis=true ) { visible=vis; }
	//--------------------------------------------------------------------------------------------------------
	bool isVisible() const { return visible; }
	//--------------------------------------------------------------------------------------------------------
	virtual void render ( CDrawContext *pContext ) = 0;
	//--------------------------------------------------------------------------------------------------------
	void moveTo ( const CPoint &p ){
		CCoord w = bBox.width()>>1;
		CCoord h = bBox.height()>>1;
		offset ( (p.x - bBox.x)-w , (p.y - bBox.y)-h );
	}
	//--------------------------------------------------------------------------------------------------------
	const CPoint & getPos() const { return focus; }
	//--------------------------------------------------------------------------------------------------------
	int getZPos() const { return zPos; }
	//--------------------------------------------------------------------------------------------------------
	void setZPos ( int z ){ zPos = z; } 
	//--------------------------------------------------------------------------------------------------------
	virtual void offset ( const CCoord &x, const CCoord &y ){
		bBox.offset ( x, y );
		focus.offset ( x, y );
		//unterobjekte verschieben
		for ( unsigned int i=0; i<subObjects.size(); i++ ) {
			subObjects[i]->offset ( x, y );
		}
	}
	//--------------------------------------------------------------------------------------------------------
	// liefert true wenn CPoint p im bereich vom GObject 
	// liegt.
	virtual bool hitTest ( const CPoint &p ) const { return bBox.pointInside ( p ); }
	//--------------------------------------------------------------------------------------------------------
	// gibt an das GObject neu gezeichnet werden muss.
	virtual void setObjectDirty();
	//--------------------------------------------------------------------------------------------------------
	VSTGUI::CRect getSize() const { return bBox; }
	//--------------------------------------------------------------------------------------------------------
	CircuidView * getParentView () const { return parentView; }
	//--------------------------------------------------------------------------------------------------------
	GObject::Ptr getSubObject ( int index ) const { return subObjects.at( index ); } 
};
//============================================================================================================
//	Klasse GLine:
//  Stellt Linie eine dar.
//============================================================================================================
class GLine : public GObject {
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
	static GLine::Ptr create( CircuidView *parent ) {
		GLine::Ptr neu( new GLine(parent) );
		neu->_setSelfPtr ( neu );
		return neu;
	}
	//--------------------------------------------------------------------------------------------------------
	virtual void render( CDrawContext *pContext );
	//--------------------------------------------------------------------------------------------------------
	void setColor ( CColor col ){ color = col; }
	//--------------------------------------------------------------------------------------------------------
	void setStyle ( CLineStyle lineStyle ){ style = lineStyle; }
	//--------------------------------------------------------------------------------------------------------
	void setWidth ( int lineWidth ){ width = lineWidth; }
	//--------------------------------------------------------------------------------------------------------
	int getWidth() const { return width; }
	//--------------------------------------------------------------------------------------------------------
	virtual bool hitTest ( const CPoint &p ) const;
	//--------------------------------------------------------------------------------------------------------
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
//	Klasse GRect:
//  Stellt ein Rechteck dar.
//============================================================================================================
class GRect : public GObject {
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
	static GRect::Ptr create( CircuidView *parent ) {
		GRect::Ptr neu( new GRect(parent) );
		neu->_setSelfPtr ( neu );
		return neu;
	}
	//--------------------------------------------------------------------------------------------------------
	virtual void render( CDrawContext *pContext );
	//--------------------------------------------------------------------------------------------------------
	void setColor ( CColor col ){ color = col; }
	//--------------------------------------------------------------------------------------------------------
	void setBGColor ( CColor col ){ bgColor = col; }
	//--------------------------------------------------------------------------------------------------------
	void setDrawStyle ( CDrawStyle style ){ drawStyle = style; }
	//--------------------------------------------------------------------------------------------------------
	void setLineStyle ( CLineStyle style ){ lineStyle = style; }
	//--------------------------------------------------------------------------------------------------------
	void setWidth ( int lineWidth ){ width = lineWidth; }
	//--------------------------------------------------------------------------------------------------------
	void setRect ( VSTGUI::CRect &r ){
		bBox = r;
		focus = CPoint ( r.getWidth()>>1, r.getHeight()>>1 ); // mittelpunkt der Box 
	}
};
//============================================================================================================
//	Klasse GBitmap:
//============================================================================================================
class GBitmap : public GObject {
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
	virtual void render ( CDrawContext *pContext );
	//--------------------------------------------------------------------------------------------------------
	static Ptr create( CircuidView *view, Resources::BitmapID bmpID ) {
		GBitmap::Ptr neu( new GBitmap(view, bmpID ) );
		neu->_setSelfPtr ( neu );
		return neu;
	}
};
//============================================================================================================
//	Klasse GCircle:
//============================================================================================================
class GCircle : public GObject {
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
	static GCircle::Ptr create( CircuidView *view, 
								const MyString &name = "Circular Object", 
								const CColor &color=black  ) 
	{
		GCircle::Ptr neu( new GCircle(view, name, color) );
		neu->_setSelfPtr ( neu );
		return neu;
	}
	//--------------------------------------------------------------------------------------------------------
	void setRadius ( int r ) { radius = r;  }
	//--------------------------------------------------------------------------------------------------------
	int getRadius () const { return radius; }
	//--------------------------------------------------------------------------------------------------------
	// liefert true wenn CPoint p im bereich vom GObject 
	// liegt.
	virtual bool hitTest ( const CPoint &p ) const {
		int x = p.x - focus.x;
		int y = p.y - focus.y;
		return x*x + y*y <= radius*radius;
	}
	//--------------------------------------------------------------------------------------------------------
	void render ( CDrawContext *cc, int renderRadius  );
	//--------------------------------------------------------------------------------------------------------
	virtual void render ( CDrawContext *cc ) { render(cc, radius); }
	//--------------------------------------------------------------------------------------------------------
	virtual ~GCircle(){}
};
//============================================================================================================
//	Klasse IONode:
//  Oberklasse fuer Ein bzw. Ausgangs Knoten der frei in der Circuid View bewegt werden kann.
//  Wird als Kreisfoermiges Objekt dargestellt.
//============================================================================================================
class GIONode : public GCircle {
friend class boost::serialization::access;
private:
	//--------------------------------------------------------------------------------------------------------
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
	virtual void render( CDrawContext *pContext ) = 0;
};

//============================================================================================================
//	Klasse GProcessorNode:
//  Repraesentiert ein Processor Knoten oder ProcessAdapter Objekt aus dem Graph. 
//  Ein GProcessorNode hatt ein oder mehrerer Ein-und Ausgaenge.
//============================================================================================================
class GProcessorNode : public GCircle {
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
	template < typename Archive >
	void serialize ( Archive &ar, const unsigned int version ){
		ar & boost::serialization::base_object< GObject > ( *this );
		ar & ins;
		ar & outs;
	}
	//--------------------------------------------------------------------------------------------------------
	GObject::Ptr  createInNode(CPoint &p);
	//--------------------------------------------------------------------------------------------------------
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
	const InputNodeContainer & getInputNodes() { return ins; } 
	//--------------------------------------------------------------------------------------------------------
	const OutputNodeContainer & getOutputNodes() { return outs; } 
	//--------------------------------------------------------------------------------------------------------
	void createIONodes ( int numInputs, int numOutputs ); 
	//--------------------------------------------------------------------------------------------------------
	void addIONodesOnView ( GObjectList &createdConnections ); 
	//--------------------------------------------------------------------------------------------------------
	virtual ~GProcessorNode(){}
	//--------------------------------------------------------------------------------------------------------
	void getIOs ( GObjectList &l ) const;
};
//============================================================================================================
//	Klasse GCView:
//  Erbt von CView. Diese Klasse ist leider noetig um das protected Attribut pParentView zu setzen
//  ohne frame->addView() zu verwenden. Da dies die GObject->onMouse() aufrufe uebergeht. 
//============================================================================================================
class GCView : public CView {
public:
	//--------------------------------------------------------------------------------------------------------
	void setFrame ( CFrame *frame ) {
		pParentFrame = frame;
	}
};
//============================================================================================================
//	Template CViewWrapper:
//  Huellklasse fuer VSTGUI::CView.
//  CView erbt von GOBJECT. GOBJECT muss unterobjekt von GObjekt sein.
//============================================================================================================
template < class GOBJECT >
class CViewWrapper : public GOBJECT {
protected:
	//--------------------------------------------------------------------------------------------------------
	CView *cView;
	//--------------------------------------------------------------------------------------------------------
	CViewWrapper ( CircuidView *parent, CView *cView );
public:
	//--------------------------------------------------------------------------------------------------------
	virtual void offset ( const CCoord &x, const CCoord &y ){
		GOBJECT::offset ( x, y );
		cView->setViewSize ( GOBJECT::bBox );
	}
	//--------------------------------------------------------------------------------------------------------
	CView * getCView(){ return cView; }
	//--------------------------------------------------------------------------------------------------------
	void moveTo ( const CPoint &p ){
		GOBJECT::moveTo (p);
		cView->setViewSize ( GOBJECT::bBox );
		cView->setMouseableArea ( GOBJECT::bBox );
	}
	//--------------------------------------------------------------------------------------------------------
	virtual ~CViewWrapper();
	//--------------------------------------------------------------------------------------------------------
	virtual void render ( CDrawContext *cc ) {
		for ( unsigned int i = 0; i<GOBJECT::subObjects.size(); i++ ) GOBJECT::subObjects[i]->render ( cc );
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
//	Klasse CControlWrapper:
//  Huellklasse fuer VSTGUI::CControl
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
	void setValue ( VstNumber val ){ cView->setValue ( val ); }
	//--------------------------------------------------------------------------------------------------------
	VstNumber getValue () const { return cView->getValue(); }
};
//============================================================================================================
//  Frei stehende Methode:
//  Punkt Rotieren um einen MPunkt
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
extern void rotate(CPoint &p, float alpha, const CPoint &center);
//------------------------------------------------------------------------------------------------------------
typedef list<GObject::Ptr>::const_iterator GObjectIterator;
//------------------------------------------------------------------------------------------------------------
extern VSTGUI::CRect getBoundingBox ( const GObjectIterator &begin, const GObjectIterator &end );
//------------------------------------------------------------------------------------------------------------
extern VSTGUI::CRect normalizeRect ( const VSTGUI::CRect &rect );
} //namespace ppiGui
#endif


