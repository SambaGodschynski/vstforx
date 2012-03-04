/*
 * ===========================================================================================================
 * GObjectController.h
 *      Author: Johannes Unger
 * ===========================================================================================================
 */

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
/**
 * @class ViewRelations.
 * Abbildung von GObject-Objekte auf PObject-Objekte und umgekehrt.
 */
class ViewRelations {
//============================================================================================================
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
	/**
	 * Deserialisiert ViewRelations-Objekt
	 * @param ar boost::Archive-Objekt
	 * @param version
	 */
	void load ( iArchive &ar, const unsigned int version  );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Serialisiert ViewRelations-Objekt
	 * @param ar
	 * @param version
	 */
	void save ( oArchive &ar, const unsigned int version  ) const;
public:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return  View2Model Abbildungsmap
	 */
	const V2M & getV2M(){ return v2m; }
	//--------------------------------------------------------------------------------------------------------
	ViewRelations() {}
	//--------------------------------------------------------------------------------------------------------
	virtual ~ViewRelations(){}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * nimmt GObject<->PObject beziehung in View2Model Abbildungsmap auf.
	 * @param vObj
	 * @param mObj
	 */
	void registerRelation ( GObject::Ptr vObj, PObject::Ptr mObj );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * entfernt GObject<->PObject beziehung aus View2Model Abbildungsmap.
	 * @param vObj
	 * @param mObj
	 */
	void unregisterRelation ( GObject::Ptr vObj, PObject::Ptr mObj );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param obj
	 * @return PObject-Objekt zu passenden GObject-Objekt. Castet zu konkreten PObject-Typ T.
	 */
	template < typename T >
	typename T::Ptr get ( GObject::Ptr obj ){
		V2M::iterator it = v2m.find(obj);
		if ( it == v2m.end() ) throw ppiError::MapError ("map error", __FILE__, __LINE__ );
		if ( !(*it).second ) return typename T::Ptr();
		typename T::Ptr ret = boost::shared_dynamic_cast<T, PObject> ( (*it).second );
		return ret;
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Serialisiert GObject-Seite der PObject<->GObject Abbildung in Archive.
	 * Wird benoetigt nach Editor:close, Editor:open Vorgang.
	 * GObject-Objekte werden gesichert bei Wiederherstellung, urspruenglichen PObject-Objekten zugeordnet.
	 * @param ar boost::Archive-Objekt
	 */
	void saveViewRelation ( oArchive &ar );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Deserialisiert GObjects aus Archive und fuegt sie wieder (an urspruenglicher Position) in Abbildung ein.
	 * Setzt vorraus, dass die ViewRelation Datenstruktur, seit saveViewRelation() nicht geaendert wurde.
	 * @param ar
	 */
	void loadViewRelation ( iArchive &ar );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * leert Abblidung.
	 */
	void clear() { v2m.clear(); }
};
//============================================================================================================
/**
 * @class ObjectController:
 * Oberklasse fuer Kontroller.
 */
class ObjectController {
//============================================================================================================
protected:
	//--------------------------------------------------------------------------------------------------------
	FrontController &frntCtrl;
public:
	//--------------------------------------------------------------------------------------------------------
	typedef list<ObjectController*> Container;
	//--------------------------------------------------------------------------------------------------------
	ObjectController ( FrontController &frntCtrl ) : frntCtrl( frntCtrl ) {}
	//--------------------------------------------------------------------------------------------------------
	virtual ~ObjectController() {}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Event: Objekt wird im Controller registriert.
	 * @param vObj neues GObject
	 * @param mObject zugehoeriges PObject
	 */
	virtual void registerObject ( GObject::Ptr vObj, PObject::Ptr mObject ) = 0;
	//--------------------------------------------------------------------------------------------------------
	/**
	 *  Event: Objekt wird aus Controller entfernt.
	 * @param vObj neues GObject
	 * @param mObject zugehoeriges PObject
	 */
	virtual void unregisterObject ( GObject::Ptr vObj, PObject::Ptr mObject ) = 0;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * liefert Menüeintraege zu GObject.
	 * @param obj entsprechendes GObject
	 * @param mL Ziel-MenuEntryList
	 */
	virtual void getMenuEntryList ( GObject::Ptr obj, menu::MenuEntryList &outMl ) = 0;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Event: alle Objeke werden aus Controller entfernt
	 */
	virtual void clear(){};
};
//============================================================================================================
/**
 * @class GObjectController.
 * Kontroller fuer alle GObject-Objekte.
 */
class GObjectController : 
//============================================================================================================
	public ObjectController,
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
		ObjectController( frntCtrl ), mAction(NULL) {}
	//--------------------------------------------------------------------------------------------------------
	virtual ~GObjectController() {}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Event: Objekt wird im Controller registriert.
	 * @param vObj neues GObject
	 * @param mObject zugehoeriges PObject
	 */

