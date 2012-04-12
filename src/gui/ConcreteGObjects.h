/*
 * ===========================================================================================================
 * ConcreteGObjects.h
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#ifndef CONCRETEGOBJECTS_H
#define CONCRETEGOBJECTS_H

#include "PpiGui.h"
#include "com/serialization.h"
#define SAVE_CONNECTION    CircuidView *view = t->getParentView(); \
						   GObject::Ptr a = t->getObjectA();	   \
						   GObject::Ptr b = t->getObjectB();	   \
						   ar << view;							   \
						   ar << a;								   \
						   ar << b;								   
using namespace std;
namespace ppiGui{
//============================================================================================================
//	Klasse GSelection:
//  Auswahl eines oder mehrerer GObjecte.
//  Objekte lassen sich ueber auswahl bewegen.
//============================================================================================================
class GSelection;
//============================================================================================================
//	Klasse GVSTPlugNode:
//============================================================================================================
class GVSTPlugNode;
//============================================================================================================
// Klasse GInputNode:
// Repraesentiert den Eingangs Processor Node
//============================================================================================================
class GInputNode;
//============================================================================================================
// Klasse GOutputNode:
// Repraesentiert den Ausgangs Processor Node
//============================================================================================================
class GOutputNode;
//============================================================================================================
//	Klasse GVolumeNode:
//============================================================================================================
class GVolumeNode;
//============================================================================================================
//	Klasse GChannelSplit:
//============================================================================================================
class GPanAdapter;
//============================================================================================================
//	Klasse GPeakTracker:
//  Tranformiert Signal Lautstaerke in Parameter wert.
//============================================================================================================
class GPeakTracker;
//============================================================================================================
//	Klasse GADSRTrigger:
//  Triggert Eingangssignal als Gate fuer ADSR.
//============================================================================================================
class GADSRTrigger;
//============================================================================================================
//	Klasse GOutputStepNode:
//============================================================================================================
class GOutputStepNode;
//============================================================================================================
//	Klasse GOutputSwitch:
//============================================================================================================
class GOutputSwitch;
//============================================================================================================
//	Klasse GKnob:
//  Ein GKnob Objekt ist ein ueber eine Mausaktion regelbares Grafisches Objekt. 
//  Huellklasse VSTGUI::CAnimKob.
//============================================================================================================
class GKnob;
//============================================================================================================
//	Klasse GStdKnob: 
//  Standart Dreh-Knopf.
//============================================================================================================
class GStdKnob;
//============================================================================================================
//	Klasse GStdFreeKnob: 
//  Standart Dreh-Knopf.
//============================================================================================================
class GPassiveKnob;
//============================================================================================================
//	Klasse GButton: 
//  Huellklasse fuer VSTGUI::CMovieButton.
//============================================================================================================
class GButton;
//============================================================================================================
//	Klasse GFlag:
//  Ein an einem GObject anliegendes Label
//  mit einer verbindungslinie zum GObject.
//============================================================================================================
class GFlag;
//============================================================================================================
//	Klasse PlaceGObject:
//  Ein Place Objekt implementiert den Vorgang ein Objekt auf der View zu plazieren.
//  Es wird mit einem GObject Initalisiert und der View hinzugefuegt.
//  Dann folgt es solange der Maus bis ein linksklick erfolgt ist. Jetzt wird das enthaltene
//  GObject an Ort und stelle der view hinzugefuegt und das PlaceGObject entfernt sich aus der view.
//============================================================================================================
class PlaceGObject;
//============================================================================================================
//	Klasse GConnection:
// Verbindung zwischen zwei GObject objekten.
//============================================================================================================
class GConnection;
// Standart Connection
class GConnectionIO;
// Processor Input
class GConnectionPrIn;
// Processor Output
class GConnectionPrOut;
// Parameter Parameter
class GConnectionPaPa;
// Processor Parameter
class GConnectionPrPa;
// Connection Parameter
class GConnectionCoPa;
}// namespace ppiGui

namespace ppiGui{
//============================================================================================================
//	Klasse GSelection:
//  Auswahl eines oder mehrerer GObjecte.
//  Objekte lassen sich ueber auswahl bewegen.
//============================================================================================================
class GSelection : public GRect, EventListener<OnMouseDrag> {
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<GSelection> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
    int edgeLength;
	//--------------------------------------------------------------------------------------------------------
	inline void drawBorders(CDrawContext *cd);
protected:
	//--------------------------------------------------------------------------------------------------------
	GObjectList selection;
	//--------------------------------------------------------------------------------------------------------
	virtual void eventHandler ( void *src, const OnMouseDrag &ev );
	//--------------------------------------------------------------------------------------------------------
	GSelection ( CircuidView *parent ) : GRect ( parent ){ 
		name = "Selection"; 
		EventSender<OnMouseDrag>::addEventListener ( this );
	}
public:
	//--------------------------------------------------------------------------------------------------------
	static GSelection::Ptr create( CircuidView *parent ) {
		GSelection::Ptr neu( new GSelection(parent) );
		neu->_setSelfPtr ( neu );
		return neu;
	}
	//--------------------------------------------------------------------------------------------------------
	virtual void draw( CDrawContext *cc );
	//--------------------------------------------------------------------------------------------------------
	void addGObject ( const GObject::Ptr &gObj );
	//--------------------------------------------------------------------------------------------------------
	void addGObjects ( const GObjectList &gObjs ) { 
		GObjectList::const_iterator it = gObjs.begin();
		for ( ; it!=gObjs.end(); ++it ) addGObject (*it);
	}
	//--------------------------------------------------------------------------------------------------------
	bool contains ( const GObject::Ptr &gObj ) { 
		return com::contains < GObjectList > ( selection, gObj ); 
	}
	//--------------------------------------------------------------------------------------------------------
	void removeGObject ( const GObject::Ptr &gObj ) { selection.remove ( gObj ); }
	//--------------------------------------------------------------------------------------------------------
	void clear(){ 
		if ( selection.empty() ) return; 
		selection.clear(); 
		bBox = VSTGUI::CRect (0,0,0,0);
	}
	//--------------------------------------------------------------------------------------------------------
	size_t size() const { return selection.size(); }
	//--------------------------------------------------------------------------------------------------------
	bool empty() const { return selection.empty(); }
	//--------------------------------------------------------------------------------------------------------
	// verschiebt alle Objekte in der Selection um x,y
	virtual void offset ( const CCoord &x, const CCoord &y );
	//--------------------------------------------------------------------------------------------------------
	virtual void offset ( const CPoint &p ){ offset (p.x,p.y); }
	//--------------------------------------------------------------------------------------------------------
	// Liefert Bounding Box zur Selection.
	VSTGUI::CRect getBoundingBoxSelected() const;
	//--------------------------------------------------------------------------------------------------------
	virtual bool hitTest ( const CPoint &p ) const { return getBoundingBoxSelected().pointInside(p); }
};
//============================================================================================================
//	Klasse ButtonClicked: 
//============================================================================================================
struct ButtonClicked : public Event{
	//--------------------------------------------------------------------------------------------------------
	GButton * src;
	//--------------------------------------------------------------------------------------------------------
	ButtonClicked ( GButton *src ) : src(src) {}
};
//============================================================================================================
//	Klasse GFlag:
//  Ein an einem GObject anliegendes Label
//  mit einer verbindungslinie zum GObject.
//============================================================================================================
class GFlag : public GObject {
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<GFlag> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	MyString upper; // label oberhalb der linie.
	//--------------------------------------------------------------------------------------------------------
	MyString lower; // label unterhalb der linie.
	//--------------------------------------------------------------------------------------------------------
	GObject *src;
protected:
	//--------------------------------------------------------------------------------------------------------
	GFlag( CircuidView *view, GObject *parent );
public:
	//--------------------------------------------------------------------------------------------------------
	static GFlag::Ptr create( CircuidView *parent, GObject *src ) {
		GFlag::Ptr neu( new GFlag ( parent, src) );
		neu->_setSelfPtr ( neu );
		return neu;
	}
	//--------------------------------------------------------------------------------------------------------
	virtual void draw ( CDrawContext *cc );
	//--------------------------------------------------------------------------------------------------------
	void setUpperLabel ( const MyString &str ) { upper = str.trim(); }
	//--------------------------------------------------------------------------------------------------------
	void setLowerLabel ( const MyString &str ) { lower = str.trim(); }
	//--------------------------------------------------------------------------------------------------------
	// bestimmt die pos. vom parentObj mittelpunkt aus.
	void setPos ( float angle, int distance );
	//--------------------------------------------------------------------------------------------------------
	const MyString & getUpperLabel () const { return upper; }
	//--------------------------------------------------------------------------------------------------------
	const MyString & getLowerLabel () const { return lower; }
};
using namespace com;
using namespace events;
//============================================================================================================
//	Klasse GVSTPlugNode:
//============================================================================================================
class GVSTPlugNode : 
	public GProcessorNode, 
	public Serializable,
	public EventListener < ButtonClicked >,
	public EventSender < ButtonClicked >
{
friend class boost::serialization::access;
friend void load_construct_data( iArchive&, GVSTPlugNode*, const unsigned int);
BOOST_SERIALIZATION_SPLIT_MEMBER()
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<GVSTPlugNode> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	template < typename Archive >
	void save ( Archive &ar, const unsigned int version ) const {
		ar << boost::serialization::base_object<GProcessorNode> (*this);
		string header( getGFlag()->getUpperLabel() );
		ar << header;
	} 
	//--------------------------------------------------------------------------------------------------------
	template < typename Archive >
	void load ( Archive &ar, const unsigned int version ){
		ar >> boost::serialization::base_object<GProcessorNode> (*this);
		string header;
		ar >> header;
		getGFlag()->setUpperLabel( header );
	} 
	//--------------------------------------------------------------------------------------------------------
	enum { EBTN, FLAG };
protected:
	//--------------------------------------------------------------------------------------------------------
	CBitmap *skin;
	//--------------------------------------------------------------------------------------------------------
	GVSTPlugNode ( CircuidView *parent );
	//--------------------------------------------------------------------------------------------------------
	// checkt bei lMaustaste ob E button getroffen wurde.
	// ignoriert button bei rMaustatse.
	virtual void onMouse(CDrawContext *cc, CPoint &p, long btn);
public:
	//--------------------------------------------------------------------------------------------------------
	static GVSTPlugNode::Ptr create( CircuidView *parent ) {
		GVSTPlugNode::Ptr neu( new GVSTPlugNode(parent) );
		neu->_setSelfPtr ( neu );
		return neu;
	}
	//--------------------------------------------------------------------------------------------------------
	GButton * getEButton();
	//--------------------------------------------------------------------------------------------------------
	void eventHandler ( void *src, const ButtonClicked &ev ){
		EventSender<ButtonClicked>::notifyEventListeners ( this, ev );
	}
	//--------------------------------------------------------------------------------------------------------
	void setName ( const string &name );
	//--------------------------------------------------------------------------------------------------------
	GFlag * getGFlag() const;
	//--------------------------------------------------------------------------------------------------------
	virtual void draw( CDrawContext *pContext );
	//--------------------------------------------------------------------------------------------------------
	virtual ~GVSTPlugNode();
};
//============================================================================================================
// Frei stehende Methoden fuer boost archive: GVSTPlugNode
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
template < typename Archive >
inline void save_construct_data( Archive & ar, const GVSTPlugNode * t, const unsigned int file_version ){
	ppiGui::CircuidView *view = t->getParentView();
	ar << view;
}
//============================================================================================================
// Klasse GInputNode:
// Repraesentiert den Eingangs Processor Node
//============================================================================================================
class GInputNode : public GIONode, public Serializable {
friend class boost::serialization::access;
friend void load_construct_data( iArchive&, GInputNode*, const unsigned int);
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<GInputNode> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	template < typename Archive >
	void serialize ( Archive &ar, const unsigned int version ){
		ar & boost::serialization::base_object<GIONode> (*this);
	}
protected:	
	//--------------------------------------------------------------------------------------------------------
	CBitmap *skin;
	//--------------------------------------------------------------------------------------------------------
	Resources::BitmapID skinResID;
	//--------------------------------------------------------------------------------------------------------
	GInputNode ( CircuidView *parent, Resources::BitmapID skinResID );
public:
	//--------------------------------------------------------------------------------------------------------
	static GInputNode::Ptr create( CircuidView *parent, Resources::BitmapID skinResID ) {
		GInputNode::Ptr neu( new GInputNode ( parent, skinResID ) );
		neu->_setSelfPtr ( neu );
		return neu;
	}
	//--------------------------------------------------------------------------------------------------------
	Resources::BitmapID getSkinResID () const { return skinResID; }
	//--------------------------------------------------------------------------------------------------------
	virtual void draw( CDrawContext *pContext );
	//--------------------------------------------------------------------------------------------------------
	virtual ~GInputNode();
};
//============================================================================================================
// Frei stehende Methoden fuer boost archive: GInputNode
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
template <typename Archive>
inline void save_construct_data( Archive & ar, const GInputNode * t, const unsigned int file_version ){
	ppiGui::CircuidView *view = t->getParentView();
	Resources::BitmapID resId = t->getSkinResID(); 
	ar << view;
	ar << resId;
}
//============================================================================================================
// Klasse GOutputNode:
// Repraesentiert den Ausgangs Processor Node
//============================================================================================================
class GOutputNode : public GIONode, public Serializable {
friend class boost::serialization::access;
friend void load_construct_data( iArchive&, GOutputNode*, const unsigned int);
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<GOutputNode> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	template < typename Archive >
	void serialize ( Archive &ar, const unsigned int version ){
		ar & boost::serialization::base_object<GIONode> (*this);
	}
protected:
	//--------------------------------------------------------------------------------------------------------
	CBitmap *skin;
	//--------------------------------------------------------------------------------------------------------
	Resources::BitmapID skinResID;
	//--------------------------------------------------------------------------------------------------------
	GOutputNode ( CircuidView *parent, Resources::BitmapID skinResID  );
public:
	//--------------------------------------------------------------------------------------------------------
	static GOutputNode::Ptr create( CircuidView *parent, Resources::BitmapID skinResID ) {
		GOutputNode::Ptr neu( new GOutputNode ( parent, skinResID ) );
		neu->_setSelfPtr ( neu );
		return neu;
	}
	//--------------------------------------------------------------------------------------------------------
	Resources::BitmapID getSkinResID () const { return skinResID; }
	//--------------------------------------------------------------------------------------------------------
	virtual void draw( CDrawContext *pContext );
	//--------------------------------------------------------------------------------------------------------
	virtual ~GOutputNode();
};
//============================================================================================================
// Frei stehende Methoden fuer boost archive: GOutputNode
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
template < typename Archive >
inline void save_construct_data( Archive & ar, const GOutputNode * t, const unsigned int file_version ){
	ppiGui::CircuidView *view = t->getParentView();
	Resources::BitmapID resId = t->getSkinResID(); 
	ar << view;
	ar << resId;
}
//============================================================================================================
//	Klasse GVolumeNode:
//============================================================================================================
class GVolumeNode : public GProcessorNode, public Serializable {
friend class boost::serialization::access;
friend void load_construct_data( iArchive&, GVolumeNode*, const unsigned int);
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<GVolumeNode> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	template < typename Archive >
	void serialize ( Archive &ar, const unsigned int version ){
		ar & boost::serialization::base_object< GProcessorNode > ( *this );
	}
	//--------------------------------------------------------------------------------------------------------
	CBitmap *skin;
protected:
	//--------------------------------------------------------------------------------------------------------
	GVolumeNode ( CircuidView *view );
public:
	//--------------------------------------------------------------------------------------------------------
	static GVolumeNode::Ptr create( CircuidView *parent ) {
		GVolumeNode::Ptr neu( new GVolumeNode ( parent ) );
		neu->_setSelfPtr ( neu );
		return neu;
	}
	//--------------------------------------------------------------------------------------------------------
	virtual void draw( CDrawContext *cc );
	//--------------------------------------------------------------------------------------------------------
	virtual ~GVolumeNode();
};
//============================================================================================================
// Frei stehende Methoden fuer boost archive: GVolumeNode
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
template < typename Archive >
inline void save_construct_data( Archive & ar, const GVolumeNode * t, const unsigned int file_version ){
	ppiGui::CircuidView *view = t->getParentView();
	ar << view;
}
//============================================================================================================
//	Klasse GPanAdapter:
//============================================================================================================
class GPanAdapter : public GProcessorNode, public Serializable {
friend class boost::serialization::access;
friend void load_construct_data( iArchive&, GPanAdapter*, const unsigned int);
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<GPanAdapter> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	template < typename Archive >
	void serialize ( Archive &ar, const unsigned int version ){
		ar & boost::serialization::base_object< GProcessorNode > ( *this );
	}
	//--------------------------------------------------------------------------------------------------------
	CBitmap *skin;
protected:
	//--------------------------------------------------------------------------------------------------------
	GPanAdapter ( CircuidView *view );
public:
	//--------------------------------------------------------------------------------------------------------
	static GPanAdapter::Ptr create( CircuidView *parent ) {
		GPanAdapter::Ptr neu( new GPanAdapter ( parent ) );
		neu->_setSelfPtr ( neu );
		return neu;
	}
	//--------------------------------------------------------------------------------------------------------
	virtual void draw( CDrawContext *cc );
	//--------------------------------------------------------------------------------------------------------
	virtual ~GPanAdapter();
};
//============================================================================================================
// Frei stehende Methoden fuer boost archive: GPanAdapter
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
template < typename Archive >
inline void save_construct_data( Archive & ar, const GPanAdapter * t, const unsigned int file_version ){
	ppiGui::CircuidView *view = t->getParentView();
	ar << view;
}
//============================================================================================================
//	Klasse GOutputStepNode:
//============================================================================================================
class GOutputStepNode : public GProcessorNode, public Serializable, public IHasState {
friend class boost::serialization::access;
friend void load_construct_data( iArchive&, GOutputStepNode*, const unsigned int);
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<GOutputStepNode> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	template < typename Archive >
	void serialize ( Archive &ar, const unsigned int version ){
		ar & boost::serialization::base_object< GProcessorNode > ( *this );
		ar & aONodeID;
	}
	//--------------------------------------------------------------------------------------------------------
	CBitmap *skin;
	//--------------------------------------------------------------------------------------------------------
	GBitmap::Ptr glow; // leuchten aktivierter nodes; 
	//--------------------------------------------------------------------------------------------------------
	size_t aONodeID;
protected:
	//--------------------------------------------------------------------------------------------------------
	GOutputStepNode ( CircuidView *view );
public:
	//--------------------------------------------------------------------------------------------------------
	// gesetzter knoten wird markiert dargestellt
	virtual void setState ( size_t nr ) { aONodeID = nr; }
	//--------------------------------------------------------------------------------------------------------
	virtual size_t getState() const { return aONodeID; }
	//--------------------------------------------------------------------------------------------------------
	static GOutputStepNode::Ptr create( CircuidView *parent ) {
		GOutputStepNode::Ptr neu( new GOutputStepNode ( parent ) );
		neu->_setSelfPtr ( neu );
		return neu;
	}
	//--------------------------------------------------------------------------------------------------------
	virtual void draw( CDrawContext *cc );
	//--------------------------------------------------------------------------------------------------------
	virtual ~GOutputStepNode();
};
//============================================================================================================
// Frei stehende Methoden fuer boost archive: GOutputStepNode
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
template < typename Archive >
inline void save_construct_data( Archive & ar, const GOutputStepNode * t, const unsigned int file_version ){
	ppiGui::CircuidView *view = t->getParentView();
	ar << view;
}
//============================================================================================================
//	Klasse GInputStepNode:
//============================================================================================================
class GInputStepNode : public GProcessorNode, public Serializable, public IHasState {
friend class boost::serialization::access;
friend void load_construct_data( iArchive&, GInputStepNode*, const unsigned int);
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<GInputStepNode> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	template < typename Archive >
	void serialize ( Archive &ar, const unsigned int version ){
		ar & boost::serialization::base_object< GProcessorNode > ( *this );
		ar & aINodeID;
	}
	//--------------------------------------------------------------------------------------------------------
	CBitmap *skin;
	//--------------------------------------------------------------------------------------------------------
	GBitmap::Ptr glow; // leuchten aktivierter nodes; 
	//--------------------------------------------------------------------------------------------------------
	size_t aINodeID;
protected:
	//--------------------------------------------------------------------------------------------------------
	GInputStepNode ( CircuidView *view );
public:
	//--------------------------------------------------------------------------------------------------------
	// gesetzter knoten wird markiert dargestellt
	virtual void setState ( size_t nr ) { aINodeID = nr; }
	//--------------------------------------------------------------------------------------------------------
	virtual size_t getState() const { return aINodeID; }
	//--------------------------------------------------------------------------------------------------------
	static GInputStepNode::Ptr create( CircuidView *parent ) {
		GInputStepNode::Ptr neu( new GInputStepNode ( parent ) );
		neu->_setSelfPtr ( neu );
		return neu;
	}
	//--------------------------------------------------------------------------------------------------------
	virtual void draw( CDrawContext *cc );
	//--------------------------------------------------------------------------------------------------------
	virtual ~GInputStepNode();
};
//============================================================================================================
// Frei stehende Methoden fuer boost archive: GOutputStepNode
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
template < typename Archive >
inline void save_construct_data( Archive & ar, const GInputStepNode * t, const unsigned int file_version ){
	ppiGui::CircuidView *view = t->getParentView();
	ar << view;
}
//============================================================================================================
//	Klasse GOutputSwitch:
//============================================================================================================
class GOutputSwitch : public GProcessorNode, public Serializable, public IHasState {
friend class boost::serialization::access;
friend void load_construct_data( iArchive&, GOutputSwitch*, const unsigned int);
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<GOutputSwitch> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	template < typename Archive >
	void serialize ( Archive &ar, const unsigned int version ){
		ar & boost::serialization::base_object< GProcessorNode > ( *this );
		ar & aONodeID;
	}
	//--------------------------------------------------------------------------------------------------------
	CBitmap *skin;
	//--------------------------------------------------------------------------------------------------------
	GBitmap::Ptr glow; // leuchten aktivierter nodes; 
	//--------------------------------------------------------------------------------------------------------
	size_t aONodeID;
protected:
	//--------------------------------------------------------------------------------------------------------
	GOutputSwitch ( CircuidView *view );
public:
	//--------------------------------------------------------------------------------------------------------
	static GOutputSwitch::Ptr create( CircuidView *parent ) {
		GOutputSwitch::Ptr neu( new GOutputSwitch ( parent ) );
		neu->_setSelfPtr ( neu );
		return neu;
	}
	//--------------------------------------------------------------------------------------------------------
	// gesetzter knoten wird markiert dargestellt
	virtual void setState ( size_t nr ) { aONodeID = nr; }
	//--------------------------------------------------------------------------------------------------------
	virtual size_t getState() const { return aONodeID; }
	//--------------------------------------------------------------------------------------------------------
	virtual void draw( CDrawContext *cc );
	//--------------------------------------------------------------------------------------------------------
	virtual ~GOutputSwitch();
};
//============================================================================================================
// Frei stehende Methoden fuer boost archive: GOutputSwitch
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
template < typename Archive >
inline void save_construct_data( Archive & ar, const GOutputSwitch * t, const unsigned int file_version ){
	ppiGui::CircuidView *view = t->getParentView();
	ar << view;
}
//============================================================================================================
//	Klasse GInputSwitch:
//============================================================================================================
class GInputSwitch : public GProcessorNode, public Serializable, public IHasState {
friend class boost::serialization::access;
friend void load_construct_data( iArchive&, GInputSwitch*, const unsigned int);
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<GInputSwitch> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	template < typename Archive >
	void serialize ( Archive &ar, const unsigned int version ){
		ar & boost::serialization::base_object< GProcessorNode > ( *this );
		ar & aINodeID;
	}
	//--------------------------------------------------------------------------------------------------------
	CBitmap *skin;
	//--------------------------------------------------------------------------------------------------------
	GBitmap::Ptr glow; // leuchten aktivierter nodes; 
	//--------------------------------------------------------------------------------------------------------
	size_t aINodeID;
protected:
	//--------------------------------------------------------------------------------------------------------
	GInputSwitch ( CircuidView *view );
public:
	//--------------------------------------------------------------------------------------------------------
	static GInputSwitch::Ptr create( CircuidView *parent ) {
		GInputSwitch::Ptr neu( new GInputSwitch ( parent ) );
		neu->_setSelfPtr ( neu );
		return neu;
	}
	//--------------------------------------------------------------------------------------------------------
	// gesetzter knoten wird markiert dargestellt
	virtual void setState ( size_t nr ) { aINodeID = nr; }
	//--------------------------------------------------------------------------------------------------------
	virtual size_t getState() const { return aINodeID; }
	//--------------------------------------------------------------------------------------------------------
	virtual void draw( CDrawContext *cc );
	//--------------------------------------------------------------------------------------------------------
	virtual ~GInputSwitch();
};
//============================================================================================================
// Frei stehende Methoden fuer boost archive: GInputSwitch
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
template < typename Archive >
inline void save_construct_data( Archive & ar, const GInputSwitch * t, const unsigned int file_version ){
	ppiGui::CircuidView *view = t->getParentView();
	ar << view;
}
//============================================================================================================
//	Klasse GPeakTracker:
//  Tranformiert Signal Lautstaerke in Parameter wert.
//============================================================================================================
class GPeakTracker : public GProcessorNode, public Serializable {
friend class boost::serialization::access;
friend void load_construct_data( iArchive&, GPeakTracker*, const unsigned int);
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<GPeakTracker> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	template < typename Archive >
	void serialize ( Archive &ar, const unsigned int version ){
		ar & boost::serialization::base_object< GProcessorNode > ( *this );
	}
	//--------------------------------------------------------------------------------------------------------
	CBitmap *skin;
protected:
	//--------------------------------------------------------------------------------------------------------
	GPeakTracker ( CircuidView *view );
public:
	//--------------------------------------------------------------------------------------------------------
	static GPeakTracker::Ptr create( CircuidView *parent ) {
		GPeakTracker::Ptr neu( new GPeakTracker ( parent ) );
		neu->_setSelfPtr ( neu );
		return neu;
	}
	//--------------------------------------------------------------------------------------------------------
	virtual void draw( CDrawContext *cc );
	//--------------------------------------------------------------------------------------------------------
	virtual ~GPeakTracker();
};
//============================================================================================================
// Frei stehende Methoden fuer boost archive: GPeakTracker
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
template < typename Archive >
inline void save_construct_data( Archive & ar, const GPeakTracker * t, const unsigned int file_version ){
	ppiGui::CircuidView *view = t->getParentView();
	ar << view;
}
//============================================================================================================
//	Klasse GADSRTrigger:
//  Triggert Eingangssignal als Gate fuer ADSR.
//============================================================================================================
class GADSRTrigger : public GProcessorNode, public Serializable {
friend class boost::serialization::access;
friend void load_construct_data( iArchive&, GADSRTrigger*, const unsigned int);
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<GADSRTrigger> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	template < typename Archive >
	void serialize ( Archive &ar, const unsigned int version ){
		ar & boost::serialization::base_object< GProcessorNode > ( *this );
	} 
	//--------------------------------------------------------------------------------------------------------
	CBitmap *skin;
protected:
	//--------------------------------------------------------------------------------------------------------
	GADSRTrigger ( CircuidView *view );
public:
	//--------------------------------------------------------------------------------------------------------
	static GADSRTrigger::Ptr create( CircuidView *parent ) {
		GADSRTrigger::Ptr neu( new GADSRTrigger ( parent ) );
		neu->_setSelfPtr ( neu );
		return neu;
	}
	//--------------------------------------------------------------------------------------------------------
	virtual void draw( CDrawContext *cc );
	//--------------------------------------------------------------------------------------------------------
	virtual ~GADSRTrigger();
};
//============================================================================================================
// Frei stehende Methoden fuer boost archive: GADSRTrigger
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
template < typename Archive >
inline void save_construct_data( Archive & ar, const GADSRTrigger * t, const unsigned int file_version ){
	ppiGui::CircuidView *view = t->getParentView();
	ar << view;
}
//============================================================================================================
//	Klasse GMidiProcessor:
//============================================================================================================
class GMidiProcessor : public GProcessorNode, public Serializable {
friend class boost::serialization::access;
friend void load_construct_data( iArchive&, GMidiProcessor*, const unsigned int);
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<GMidiProcessor> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	template < typename Archive >
	void serialize ( Archive &ar, const unsigned int version ){
		ar & boost::serialization::base_object< GProcessorNode > ( *this );
	} 
	//--------------------------------------------------------------------------------------------------------
	CBitmap *skin;
protected:
	//--------------------------------------------------------------------------------------------------------
	GMidiProcessor ( CircuidView *view );
public:
	//--------------------------------------------------------------------------------------------------------
	static GMidiProcessor::Ptr create( CircuidView *parent ) {
		GMidiProcessor::Ptr neu( new GMidiProcessor ( parent ) );
		neu->_setSelfPtr ( neu );
		return neu;
	}
	//--------------------------------------------------------------------------------------------------------
	virtual void draw( CDrawContext *cc );
	//--------------------------------------------------------------------------------------------------------
	virtual ~GMidiProcessor();
};
//============================================================================================================
// Frei stehende Methoden fuer boost archive: GMidiProcessor
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
template < typename Archive >
inline void save_construct_data( Archive & ar, const GMidiProcessor * t, const unsigned int file_version ) {
	ppiGui::CircuidView *view = t->getParentView();
	ar << view;
}
//============================================================================================================
//	Klasse GLuaProcessor:
//============================================================================================================
class GLuaProcessor : public GProcessorNode, public Serializable {
friend class boost::serialization::access;
friend void load_construct_data( iArchive&, GLuaProcessor*, const unsigned int);
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<GLuaProcessor> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	template < typename Archive >
	void serialize ( Archive &ar, const unsigned int version ){
		ar & boost::serialization::base_object< GProcessorNode > ( *this );
	} 
	//--------------------------------------------------------------------------------------------------------
	CBitmap *skin;
protected:
	//--------------------------------------------------------------------------------------------------------
	GLuaProcessor ( CircuidView *view );
public:
	//--------------------------------------------------------------------------------------------------------
	static GLuaProcessor::Ptr create( CircuidView *parent ) {
		GLuaProcessor::Ptr neu( new GLuaProcessor ( parent ) );
		neu->_setSelfPtr ( neu );
		return neu;
	}
	//--------------------------------------------------------------------------------------------------------
	virtual void draw( CDrawContext *cc );
	//--------------------------------------------------------------------------------------------------------
	virtual ~GLuaProcessor();
};
//============================================================================================================
// Frei stehende Methoden fuer boost archive: GLuaProcessor
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
template < typename Archive >
void save_construct_data(Archive & ar, const GLuaProcessor * t, const unsigned int file_version ) {
	ppiGui::CircuidView *view = t->getParentView();
	ar << view;
}
//============================================================================================================
//	Klasse GRemoteChannelReceiver:
//============================================================================================================
class GRemoteChannelReceiver : public GProcessorNode, public Serializable {
friend class boost::serialization::access;
friend void load_construct_data( iArchive&, GRemoteChannelReceiver*, const unsigned int);
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<GRemoteChannelReceiver> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	template < typename Archive >
	void serialize ( Archive &ar, const unsigned int version ){
		ar & boost::serialization::base_object< GProcessorNode > ( *this );
	} 
	//--------------------------------------------------------------------------------------------------------
	CBitmap *skin;
protected:
	//--------------------------------------------------------------------------------------------------------
	GRemoteChannelReceiver ( CircuidView *view );
public:
	//--------------------------------------------------------------------------------------------------------
	static Ptr create( CircuidView *parent ) {
		Ptr neu( new GRemoteChannelReceiver ( parent ) );
		neu->_setSelfPtr ( neu );
		return neu;
	}
	//--------------------------------------------------------------------------------------------------------
	virtual void draw( CDrawContext *cc );
	//--------------------------------------------------------------------------------------------------------
	virtual ~GRemoteChannelReceiver();
};
//============================================================================================================
// Frei stehende Methoden fuer boost archive: GRemoteChannelReceiver
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
template < typename Archive >
void save_construct_data(Archive & ar, const GRemoteChannelReceiver * t, const unsigned int file_version ) {
	ppiGui::CircuidView *view = t->getParentView();
	ar << view;
}
//============================================================================================================
//	Klasse GConnection:
//  Verbindung zwischen zwei GObjects dargestellt als Linie. 
//============================================================================================================
class GConnection : public GLine {
friend class boost::serialization::access;
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<GConnection> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	template <typename Archiv>
	void serialize ( Archiv &ar, const unsigned int version ){
		ar & boost::serialization::base_object<GLine> (*this);
	}
protected:
	//--------------------------------------------------------------------------------------------------------
	bool onMouse;
	//--------------------------------------------------------------------------------------------------------
	GObject::Ptr o1;
	//--------------------------------------------------------------------------------------------------------
	GObject::Ptr o2;
	//--------------------------------------------------------------------------------------------------------
	GConnection ( CircuidView *parent, const GObject::Ptr &o1, const GObject::Ptr &o2 );
	//--------------------------------------------------------------------------------------------------------
	//string debug_info;
	//--------------------------------------------------------------------------------------------------------
	virtual void onMouseEnter( const CPoint &p ) {
		onMouse = true;
		setObjectDirty();
	}
	//--------------------------------------------------------------------------------------------------------
	virtual void onMouseLeave( const CPoint &p ) {
		onMouse = false;
		setObjectDirty();
	}
	//--------------------------------------------------------------------------------------------------------
public:
	//--------------------------------------------------------------------------------------------------------
	GObject::Ptr getObjectA() const { return o1; }
	//--------------------------------------------------------------------------------------------------------
	GObject::Ptr getObjectB() const { return o2; }
	//--------------------------------------------------------------------------------------------------------
	void setObjectA( const GObject::Ptr &a ) { o1 = a; }
	//--------------------------------------------------------------------------------------------------------
	void setObjectB( const GObject::Ptr &b ) { o2 = b; }
	//--------------------------------------------------------------------------------------------------------
	virtual void draw ( CDrawContext *cc );
	//--------------------------------------------------------------------------------------------------------
	virtual void drawConnection ( CDrawContext *cc ) = 0;
	//--------------------------------------------------------------------------------------------------------
};

//============================================================================================================
// Klasse GConnectionIO
// Standart Connection
//============================================================================================================
class GConnectionIO : public GConnection, public Serializable {
friend class boost::serialization::access;
friend void load_construct_data( iArchive&, GConnectionIO*, const unsigned int);
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<GConnectionIO> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	template <typename Archiv>
	void serialize ( Archiv &ar, const unsigned int version ){
		ar & boost::serialization::base_object< GConnection > ( *this );
	}
protected:
	//--------------------------------------------------------------------------------------------------------
	GConnectionIO ( CircuidView *parent, const GObject::Ptr &o1, const GObject::Ptr &o2 ) : 
		 GConnection ( parent, o1, o2 ){}
public:
	//--------------------------------------------------------------------------------------------------------
	virtual void drawConnection ( CDrawContext *cc );
	//--------------------------------------------------------------------------------------------------------
	static GConnectionIO::Ptr create( CircuidView *parent, const GObject::Ptr &o1, const GObject::Ptr &o2 ) {
		GConnectionIO::Ptr neu( new GConnectionIO ( parent, o1, o2 ) );
		neu->_setSelfPtr ( neu );
		return neu;
	}
};
//============================================================================================================
// Frei stehende Methoden fuer boost archive: GConnectionIO
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
template < typename Archive >
inline void save_construct_data( Archive & ar, const GConnectionIO * t, const unsigned int file_version ){
	SAVE_CONNECTION
}
//============================================================================================================
// Klasse GConnectionPrIn
// Processor Input
//============================================================================================================
class GConnectionPrIn : public GConnection, public Serializable {
friend class boost::serialization::access;
friend void load_construct_data( iArchive&, GConnectionPrIn*, const unsigned int);
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<GConnectionPrIn> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	template <typename Archiv>
	void serialize ( Archiv &ar, const unsigned int version ){
		ar & boost::serialization::base_object< GConnection > ( *this );
	}
protected:
	//--------------------------------------------------------------------------------------------------------
	GConnectionPrIn ( CircuidView *parent, const GObject::Ptr &o1, const GObject::Ptr &o2 )
		:GConnection ( parent, o1, o2 ){}
public:
	//--------------------------------------------------------------------------------------------------------
	virtual void drawConnection ( CDrawContext *cc );
	//--------------------------------------------------------------------------------------------------------
	static GConnectionPrIn::Ptr create( CircuidView *parent, const GObject::Ptr &o1, const GObject::Ptr &o2 ) {
		GConnectionPrIn::Ptr neu( new GConnectionPrIn ( parent, o1, o2 ) );
		neu->_setSelfPtr ( neu );
		return neu;
	}
};
//============================================================================================================
// Frei stehende Methoden fuer boost archive: GConnectionPrIn
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
template < typename Archive >
inline void save_construct_data( Archive & ar, const GConnectionPrIn * t, const unsigned int file_version ){
	SAVE_CONNECTION
}
//============================================================================================================
// Klasse GConnectionPrOut
// Processor Output
//============================================================================================================
class GConnectionPrOut : public GConnection, public Serializable {
friend class boost::serialization::access;
friend void load_construct_data( iArchive&, GConnectionPrOut*, const unsigned int);
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<GConnectionPrOut> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	template <typename Archiv>
	void serialize ( Archiv &ar, const unsigned int version ){
		ar & boost::serialization::base_object< GConnection > ( *this );
	}
protected:
	//--------------------------------------------------------------------------------------------------------
	GConnectionPrOut ( CircuidView *parent, const GObject::Ptr &o1, const GObject::Ptr &o2 ):
		 GConnection ( parent, o1, o2 ){}
public:
	//--------------------------------------------------------------------------------------------------------
	virtual void drawConnection ( CDrawContext *cc );
	//--------------------------------------------------------------------------------------------------------
	static GConnectionPrOut::Ptr create( CircuidView *parent, const GObject::Ptr &o1, const GObject::Ptr &o2 ) 
	{
		GConnectionPrOut::Ptr neu( new GConnectionPrOut ( parent, o1, o2 ) );
		neu->_setSelfPtr ( neu );
		return neu;
	}
};
//============================================================================================================
// Frei stehende Methoden fuer boost archive: GConnectionPrOut
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
template < typename Archive >
inline void save_construct_data( Archive & ar, const GConnectionPrOut * t, const unsigned int file_version ){
	SAVE_CONNECTION
}
//============================================================================================================
// Klasse GConnectionPaPa
// Parameter Parameter
//============================================================================================================
class GConnectionPaPa : public GConnection, public Serializable {
friend class boost::serialization::access;
friend void load_construct_data( iArchive&, GConnectionPaPa*, const unsigned int);
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<GConnectionPaPa> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	template <typename Archiv>
	void serialize ( Archiv &ar, const unsigned int version ){
		ar & boost::serialization::base_object< GConnection > ( *this );
	}
protected:
	//--------------------------------------------------------------------------------------------------------
	GConnectionPaPa ( CircuidView *parent, const GObject::Ptr &o1, const GObject::Ptr &o2 )
		:GConnection ( parent, o1, o2 ){}
public:
	//--------------------------------------------------------------------------------------------------------
	virtual void drawConnection ( CDrawContext *cc );
	//--------------------------------------------------------------------------------------------------------
	static GConnectionPaPa::Ptr create( CircuidView *parent, const GObject::Ptr &o1, const GObject::Ptr &o2 ) {
		GConnectionPaPa::Ptr neu( new GConnectionPaPa ( parent, o1, o2 ) );
		neu->_setSelfPtr ( neu );
		return neu;
	}
};
//============================================================================================================
// Frei stehende Methoden fuer boost archive: GConnectionPaPa
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
template < typename Archive >
inline void save_construct_data( Archive & ar, const GConnectionPaPa * t, const unsigned int file_version ){
	SAVE_CONNECTION
}
//============================================================================================================
// Klasse GConnectionPrPa
// Processor Parameter
//============================================================================================================
class GConnectionPrPa : public GConnection, public Serializable {
friend class boost::serialization::access;
friend void load_construct_data( iArchive&, GConnectionPrPa*, const unsigned int);
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<GConnectionPrPa> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	template <typename Archiv>
	void serialize ( Archiv &ar, const unsigned int version ){
		ar & boost::serialization::base_object< GConnection > ( *this );
	}
protected:
	//--------------------------------------------------------------------------------------------------------
	GConnectionPrPa ( CircuidView *parent, const GObject::Ptr &o1, const GObject::Ptr &o2 ):
		 GConnection ( parent, o1, o2 ){}
public:
	//--------------------------------------------------------------------------------------------------------
	virtual void drawConnection ( CDrawContext *cc );
	//--------------------------------------------------------------------------------------------------------
	static GConnectionPrPa::Ptr create( CircuidView *parent, const GObject::Ptr &o1, const GObject::Ptr &o2 ) {
		GConnectionPrPa::Ptr neu( new GConnectionPrPa ( parent, o1, o2 ) );
		neu->_setSelfPtr ( neu );
		return neu;
	}
};
//============================================================================================================
// Klasse GConnectionCoPa
// Connection Parameter
//============================================================================================================
class GConnectionCoPa : public GConnection, public Serializable {
friend class boost::serialization::access;
friend void load_construct_data( iArchive&, GConnectionCoPa*, const unsigned int);
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<GConnectionCoPa> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	template <typename Archiv>
	void serialize ( Archiv &ar, const unsigned int version ){
		ar & boost::serialization::base_object< GConnection > ( *this );
	}
protected:
	//--------------------------------------------------------------------------------------------------------
	GConnectionCoPa ( CircuidView *parent, const GObject::Ptr &o1, const GObject::Ptr &o2 ) 
		: GConnection ( parent, o1, o2 ){}
public:
	//--------------------------------------------------------------------------------------------------------
	virtual void drawConnection ( CDrawContext *cc );
	//--------------------------------------------------------------------------------------------------------
	static GConnectionCoPa::Ptr create( CircuidView *parent, const GObject::Ptr &o1, const GObject::Ptr &o2 )
	{
		GConnectionCoPa::Ptr neu( new GConnectionCoPa ( parent, o1, o2 ) );
		neu->_setSelfPtr ( neu );
		return neu;
	}
};
//============================================================================================================
// Frei stehende Methoden fuer boost archive: GConnectionCoPa
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
template < typename Archive >
inline void save_construct_data( Archive & ar, const GConnectionPrPa * t, const unsigned int file_version ){
	SAVE_CONNECTION
}
//============================================================================================================
//	Klasse GKnob:
//  Ein Regler der mit einem Parameter verbunden ist.
//============================================================================================================
class GKnob : public CControlWrapper<GCircle>, 
			  public ValueChangedSender<float>,
			  public CControlListener
{
friend class boost::serialization::access; 
BOOST_SERIALIZATION_SPLIT_MEMBER()
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<GKnob> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	void save( oArchive &ar, const unsigned int version ) const;
	//--------------------------------------------------------------------------------------------------------
	void load( iArchive &ar, const unsigned int version );
	//--------------------------------------------------------------------------------------------------------
	bool enabled;
	//--------------------------------------------------------------------------------------------------------
	void valueChanged ( CDrawContext *cD, CControl *cc ){
		this->notifyListeners ( this, getValue() );
		this->setObjectDirty();
	}
protected:
	//--------------------------------------------------------------------------------------------------------
	GFlag::Ptr gFlag;
	//--------------------------------------------------------------------------------------------------------
	enum { MARK };
	//--------------------------------------------------------------------------------------------------------
	virtual void endEdit ();
	//--------------------------------------------------------------------------------------------------------
	GKnob ( CircuidView *parent, CAnimKnob *knob );
public:
	//--------------------------------------------------------------------------------------------------------
	void setEnabled ( bool b = true ){ enabled = b; }
	//--------------------------------------------------------------------------------------------------------
	bool isEnabled (){ return enabled; }
	//--------------------------------------------------------------------------------------------------------
	virtual ~GKnob();
	//--------------------------------------------------------------------------------------------------------
	void setDisplay ( const MyString &str ) { gFlag->setLowerLabel( str ); }
	//--------------------------------------------------------------------------------------------------------
	void setHeader ( const MyString &str ) { gFlag->setUpperLabel( str ); }
	//--------------------------------------------------------------------------------------------------------
	const MyString & getDisplay() const { return gFlag->getLowerLabel(); }
	//--------------------------------------------------------------------------------------------------------
	const MyString & getHeader() const { return gFlag->getUpperLabel(); }
	//--------------------------------------------------------------------------------------------------------
	// erzuegt CAnimKnob
	static CAnimKnob * createNewCAnimKnob ( Resources::BitmapID resId, int subPix );
};
//============================================================================================================
//	Klasse GStdKnob: 
//  Standart Dreh-Knopf.
//============================================================================================================
class GStdKnob : public GKnob, public Serializable {
friend class boost::serialization::access; 
friend void load_construct_data( iArchive&, GStdKnob*, const unsigned int);
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<GStdKnob> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	template < typename Archive >
	void serialize ( Archive &ar, const unsigned int version ) {
		ar & boost::serialization::base_object<GKnob>(*this);
	}
protected:
	//--------------------------------------------------------------------------------------------------------
	GStdKnob( CircuidView *view );
public:
	//--------------------------------------------------------------------------------------------------------
	virtual ~GStdKnob(){}
	//--------------------------------------------------------------------------------------------------------
	static GStdKnob::Ptr create( CircuidView *parent ) {
		GStdKnob::Ptr neu( new GStdKnob ( parent ) );
		neu->_setSelfPtr ( neu );
		return neu;
	}
};
//============================================================================================================
// Frei stehende Methoden fuer boost archive: GStdKnob
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
template < typename Archive >
inline void save_construct_data( Archive & ar, const GStdKnob * t, const unsigned int file_version ){
	ppiGui::CircuidView *view = t->getParentView();
	ar << view;
}
//============================================================================================================
//	Klasse GPassiveKnob: 
//  Knopf dessen Wert nicht direkt per Maus eingestellt werden kann. 
//============================================================================================================
class GPassiveKnob : public GKnob, public Serializable {
friend class boost::serialization::access;
friend void load_construct_data( iArchive&, GPassiveKnob*, const unsigned int);
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<GPassiveKnob> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	template < typename Archive >
	void serialize ( Archive &ar, const unsigned int version ) {
		ar & boost::serialization::base_object<GKnob>(*this);
	}
protected:
	//--------------------------------------------------------------------------------------------------------
	GPassiveKnob( CircuidView *view );
public:
	//--------------------------------------------------------------------------------------------------------
	virtual ~GPassiveKnob(){}
	//--------------------------------------------------------------------------------------------------------
	static GPassiveKnob::Ptr create( CircuidView *parent ) {
		GPassiveKnob::Ptr neu( new GPassiveKnob ( parent ) );
		neu->_setSelfPtr ( neu );
		return neu;
	}
};
//============================================================================================================
// Frei stehende Methoden fuer boost archive: GPassiveKnob
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
template < typename Archive >
inline void save_construct_data( Archive & ar, const GPassiveKnob * t, const unsigned int file_version ){
	ppiGui::CircuidView *view = t->getParentView();
	ar << view;
}
//============================================================================================================
//	Klasse GButton: 
//  Erbt von VSTGUI::CMovieButton.
//============================================================================================================
class GButton : 
	public CControlWrapper<GCircle>, 
	public CControlListener,
	public EventSender<ButtonClicked>
{
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<GButton> Ptr;
private:
protected:
	//--------------------------------------------------------------------------------------------------------
	// valueChanged methode von CControlListener.
	// Arbeitet ValueChagedListener Liste ab.
	virtual void valueChanged ( CDrawContext *pContext, CControl *pControl );
	//--------------------------------------------------------------------------------------------------------
	GButton ( CircuidView *parent, CMovieButton *btn );
public:
	//--------------------------------------------------------------------------------------------------------
	bool getValue (){
		return  ( cView->getValue() == 0.0f ) ? false : true;
	}
	//--------------------------------------------------------------------------------------------------------
	virtual ~GButton();
	//--------------------------------------------------------------------------------------------------------
	// erzeugt VSTPlugin Edit-Button.
	static GButton::Ptr createNewEditButton ( CircuidView *view );
};
//============================================================================================================
//	Klasse PlaceGObject:
//  Ein Place Objekt implementiert den Vorgang ein Objekt auf der View zu plazieren.
//  Es wird mit einem GObject Initalisiert und der View hinzugefuegt.
//  Dann folgt es solange der Maus bis ein linksklick erfolgt ist. Jetzt wird das enthaltene
//  GObject an Ort und stelle der view hinzugefuegt und das PlaceGObject entfernt sich aus der view.
//============================================================================================================
class PlaceGObject : public GObject, public EventListener<OnIdle> {
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<PlaceGObject> Ptr;
	//--------------------------------------------------------------------------------------------------------
	typedef list<GObject::Ptr> GObjList;
private:
	//--------------------------------------------------------------------------------------------------------
	GObjList gObjList;
	//--------------------------------------------------------------------------------------------------------
	void initListener();
protected:
	//--------------------------------------------------------------------------------------------------------
	PlaceGObject ( CircuidView *parent, const GObject::Ptr &content );
	//--------------------------------------------------------------------------------------------------------
	PlaceGObject ( CircuidView *parent, const GObjList &content );
public:
	//--------------------------------------------------------------------------------------------------------
	static PlaceGObject::Ptr create( CircuidView *parent, const GObject::Ptr &content ) {
		PlaceGObject::Ptr neu( new PlaceGObject ( parent, content ) );
		neu->_setSelfPtr ( neu );
		neu->initListener();
		return neu;
	}
	//--------------------------------------------------------------------------------------------------------
	static PlaceGObject::Ptr create( CircuidView *parent, const GObjList &content ) {
		PlaceGObject::Ptr neu( new PlaceGObject ( parent, content ) );
		neu->_setSelfPtr ( neu );
		neu->initListener();
		return neu;
	}
	//--------------------------------------------------------------------------------------------------------
	const GObjList & getContent() const { return gObjList; }
	//--------------------------------------------------------------------------------------------------------
	~PlaceGObject();
	//--------------------------------------------------------------------------------------------------------
	virtual void onMouse ( CDrawContext *cc, CPoint &p, long btn );
	//--------------------------------------------------------------------------------------------------------
	virtual void draw( CDrawContext *cc );
	//--------------------------------------------------------------------------------------------------------
	virtual void eventHandler ( void *src, const OnIdle &ev );
	//--------------------------------------------------------------------------------------------------------
	virtual void moveTo ( CPoint &p );
	//--------------------------------------------------------------------------------------------------------
	virtual void offset ( const CCoord &x, const CCoord &y );
};
} //namespace ppiGui
#endif


