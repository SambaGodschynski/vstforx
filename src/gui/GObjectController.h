#ifndef GOBJECT_CONTROLLER
#define GOBJECT_CONTROLLER

#include "CircuidView.h"
#include "MouseAction.h"
#include "ViewEvents.h"
#include "processing/processing.h"
#include "CMenu.h"
#include <map>
#include <boost/bimap.hpp> 
#include "gui/VstPlugView.h"
#include "processing/MidiEventProcessor.h"


namespace ppiGui{
using namespace std;
//============================================================================================================
// class ViewRelations:
// abbildung von GObject auf GraphObjekt und umgekehrt. ( view 1<=>1 model )
//============================================================================================================
class ViewRelations {
friend class boost::serialization::access;
BOOST_SERIALIZATION_SPLIT_MEMBER()
public:
	//--------------------------------------------------------------------------------------------------------
	typedef PObject::Ptr V;
	//--------------------------------------------------------------------------------------------------------
	typedef GObject::Ptr U;
	//--------------------------------------------------------------------------------------------------------	
	typedef map< U, V > V2M; 
private:
	//--------------------------------------------------------------------------------------------------------	
	V2M v2m;
	//--------------------------------------------------------------------------------------------------------
	list <V> tmpPObjects; // gefuellt wenn view geschlossen
	//--------------------------------------------------------------------------------------------------------
	void load ( iArchive &ar, const unsigned int version  );
	//--------------------------------------------------------------------------------------------------------
	void save ( oArchive &ar, const unsigned int version  ) const;
public:
	//--------------------------------------------------------------------------------------------------------
	const V2M & getV2M(){ return v2m; }
	//--------------------------------------------------------------------------------------------------------
	ViewRelations() {}
	//--------------------------------------------------------------------------------------------------------
	virtual ~ViewRelations(){}
	//--------------------------------------------------------------------------------------------------------
	void registerRelation ( GObject::Ptr vObj, PObject::Ptr mObj );
	//--------------------------------------------------------------------------------------------------------
	void unregisterRelation ( GObject::Ptr vObj, PObject::Ptr mObj );
	//--------------------------------------------------------------------------------------------------------
	template < typename T >
	typename T::Ptr get ( GObject::Ptr obj ){
		V2M::iterator it = v2m.find(obj);
		if ( it == v2m.end() ) throw ppiError::MapError ("map error", __FILE__, __LINE__ );
		if ( !(*it).second ) return typename T::Ptr();
		typename T::Ptr ret = boost::shared_dynamic_cast<T, PObject> ( (*it).second );
		return ret;
	}
	//--------------------------------------------------------------------------------------------------------
	// speichert alle GObjects und transferiert PObjects nach tmpPObjects
	void saveViewRelation ( oArchive &ar );
	//--------------------------------------------------------------------------------------------------------
	// deserialisiert alle Viewbezogenen Objekte.
	// setzt vorraus das die ViewRelation Datenstruktur (map) seit der 
	// View Objekt deserilisation nicht geandert wurde. 
	void loadViewRelation ( iArchive &ar );
	//--------------------------------------------------------------------------------------------------------
	void clear() { v2m.clear(); }
};
//============================================================================================================
// class IObjectController:
// Interface fuer Kontroller.
//============================================================================================================
class IObjectController {
protected:
	//--------------------------------------------------------------------------------------------------------
	FrontController &frntCtrl;
public:
	//--------------------------------------------------------------------------------------------------------
	typedef list<IObjectController*> Container;
	//--------------------------------------------------------------------------------------------------------
	IObjectController ( FrontController &frntCtrl ) : frntCtrl( frntCtrl ) {}
	//--------------------------------------------------------------------------------------------------------
	virtual ~IObjectController() {}
	//--------------------------------------------------------------------------------------------------------
	virtual void registerObject ( GObject::Ptr vObj, PObject::Ptr mObject ) = 0;
	//--------------------------------------------------------------------------------------------------------
	virtual void unregisterObject ( GObject::Ptr vObj, PObject::Ptr mObject ) = 0;
	//--------------------------------------------------------------------------------------------------------
	virtual void getMenuEntryList ( GObject::Ptr obj, menu::MenuEntryList &mL ) = 0;
	//--------------------------------------------------------------------------------------------------------
	virtual void clear(){};
};
//============================================================================================================
// class GObjectController:
// Kontroller fuer GObject objekte.
//============================================================================================================
class GObjectController : 
	public IObjectController,
	public EventListener<OnMouseClick>,
	public EventListener<OnMouseDrag>,
	public EventListener<OnRemove>
{
private:
	//--------------------------------------------------------------------------------------------------------
	MouseAction *mAction;
protected:
	//--------------------------------------------------------------------------------------------------------
	void removeConnectedComponents ( GObject::Ptr gObj );
public:
	//--------------------------------------------------------------------------------------------------------
	typedef list<GObjectController*> Container;
	//--------------------------------------------------------------------------------------------------------
	GObjectController ( FrontController &frntCtrl ) : 
	IObjectController( frntCtrl ), mAction(NULL) {}
	//--------------------------------------------------------------------------------------------------------
	virtual ~GObjectController() {}
	//--------------------------------------------------------------------------------------------------------
	virtual void registerObject ( GObject::Ptr vObj, PObject::Ptr mObject );
	//--------------------------------------------------------------------------------------------------------
	virtual void unregisterObject ( GObject::Ptr vObj, PObject::Ptr mObject ){}
	//--------------------------------------------------------------------------------------------------------
	virtual void getMenuEntryList ( GObject::Ptr obj, menu::MenuEntryList &mL ) {}
	//--------------------------------------------------------------------------------------------------------
	void eventHandler ( void *src, const OnMouseClick &ev );
	//--------------------------------------------------------------------------------------------------------
	void eventHandler ( void *src, const OnMouseDrag &ev );
	//--------------------------------------------------------------------------------------------------------
	void eventHandler ( void *src, const OnRemove &ev );
	//--------------------------------------------------------------------------------------------------------
	void setMouseAction ( MouseAction *mA ){ mAction = mA; }
	//--------------------------------------------------------------------------------------------------------
	MouseAction * getMouseAction() { return mAction; }
	//------------------------------------------------------------------------------------------------------------
	void findConnectionsOnObject ( GObject::Ptr obj, list<GConnection::Ptr> &l );
	//--------------------------------------------------------------------------------------------------------
	// liefert GObjecte die bestandteil von GObject sind.
	void getConnectedComponents ( GObject::Ptr gObj, list<GObject::Ptr> &retL );
};

//============================================================================================================
// class FrontController:
// Vermittelt zwischen Konkreten GObject-Kontroll Objekten.
//============================================================================================================
class FrontController {
public:
	//--------------------------------------------------------------------------------------------------------
	// Controller :
	enum ControllerId { CTRL_GOBJECT,
				        CTRL_GKNOB, 
						CTRL_GIONODE, 
						CTRL_GPROCESSOR, 
						CTRL_GKNOB_CONNECTION,
						CTRL_IO_CONNECTION,
						CTRL_GSWITCH_NODE,
						CTRL_GVST_PLUGIN,
						NUM_CTRL };
private:
	//--------------------------------------------------------------------------------------------------------
	typedef PObject::Ptr M; typedef GObject::Ptr N;
	//--------------------------------------------------------------------------------------------------------	
	typedef multimap<M, N> M2V; // abbildung: PObject 1<=>* GObject
	//--------------------------------------------------------------------------------------------------------	
	M2V model2view;
	//--------------------------------------------------------------------------------------------------------
	ViewRelations &view2model; // extern: ist in editor defininert
	//--------------------------------------------------------------------------------------------------------
	// liefert Hashwert aus Type mittels typeid.name
	template < typename T >
	long hash(){
		const char *type_name = typeid(T).name();
		return MyString(type_name).hash();
	}
	//--------------------------------------------------------------------------------------------------------
	// liefert Hashwert aus Type mittels typeid.name
	long hash( GObject::Ptr obj ){
		const char *type_name = typeid( *( obj.get() ) ).name();
		return MyString(type_name).hash();
	}
	//--------------------------------------------------------------------------------------------------------
	IObjectController *controller[NUM_CTRL];
	//--------------------------------------------------------------------------------------------------------
	typedef long U; typedef IObjectController* V;
	//--------------------------------------------------------------------------------------------------------
	typedef map<U, V> ControllerMap;
	//--------------------------------------------------------------------------------------------------------
	// bildet Hashwert - der aus typeid.name ermittelt wird - auf kontroller ab.
	ControllerMap controllerMap;
public:
	//--------------------------------------------------------------------------------------------------------
	IObjectController * getController ( ControllerId id ){ return controller[id]; }
	//--------------------------------------------------------------------------------------------------------
	void getMenuEntryList ( menu::MenuEntryList &mE, GObject::Ptr obj ){ 
		IObjectController *ctrl = getController(obj);
		if (!ctrl) return;
		ctrl->getMenuEntryList(obj, mE); 
	}
	//--------------------------------------------------------------------------------------------------------
	void setMouseAction ( MouseAction *mA ){ 
		( (GObjectController*) controller[CTRL_GOBJECT]) -> setMouseAction (mA);  
	}
	//--------------------------------------------------------------------------------------------------------
	MouseAction * getMouseAction(){ 
		return
			( (GObjectController*) controller[CTRL_GOBJECT]) -> getMouseAction ();  
	}
	//--------------------------------------------------------------------------------------------------------
	FrontController( ViewRelations &viewRelations );
	//--------------------------------------------------------------------------------------------------------
	virtual ~FrontController();
	//--------------------------------------------------------------------------------------------------------
	ViewRelations & getViewRelations(){ return view2model; }
	//--------------------------------------------------------------------------------------------------------
	void registerObject ( GObject::Ptr vObj, PObject::Ptr mObj = PObject::Ptr() );
	//--------------------------------------------------------------------------------------------------------
	void unregisterObject ( GObject::Ptr gObj );
	//--------------------------------------------------------------------------------------------------------
	IObjectController * getController ( GObject::Ptr obj );
	//--------------------------------------------------------------------------------------------------------
	// registriert alle ViewRelations Objekte neu.	
	void reRegisterObjects();
	//--------------------------------------------------------------------------------------------------------
	void registerModelRelation ( GObject::Ptr gObj, PObject::Ptr pObj ){
		model2view.insert ( pair< M, N >( pObj, gObj ) );
	}
	//--------------------------------------------------------------------------------------------------------
	const M2V & getModelRelation () const { return model2view; }
	//--------------------------------------------------------------------------------------------------------
	template < typename T >
	void getGObjectList ( PObject::Ptr obj, list <typename T::Ptr> &l ) {
		pair< M2V::iterator, M2V::iterator > ret;
		ret = model2view.equal_range ( obj );
		if ( ret.first == model2view.end() ) return;
		for ( M2V::iterator it = ret.first; it!= ret.second; ++it ) {
			typename T::Ptr t = boost::shared_dynamic_cast<T, GObject> ( it->second );
			if (!t) continue;
			l.push_back ( t );
		}
	}
	//--------------------------------------------------------------------------------------------------------
	virtual void clear();
};
//============================================================================================================
// class GKnobController:
//============================================================================================================
class GKnobController : 
	public IObjectController, 
	public EventListener<OnConnect>, 
	public EventListener< OnDestroy<GObject> >
{
private:
	//--------------------------------------------------------------------------------------------------------
	bool isPassiveKnob( GKnob *knb ) { return dynamic_cast<GPassiveKnob*> (knb); }
public:
	//--------------------------------------------------------------------------------------------------------
	GKnobController ( FrontController &frntCtrl ) : IObjectController(frntCtrl) {}
	//--------------------------------------------------------------------------------------------------------
	void knobValueChanged ( void *src, const float &value );
	//--------------------------------------------------------------------------------------------------------
	void parameterValueChanged ( void *src, const float &value );
	//--------------------------------------------------------------------------------------------------------
	virtual void registerObject ( GObject::Ptr vObj, PObject::Ptr mObj );
	//--------------------------------------------------------------------------------------------------------
	virtual void unregisterObject ( GObject::Ptr vObj, PObject::Ptr mObject );
	//--------------------------------------------------------------------------------------------------------
	virtual void getMenuEntryList ( GObject::Ptr obj, menu::MenuEntryList &mL );
	//--------------------------------------------------------------------------------------------------------
	virtual void eventHandler( void *src, const OnDestroy<GObject> &ev );
	//--------------------------------------------------------------------------------------------------------
	void eventHandler ( void *src, const OnConnect &ev );
};
//============================================================================================================
// class GIONodeController:
//============================================================================================================
class GIONodeController : 
	public IObjectController, 
	public EventListener<OnConnect>
{
public:
	//--------------------------------------------------------------------------------------------------------
	GIONodeController ( FrontController &frntCtrl ) : IObjectController(frntCtrl) {}
	//--------------------------------------------------------------------------------------------------------
	virtual void registerObject ( GObject::Ptr vObj, PObject::Ptr mObj );
	//--------------------------------------------------------------------------------------------------------
	virtual void unregisterObject ( GObject::Ptr vObj, PObject::Ptr mObject );
	//--------------------------------------------------------------------------------------------------------
	virtual void getMenuEntryList ( GObject::Ptr obj, menu::MenuEntryList &mL ){}
	//--------------------------------------------------------------------------------------------------------
	void eventHandler ( void *src, const OnConnect &ev );
};
//============================================================================================================
// class GProcessorNodeController:
//============================================================================================================
class GProcessorNodeController : public IObjectController {
private:
public:
	//--------------------------------------------------------------------------------------------------------
	GProcessorNodeController ( FrontController &frntCtrl ) : IObjectController(frntCtrl) {}
	//--------------------------------------------------------------------------------------------------------
	virtual void registerObject ( GObject::Ptr vObj, PObject::Ptr mObj ){}
	//--------------------------------------------------------------------------------------------------------
	void addParameter( GObject::Ptr obj, menu::MenuEntryList &mL, PObject::Ptr mObj );
	//--------------------------------------------------------------------------------------------------------
	void addNode_Entry( GObject::Ptr obj, menu::MenuEntryList &mL, PObject::Ptr mObj );
	//--------------------------------------------------------------------------------------------------------
	virtual void unregisterObject ( GObject::Ptr vObj, PObject::Ptr mObject );
	//--------------------------------------------------------------------------------------------------------
	void getMidiMenu ( MidiEventProcessor::Ptr evP, GObject::Ptr obj, menu::MenuEntryList &mL );
	//--------------------------------------------------------------------------------------------------------
	virtual void getMenuEntryList ( GObject::Ptr obj, menu::MenuEntryList &mL ){ 
		createMenu ( obj, mL ); 
	}
	//--------------------------------------------------------------------------------------------------------
	void createMenu ( GObject::Ptr obj, 
					  menu::MenuEntryList &mL );
};
//============================================================================================================
// class GKnobConnectionController:
//============================================================================================================
class GKnobConnectionController : public IObjectController 
{
private:
public:
	//--------------------------------------------------------------------------------------------------------
	GKnobConnectionController ( FrontController &frntCtrl ) : 
	  IObjectController(frntCtrl) {}
	//--------------------------------------------------------------------------------------------------------
	virtual void registerObject ( GObject::Ptr vObj, PObject::Ptr mObj );
	//--------------------------------------------------------------------------------------------------------
	virtual void unregisterObject ( GObject::Ptr vObj, PObject::Ptr mObject );
	//--------------------------------------------------------------------------------------------------------
	virtual void getMenuEntryList ( GObject::Ptr obj, menu::MenuEntryList &mL );
};
//============================================================================================================
// class GIOConnectionController:
//============================================================================================================
class GIOConnectionController : public IObjectController {
private:
public:
	//--------------------------------------------------------------------------------------------------------
	GIOConnectionController ( FrontController &frntCtrl ) : 
	  IObjectController(frntCtrl) {}
	//--------------------------------------------------------------------------------------------------------
	virtual void registerObject ( GObject::Ptr vObj, PObject::Ptr mObj );
	//--------------------------------------------------------------------------------------------------------
	virtual void unregisterObject ( GObject::Ptr vObj, PObject::Ptr mObject );
	//--------------------------------------------------------------------------------------------------------
	virtual void getMenuEntryList ( GObject::Ptr obj, menu::MenuEntryList &mL );
};
//============================================================================================================
// class GSwitchNodeController:
//============================================================================================================
class GSwitchNodeController : 
	public IObjectController, 
	public EventListener<OnIdle>,
	public EventListener< OnDestroy<GObject> >
{
public:
	//--------------------------------------------------------------------------------------------------------
	typedef list<IHasState::Ptr> StateNodes;
private:
	//--------------------------------------------------------------------------------------------------------
	StateNodes stateNodes;
public:
	//--------------------------------------------------------------------------------------------------------
	virtual void eventHandler ( void *src, const OnDestroy<GObject> &ev );
	//--------------------------------------------------------------------------------------------------------
	virtual void eventHandler ( void *src, const OnIdle &ev );
	//--------------------------------------------------------------------------------------------------------
	GSwitchNodeController ( FrontController &frntCtrl ) : IObjectController(frntCtrl) {}
	//--------------------------------------------------------------------------------------------------------
	virtual void registerObject ( GObject::Ptr vObj, PObject::Ptr mObj );
	//--------------------------------------------------------------------------------------------------------
	virtual void unregisterObject ( GObject::Ptr vObj, PObject::Ptr pObj );
	//--------------------------------------------------------------------------------------------------------
	virtual void getMenuEntryList ( GObject::Ptr obj, menu::MenuEntryList &mL );
};
//============================================================================================================
// class GPluginController:
//============================================================================================================
class GPluginController : 
	public IObjectController,
	public EventListener< ButtonClicked >,
	public EventListener< OnDestroy<GObject> >,
	public EventListener< OnClose >,
	public EventListener< OnMoving >,
	public EventListener<EditorPositionEvent>,
	public EventListener<EditorOpenParameterChanged>
{
private:
	//--------------------------------------------------------------------------------------------------------
	typedef GVSTPlugNode::Ptr U; 
	//--------------------------------------------------------------------------------------------------------
	typedef VSTPlugView::Ptr V;
	//--------------------------------------------------------------------------------------------------------
	typedef boost::bimap<U, V> VSTPlugViewMap;
	//--------------------------------------------------------------------------------------------------------
	typedef list<processing::PlugNode::Ptr> PlugNodeList;
	//--------------------------------------------------------------------------------------------------------
	PlugNodeList keepOpenState; // keeps openParameter on 1.0f when closing forced
	//--------------------------------------------------------------------------------------------------------
	VSTPlugViewMap vstPlugViewMap;
	//--------------------------------------------------------------------------------------------------------
	void openEdWindow ( const GVSTPlugNode::Ptr &node ); 
	//--------------------------------------------------------------------------------------------------------
	void closeEdWindow ( const GVSTPlugNode::Ptr &node ); 
	//--------------------------------------------------------------------------------------------------------
	void getPluginMenu (PlugNode::Ptr plug, GObject::Ptr obj, menu::MenuEntryList &mL );
public:
	//--------------------------------------------------------------------------------------------------------
	void eventHandler ( void *src, const OnMoving &ev );
	//--------------------------------------------------------------------------------------------------------
	GPluginController ( FrontController &frntCtrl ) : IObjectController(frntCtrl) {}
	//--------------------------------------------------------------------------------------------------------
	virtual void registerObject ( GObject::Ptr vObj, PObject::Ptr mObj );
	//--------------------------------------------------------------------------------------------------------
	virtual void unregisterObject ( GObject::Ptr vObj, PObject::Ptr mObject );
	//--------------------------------------------------------------------------------------------------------
	void eventHandler ( void *src, const ButtonClicked &ev );
	//--------------------------------------------------------------------------------------------------------
	void eventHandler ( void *src, const OnDestroy<GObject> &ev );
	//--------------------------------------------------------------------------------------------------------
	void eventHandler ( void *src, const OnClose &ev );
	//--------------------------------------------------------------------------------------------------------
	void eventHandler ( void *src, const EditorPositionEvent &ev );
	//--------------------------------------------------------------------------------------------------------
	void eventHandler ( void *src, const EditorOpenParameterChanged &ev );
	//--------------------------------------------------------------------------------------------------------
	virtual void getMenuEntryList ( GObject::Ptr obj, menu::MenuEntryList &mL );
	//--------------------------------------------------------------------------------------------------------
	virtual void clear() {
		vstPlugViewMap.clear();
	}
	//--------------------------------------------------------------------------------------------------------
	virtual ~GPluginController();
};
} // namespace ppiGui

#endif