	virtual void registerObject ( GObject::Ptr vObj, PObject::Ptr mObject );
	//--------------------------------------------------------------------------------------------------------
	/**
	 *  Event: Objekt wird aus Controller entfernt.
	 * @param vObj neues GObject
	 * @param mObject zugehoeriges PObject
	 */
	virtual void unregisterObject ( GObject::Ptr vObj, PObject::Ptr mObject ){}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * liefert Menüeintraege zu GObject.
	 * @param obj entsprechendes GObject
	 * @param outMl Ziel-MenuEntryList
	 */
	virtual void getMenuEntryList ( GObject::Ptr obj, menu::MenuEntryList &outMl ) {}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * OnMouseClick-Event
	 * @param src Eventquelle
	 * @param ev Eventobjekt
	 */
	void eventHandler ( void *src, const OnMouseClick &ev );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * OnMouseDrag-Event
	 * @param src Eventquelle
	 * @param ev Eventobjekt
	 */
	void eventHandler ( void *src, const OnMouseDrag &ev );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Event: Wird von Buehne entfernt
	 * @param src src Eventquelle
	 * @param ev Eventobjekt
	 */
	void eventHandler ( void *src, const OnRemove &ev );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * setzt aktuelles MouseAction-Objekt.
	 * @param mA MouseAction-Objekt (use/connect/drag).
	 */
	void setMouseAction ( MouseAction *mA ){ mAction = mA; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return aktuelles MausAction-Objekt.
	 */
	MouseAction * getMouseAction() { return mAction; }
	//------------------------------------------------------------------------------------------------------------
	/**
	 * liefert alle GConnection-Objekt zu GObject-Objekt.
	 * @param obj
	 * @param l
	 */
	void findConnectionsOnObject ( GObject::Ptr obj, list<GConnection::Ptr> &l );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * liefert GObject-Objekte die fest an GObject gebunden sind.
	 * (zB.: ProcessorNode Ein/Ausgaenge)
	 * @param gObj
	 * @param retL
	 */
	void getConnectedComponents ( GObject::Ptr gObj, list<GObject::Ptr> &retL );
};

//============================================================================================================
/**
 * @class FrontController.
 * Deligiert Kontrolleranfragen an Konkreten Kontroller.
 */
class FrontController {
//============================================================================================================
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
	/**
	 * TODO: zu Viewrelations verschieben.
	 * Abbildung: PObject 1<=>* GObject
	 */
	typedef multimap<M, N> M2V;
	//--------------------------------------------------------------------------------------------------------	
	M2V model2view;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * extern: ist in Editor enthalten.
	 */
	ViewRelations &view2model;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * liefert Hashwert zu Type.
	 */
	template < typename T >
	long hash(){
		const char *type_name = typeid(T).name();
		return MyString(type_name).hash();
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * liefert Hashwert zu GObject-Objekt.
	 * @param obj
	 */
	long hash( GObject::Ptr obj ){
		const char *type_name = typeid( *( obj.get() ) ).name();
		return MyString(type_name).hash();
	}
	//--------------------------------------------------------------------------------------------------------
	ObjectController *controller[NUM_CTRL];
	//--------------------------------------------------------------------------------------------------------
	typedef long U; typedef ObjectController* V;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Abbildung: Objekttype -> Kontroller
	 */
	typedef map<U, V> ControllerMap;
	//--------------------------------------------------------------------------------------------------------
	ControllerMap controllerMap;
public:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param id
	 * @return Kontroller zu ControllerId
	 */
	ObjectController * getController ( ControllerId id ){ return controller[id]; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * liefert Menüeintraege zu GObject.
	 * @param outMl Ziel-MenuEntryList
	 * @param obj entsprechendes GObject
	 * @throw com::ppiError::NullPointer
	 */
	void getMenuEntryList ( menu::MenuEntryList &outMl, GObject::Ptr obj ){
		ObjectController *ctrl = getController(obj);
		if (!ctrl) return;
		ctrl->getMenuEntryList(obj, outMl); 
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * setzt akuelles MouseAction-Objekt
	 * @param mA
	 */
	void setMouseAction ( MouseAction *mA ){ 
		( (GObjectController*) controller[CTRL_GOBJECT]) -> setMouseAction (mA);  
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return akuelles MouseAction-Objekt
	 */
	MouseAction * getMouseAction(){ 
		return
			( (GObjectController*) controller[CTRL_GOBJECT]) -> getMouseAction ();  
	}
	//--------------------------------------------------------------------------------------------------------
	FrontController( ViewRelations &viewRelations );
	//--------------------------------------------------------------------------------------------------------
	virtual ~FrontController();
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return ViewRelations (GObject<->POBject Abbildung)
	 */
	ViewRelations & getViewRelations(){ return view2model; }
	//--------------------------------------------------------------------------------------------------------
	void registerObject ( GObject::Ptr vObj, PObject::Ptr mObj = PObject::Ptr() );
	//--------------------------------------------------------------------------------------------------------
	void unregisterObject ( GObject::Ptr gObj );
	//--------------------------------------------------------------------------------------------------------
	ObjectController * getController ( GObject::Ptr obj );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * GObject-Objekte werden nach
	 * Editor:close, Editor:open neu registriert.
	 */
	void reRegisterObjects();
	//--------------------------------------------------------------------------------------------------------
	/**
	 * hinzufuegen einer PObject(Model)->GObject(View) beziehung.
	 * TODO: verschieben nach Viewrelationsklasse.
	 * @param gObj
	 * @param pObj
	 */
	void registerModelRelation ( GObject::Ptr gObj, PObject::Ptr pObj ){
		model2view.insert ( pair< M, N >( pObj, gObj ) );
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return PObject(Model)->GObject(View) Abbildungs-Container
	 */
	const M2V & getModelRelation () const { return model2view; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * liefert alle zu PObject registrierten GObject-Objekte.
	 * Castet nach konkreten Typ<T>.
	 * @param obj
	 * @param outList Ziel-Objektliste<T>
	 * TODO: Generischer Containertyp: template<Container, T> oder besser (STL: Container::value_type)
	 */
	template < typename T >
	void getGObjectList ( PObject::Ptr obj, list <typename T::Ptr> &outList ) {
		pair< M2V::iterator, M2V::iterator > ret;
		ret = model2view.equal_range ( obj );
		if ( ret.first == model2view.end() ) return;
		for ( M2V::iterator it = ret.first; it!= ret.second; ++it ) {
			typename T::Ptr t = boost::shared_dynamic_cast<T, GObject> ( it->second );
			if (!t) continue;
			outList.push_back ( t );
		}
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * entfernt alle registrierten Objekte
	 */
	virtual void clear();
};
//============================================================================================================
// class GKnobController:
//============================================================================================================
class GKnobController : 
	public ObjectController, 
	public EventListener<OnConnect>, 
	public EventListener< OnDestroy<GObject> >
{
private:
	//--------------------------------------------------------------------------------------------------------
	bool isPassiveKnob( GKnob *knb ) { return dynamic_cast<GPassiveKnob*> (knb); }
public:
	//--------------------------------------------------------------------------------------------------------
	GKnobController ( FrontController &frntCtrl ) : ObjectController(frntCtrl) {}
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
	public ObjectController, 
	public EventListener<OnConnect>
{
public:
	//--------------------------------------------------------------------------------------------------------
	GIONodeController ( FrontController &frntCtrl ) : ObjectController(frntCtrl) {}
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
class GProcessorNodeController : public ObjectController {
private:
public:
	//--------------------------------------------------------------------------------------------------------
	GProcessorNodeController ( FrontController &frntCtrl ) : ObjectController(frntCtrl) {}
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
class GKnobConnectionController : public ObjectController 
{
private:
public:
	//--------------------------------------------------------------------------------------------------------
	GKnobConnectionController ( FrontController &frntCtrl ) : 
	  ObjectController(frntCtrl) {}
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
class GIOConnectionController : public ObjectController {
private:
public:
	//--------------------------------------------------------------------------------------------------------
	GIOConnectionController ( FrontController &frntCtrl ) : 
	  ObjectController(frntCtrl) {}
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
	public ObjectController, 
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
	GSwitchNodeController ( FrontController &frntCtrl ) : ObjectController(frntCtrl) {}
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
	public ObjectController,
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
	typedef list<processing::Plugin::Ptr> PlugNodeList;
	//--------------------------------------------------------------------------------------------------------
	PlugNodeList keepOpenState; // keeps openParameter on 1.0f when closing forced
	//--------------------------------------------------------------------------------------------------------
	VSTPlugViewMap vstPlugViewMap;
	//--------------------------------------------------------------------------------------------------------
	void openEdWindow ( const GVSTPlugNode::Ptr &node ); 
	//--------------------------------------------------------------------------------------------------------
	void closeEdWindow ( const GVSTPlugNode::Ptr &node ); 
	//--------------------------------------------------------------------------------------------------------
	void getPluginMenu (Plugin::Ptr plug, GObject::Ptr obj, menu::MenuEntryList &mL );
public:
	//--------------------------------------------------------------------------------------------------------
	void eventHandler ( void *src, const OnMoving &ev );
	//--------------------------------------------------------------------------------------------------------
	GPluginController ( FrontController &frntCtrl ) : ObjectController(frntCtrl) {}
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


