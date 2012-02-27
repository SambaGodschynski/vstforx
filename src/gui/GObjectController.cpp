/*
 * ===========================================================================================================
 * GObjectController.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */

#include "GObjectController.h"
#include "ViewCommand.h"
#include "processing/ConcreteProcessAdapter.h"
#include "processing/parameter/ConnectionOperators.h"
#include "PpiEditor.h"
#include "com/SystemCommand.h"
#include <sstream>
#include <boost/static_assert.hpp>

namespace {
	//--------------------------------------------------------------------------------------------------------
	inline void hasParameterToMenuEntryList ( HasParameter::Ptr hP, 
										  ppiGui::menu::MenuEntryList &mL, 
										  ppiGui::GObject::Ptr src,
										  ppiGui::FrontController *ctrl ); 
	//--------------------------------------------------------------------------------------------------------
	/*
		menu entries koennen je nach tag alle zusammen aufgelistet werden oder aufgeteilt in untermenues.
		zb. 0..100 101..200 ...
		dazu muss ein Tag angegeben (siehe unten) und die sepzifizierte template methode
		dynMenuCallback<Tag> implementiert werden.
	*/
	template <typename Tag>
	inline void entriesToMenuList ( const typename Tag::TagType &object, 
								    ppiGui::menu::MenuEntryList &mL,
								    ppiGui::GObject::Ptr src,
								    ppiGui::FrontController *ctrl,
								    size_t num_entries );
	//------------------------------------------------------------------------------------------------------------
	struct PlugProgram_Tag {
		typedef Plugin::Ptr TagType;
		enum { MAX_ENTRIES = 100 };
		static string getPrefix() { return "program"; }
	};
	//------------------------------------------------------------------------------------------------------------
	struct HasParameter_Tag {
		typedef HasParameter::Ptr TagType;
		enum { MAX_ENTRIES = 100 };
		static string getPrefix() { return "parameter"; }
	};
} // namespace
namespace ppiGui{
using namespace menu;
//============================================================================================================
// class ViewRelations:
// abbildung von GObjects auf GraphObjekt und umgekehrt.
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void ViewRelations::registerRelation( GObject::Ptr vObj, PObject::Ptr mObj){
	assert (vObj);
	if ( !vObj ) return;
	pair < V2M::iterator, bool > ret = v2m.insert ( pair<U, V> ( vObj, mObj ) );
	assert ( ret.second );
}
//------------------------------------------------------------------------------------------------------------
void ViewRelations::unregisterRelation( GObject::Ptr vObj, PObject::Ptr mObj ){
	V2M::iterator it = v2m.find(vObj);
	assert ( it != v2m.end() );
	v2m.erase(it);
}
//------------------------------------------------------------------------------------------------------------
// speichert alle GObjects und transferiert PObjects nach tmpPObjects
void ViewRelations::saveViewRelation ( oArchive &ar ) {
	com::MethodMessage<ViewRelations> methodMessage("saveViewRelation()");
	V2M::const_iterator it = v2m.begin();
	list<U> l;
	tmpPObjects.clear();
	for ( ; it!=v2m.end(); ++it ){
		if ( dynamic_cast<Serializable*> ( (*it).first.get() ) ) {
			l.push_back ( (*it).first );
			tmpPObjects.push_back ( it->second );
		}
	}
	ar << l;
}
//------------------------------------------------------------------------------------------------------------
void ViewRelations::loadViewRelation ( iArchive &ar ) {
	com::MethodMessage<ViewRelations> methodMessage("loadViewRelation()");
	list<U> l;
	ar >> l;
	// tmpPObjects darf sich waehrend view closed nicht geandert haben.
	// TODO: Ausnahme werfen
	assert ( l.size() == tmpPObjects.size() ); 
	list<U>::iterator uit = l.begin();
	list<V>::iterator vit = tmpPObjects.begin();
	while ( uit!=l.end() ){
		v2m.insert ( pair<U, V> ( *uit, *vit ) );
		++vit;++uit;
	}
}
//------------------------------------------------------------------------------------------------------------
void ViewRelations::save(oArchive &ar, const unsigned int version) const {
	com::MethodMessage<ViewRelations> methodMessage("save()");
	ar<<tmpPObjects;
}
//------------------------------------------------------------------------------------------------------------
void ViewRelations::load(iArchive &ar, const unsigned int version) {
	com::MethodMessage<ViewRelations> methodMessage("load()");
	tmpPObjects.clear();
	ar>>tmpPObjects;
}	
//============================================================================================================
// class FrontController:
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
FrontController::FrontController ( ViewRelations &viewRelations ) :
	view2model ( viewRelations )
{
	// initalisiere Controller
	// GObject controller
	controller[CTRL_GOBJECT] = new GObjectController ( *this );
	// GKnob controller
	controller[CTRL_GKNOB] = new GKnobController( *this );
	controllerMap.insert ( pair<U, V> ( hash<GStdKnob>(), controller[CTRL_GKNOB] ) );
	controllerMap.insert ( pair<U, V> ( hash<GPassiveKnob>(), controller[CTRL_GKNOB] ) );
	// GIONode Controller
	controller[CTRL_GIONODE] = new GIONodeController( *this );
	controllerMap.insert ( pair<U, V> ( hash<GInputNode>(), controller[CTRL_GIONODE] ) );
	controllerMap.insert ( pair<U, V> ( hash<GOutputNode>(), controller[CTRL_GIONODE] ) );
	// GProcessorNodeController
	controller[CTRL_GPROCESSOR] = new GProcessorNodeController( *this );
	controllerMap.insert ( pair<U, V> ( hash<GVolumeNode>(), controller[CTRL_GPROCESSOR] ) );
	controllerMap.insert ( pair<U, V> ( hash<GPanAdapter>(), controller[CTRL_GPROCESSOR] ) );
	controllerMap.insert ( pair<U, V> ( hash<GPeakTracker>(), controller[CTRL_GPROCESSOR] ) );
	controllerMap.insert ( pair<U, V> ( hash<GADSRTrigger>(), controller[CTRL_GPROCESSOR] ) );
	controllerMap.insert ( pair<U, V> ( hash<GMidiProcessor>(), controller[CTRL_GPROCESSOR] ) );
	// GConnection
	controller[CTRL_GKNOB_CONNECTION] = new GKnobConnectionController( *this );
	controllerMap.insert ( pair<U, V> ( hash<GConnectionPaPa>(), controller[CTRL_GKNOB_CONNECTION] ) );
	controllerMap.insert ( pair<U, V> ( hash<GConnectionPrPa>(), controller[CTRL_GKNOB_CONNECTION] ) );
	controller[CTRL_IO_CONNECTION] = new GIOConnectionController( *this );
	controllerMap.insert ( pair<U, V> ( hash<GConnectionIO>(), controller[CTRL_IO_CONNECTION] ) );
	controllerMap.insert ( pair<U, V> ( hash<GConnectionPrIn>(), controller[CTRL_IO_CONNECTION] ) );
	controllerMap.insert ( pair<U, V> ( hash<GConnectionPrOut>(), controller[CTRL_IO_CONNECTION] ) );
	// GOutputStepNode
	controller[CTRL_GSWITCH_NODE] = new GSwitchNodeController( *this );
	controllerMap.insert ( pair<U, V> ( hash<GOutputStepNode>(), controller[CTRL_GSWITCH_NODE] ) );
	controllerMap.insert ( pair<U, V> ( hash<GInputStepNode>(), controller[CTRL_GSWITCH_NODE] ) );
	controllerMap.insert ( pair<U, V> ( hash<GOutputSwitch>(), controller[CTRL_GSWITCH_NODE] ) );
	controllerMap.insert ( pair<U, V> ( hash<GInputSwitch>(), controller[CTRL_GSWITCH_NODE] ) );
	// GVSTPlugNode
	controller[CTRL_GVST_PLUGIN] = new GPluginController( *this );
	controllerMap.insert ( pair<U, V> ( hash<GVSTPlugNode>(), controller[CTRL_GVST_PLUGIN] ) );
	// ...
		
}
//------------------------------------------------------------------------------------------------------------
FrontController::~FrontController (){
	for ( int i=0; i<NUM_CTRL; ++i ) delete controller[i]; 
}
//------------------------------------------------------------------------------------------------------------
void FrontController::registerObject( GObject::Ptr vObj, PObject::Ptr mObj) {
	view2model.registerRelation ( vObj, mObj );
	if (mObj) registerModelRelation ( vObj, mObj );
	controller[CTRL_GOBJECT]->registerObject ( vObj, mObj );
	ObjectController *ctrl = getController (vObj);
	if (!ctrl) return;
	ctrl->registerObject ( vObj, mObj );
}
//------------------------------------------------------------------------------------------------------------
void FrontController::unregisterObject( GObject::Ptr vObj ) {
	PObject::Ptr pObj = getViewRelations().get<PObject> (vObj);
	ObjectController *ctrl = getController (vObj);
	if (ctrl) ctrl->unregisterObject ( vObj, pObj );
	getViewRelations().unregisterRelation ( vObj, pObj );
	if (!pObj) return;
	pair < M2V::iterator, M2V::iterator > ret = model2view.equal_range ( pObj );
	for ( M2V::iterator it = ret.first; it!=ret.second; ++it ) {
		if ( it->second == vObj ) {
			model2view.erase (it);
			break;
		}
	}
}
//------------------------------------------------------------------------------------------------------------
ObjectController * FrontController::getController( GObject::Ptr obj ){
	ControllerMap::iterator it = controllerMap.find ( hash(obj) );
	if ( it == controllerMap.end() ) {
		LOG_ASSERT (0);
		return NULL;
	}
	return (*it).second;
}
//------------------------------------------------------------------------------------------------------------
void FrontController::clear() {
	for ( ControllerMap::iterator it = controllerMap.begin();
		  it!=controllerMap.end();
		  ++it
	) {
		(*it).second->clear();
	}
	view2model.clear();
	model2view.clear();
}
//------------------------------------------------------------------------------------------------------------
void FrontController::reRegisterObjects(){
	const ViewRelations::V2M &v2m = view2model.getV2M();
	ViewRelations::V2M::const_iterator it = v2m.begin();
	for ( ; it!=v2m.end(); ++it ){
		controller[CTRL_GOBJECT]->registerObject ( (*it).first, (*it).second );
		ObjectController *ctrl = getController ( it->first );
		registerModelRelation ( (*it).first, (*it).second );
		if (!ctrl) continue;
		ctrl->registerObject ( it->first, it->second );
	}
}
//============================================================================================================
// class GObjectController:
// Kontroller fuer GObject objekte.
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void GObjectController::eventHandler(void *src, const ppiGui::OnMouseClick &ev) {
	mAction->onMouse ( ev );
}
//------------------------------------------------------------------------------------------------------------
void GObjectController::eventHandler(void *src, const ppiGui::OnMouseDrag &ev) {
	mAction->onMouse ( ev );
}
//------------------------------------------------------------------------------------------------------------
void GObjectController::getConnectedComponents ( GObject::Ptr gObj, list<GObject::Ptr> &retL ) {
	list<GConnection::Ptr> l;
	findConnectionsOnObject ( gObj, l );
	list<GConnection::Ptr>::iterator it = l.begin();
	for ( ; it!=l.end(); ++it ) { 
		// connection dessen gegnstck. an processor gebunden ist
		if ( dynamic_cast<GConnectionPrPa*>  ( it->get() ) || 
			 dynamic_cast<GConnectionCoPa*>  ( it->get() ) ||  
			 dynamic_cast<GConnectionPrIn*>  ( it->get() ) ||
			 dynamic_cast<GConnectionPrOut*> ( it->get() )  
		) {
			// gegenuebeliegendes obj. finden :
			GObject::Ptr opp = ( gObj == (*it)->getObjectA() ) ? (*it)->getObjectB() : (*it)->getObjectA();
			retL.push_back ( opp );
		}
	}
}
//------------------------------------------------------------------------------------------------------------
void GObjectController::removeConnectedComponents ( GObject::Ptr gObj ) {
	GObjectList l;
	getConnectedComponents ( gObj, l );
	GObjectList::iterator it = l.begin();
	CircuidView *view = gObj->getParentView();
	for ( ; it!=l.end(); ++it ) {
		view->removeGObject ( *it );
	}
}
//------------------------------------------------------------------------------------------------------------
void GObjectController::eventHandler(void *src, const ppiGui::OnRemove &ev) {
	GObject::Ptr pt = ev.gObject->getPtr();
	assert ( pt );
	if ( dynamic_cast <GProcessorNode*>(ev.gObject) || dynamic_cast <GConnectionPaPa*>(ev.gObject) ) {
		// hier exitstieren evtl. weitere zugehoerige obj.
		removeConnectedComponents ( pt );
	}
	// connections entf.
	list<GConnection::Ptr> l;
	findConnectionsOnObject ( pt, l );
	list<GConnection::Ptr>::iterator it = l.begin();
	for ( ; it!=l.end(); ++it ) { // connections loeschen
		// Romanes Eunt Domus !
		ev.gObject->getParentView()->removeGObject(*it); //connection aus view entfernen
	}
	frntCtrl.unregisterObject ( pt ); /* entfernt object aus viewrelations und ruft										 
														  unregisterObject() im zugeordneten controller auf */
}
//------------------------------------------------------------------------------------------------------------
void GObjectController::registerObject( GObject::Ptr vObj, PObject::Ptr mObj) {
	vObj->EventSender<OnMouseClick>::addEventListener ( this );
	vObj->EventSender<OnMouseDrag>::addEventListener ( this );
	vObj->EventSender<OnRemove>::addEventListener ( this );
}
//------------------------------------------------------------------------------------------------------------
void GObjectController::findConnectionsOnObject ( GObject::Ptr obj, list<GConnection::Ptr> &cL ) {
	// :: Suche GConnection (nicht gerade die beste methode aber ausreichend) ::
	GObjectList l;
	// hole alle connections auf stage:
	obj->getParentView()->getGObjectsAtStage ( l, CircuidView::CONNECTIONS );
	GObjectList::iterator it = l.begin();
	for ( ; it!=l.end(); ++it ) {
		GConnection::Ptr gc = boost::shared_dynamic_cast <GConnection, GObject> ( *it );
		if (!gc) continue;
		if (  gc->getObjectA() != obj && gc->getObjectB() != obj  ) continue;
		cL.push_back ( gc );
	}
}
//============================================================================================================
// class GKnobController:
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void GKnobController::knobValueChanged ( void *src, const float &value ){
	GObject *obj = static_cast <GObject*>( src );
	GKnob::Ptr knob = boost::shared_dynamic_cast<GKnob, GObject>( obj->getPtr() );
	Parameter::Ptr p = frntCtrl.getViewRelations().get<Parameter> (knob);
	*p = value;
}
//------------------------------------------------------------------------------------------------------------
void GKnobController::parameterValueChanged ( void *src, const float &value ){
	PObject::Ptr pObj = static_cast<Parameter*>(src)->getPtr();
	Parameter::Ptr p = boost::shared_dynamic_cast<Parameter, PObject>(pObj);
	list <GKnob::Ptr> l;
	frntCtrl.getGObjectList<GKnob> ( p, l );
	list <GKnob::Ptr>::iterator it = l.begin();
	for ( ; it!=l.end(); ++it ) {
		GKnob *knob = (GKnob*)it->get();
		knob->setValue ( min(1.0f, value) );
		knob->setDisplay ( p->getDisplay() + p->getLabel() );
		knob->setObjectDirty();
	}
}
//------------------------------------------------------------------------------------------------------------
void GKnobController::registerObject ( GObject::Ptr vObj, PObject::Ptr mObj ){ 
	GKnob::Ptr knob = boost::shared_dynamic_cast<GKnob, GObject>(vObj);
	Parameter::Ptr p = boost::shared_dynamic_cast<Parameter, PObject>(mObj);
	// Knob Listener
	Parameter::ParameterListenerFunction kf=boost::bind(&GKnobController::knobValueChanged,this,_1,_2);
	knob->addValueChangedListenerF (kf);
	// Parameter Listener
	Parameter::ParameterListenerFunction pf=boost::bind(&GKnobController::parameterValueChanged,this,_1,_2);
	p->addValueChangedListenerF (pf);
	// OnConnect Event
	knob->EventSender<OnConnect>::addEventListener ( this );
	// OnDestroy Event
	knob->EventSender< OnDestroy<GObject> >::addEventListener ( this );

	knob->setValue ( *p );
}
//------------------------------------------------------------------------------------------------------------
void GKnobController::eventHandler( void *src, const OnDestroy<GObject> &ev ){
	GKnob::Ptr knob = boost::shared_dynamic_cast<GKnob, GObject>( ev.src->getPtr() );
	// entferne Parameter Listener Funktion
	Parameter::ParameterListenerFunction pf=boost::bind(&GKnobController::parameterValueChanged,this,_1,_2);
	Parameter::Ptr p = frntCtrl.getViewRelations().get<Parameter> ( knob );
	if (!p) return;
	p->removeValueChangedListenerF (pf);
	
}
//------------------------------------------------------------------------------------------------------------
void GKnobController::eventHandler ( void *_src, const OnConnect &ev ){
	GKnob::Ptr src = boost::shared_dynamic_cast<GKnob, GObject>( ev.src->getPtr() );
	GKnob::Ptr dst = boost::shared_dynamic_cast<GKnob, GObject>( ev.dst->getPtr() );
	if ( !dst ) return;
	CmdConnectGKnob connectGKnob( dst->getParentView(), src, dst, &frntCtrl );
	connectGKnob.execute();
}
//------------------------------------------------------------------------------------------------------------
void GKnobController::unregisterObject ( GObject::Ptr vObj, PObject::Ptr pObj ) {
	GKnob::Ptr knob = boost::shared_dynamic_cast<GKnob, GObject>( vObj );
	Parameter::Ptr p = boost::shared_dynamic_cast<Parameter, PObject>(pObj);
	if ( frntCtrl.getModelRelation().count (p) == 1 ) { // war das der letzte knob mit diesem parameter
		// value changed listener entfernen
		Parameter::ParameterListenerFunction pf=boost::bind(&GKnobController::parameterValueChanged,this,_1,_2);
		p->removeValueChangedListenerF (pf);
	}
	// onDestroy listener entfernen
	knob->EventSender< OnDestroy<GObject> >::removeEventListener (this);
	Graph::Ptr g = getRelatedGraph( vObj->getParentView() );
	Graph::Janitor::Ptr updater = g->getJanitor();
	updater->remove ( p );
	// Command Worker entfernt muell
	CommandWorker::getCommandWorker()->addCommand (
		Command::Ptr ( new CmdRelease<PObject>(p) )
	);
}
//------------------------------------------------------------------------------------------------------------
inline void GKnobController::getMenuEntryList ( GObject::Ptr obj, menu::MenuEntryList &mL ) {
	GKnob::Ptr knob = boost::shared_dynamic_cast<GKnob, GObject>(obj);
	if ( isPassiveKnob( knob.get() ) ) return;
	ADD_MENU_TITLE ( mL, "knob: " + knob->getHeader() );
	ADD_MENU_LABEL ( mL, "remove knob", new CmdRemoveGObject ( obj->getParentView(), knob ) );
}
//============================================================================================================
// class GIONodeController:
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void GIONodeController::registerObject ( GObject::Ptr vObj, PObject::Ptr mObj ){ 
	vObj->EventSender<OnConnect>::addEventListener ( this );
}
//------------------------------------------------------------------------------------------------------------
void GIONodeController::unregisterObject ( GObject::Ptr gObj, PObject::Ptr pObj ) {}
//------------------------------------------------------------------------------------------------------------
void GIONodeController::eventHandler ( void *_src, const OnConnect &ev ){ 
	GObject::Ptr src = ev.src->getPtr();
	GObject::Ptr dst = ev.dst->getPtr();
	CmdConnectGIONode cnct( ev.src->getParentView(), src, dst, &frntCtrl ); 
	cnct.execute();
}
//============================================================================================================
// class GProcessorNodeController:
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void GProcessorNodeController::createMenu ( GObject::Ptr obj, 
										    menu::MenuEntryList &mL
										    )
{
	string name = obj->getName();
	ADD_MENU_TITLE  ( mL, name );
	ADD_MENU_LABEL ( mL, "remove_" + name, new CmdRemoveGObject ( obj->getParentView(), obj ) );
	PObject::Ptr mObj = frntCtrl.getViewRelations().get<PObject> ( obj );
	addNode_Entry ( obj, mL, mObj );
	// if IVstEventProcessor
	MidiEventProcessor::Ptr evP = boost::shared_dynamic_cast< MidiEventProcessor, PObject > ( mObj );
	if ( evP ) getMidiMenu(evP, obj, mL);
	addParameter ( obj, mL, mObj );
	
}
//------------------------------------------------------------------------------------------------------------
void GProcessorNodeController::addParameter( GObject::Ptr obj, menu::MenuEntryList &mL, PObject::Ptr mObj )
{
	HasParameter::Ptr hP = boost::shared_dynamic_cast<HasParameter, PObject> (mObj);
	if ( !hP ) return;
	string name = obj->getName();
	CMenu::Ptr sub = menu::CSubMenu::create ( obj->getParentView()->getFrame() );
	MenuEntryList &subEntr = sub->getMenuEntries();
	ADD_MENU_TITLE ( subEntr, "parameter :" ); 
	entriesToMenuList<HasParameter_Tag> ( hP, subEntr, obj, &frntCtrl, hP->getNumParameter() );
	ADD_SUB_MENU ( mL, name + " parameter", sub );
}
//------------------------------------------------------------------------------------------------------------
void GProcessorNodeController::addNode_Entry( GObject::Ptr obj, menu::MenuEntryList &mL, PObject::Ptr mObj )
{
	GProcessorNode::Ptr prNode = boost::shared_dynamic_cast<GProcessorNode, GObject>(obj);
	if (!prNode) return;
	
	VariableOutputAdapter *vO = dynamic_cast<VariableOutputAdapter*> ( mObj.get() );
	if ( vO ) {
		ADD_MENU_LABEL ( mL, "add_output_node", new CmdAddOutput ( 
			obj->getParentView(), 
			prNode,
			&frntCtrl
		) );
	}

	VariableInputAdapter *vI = dynamic_cast<VariableInputAdapter*> ( mObj.get() );
	if ( vI ) {
		ADD_MENU_LABEL ( mL, "add_input_node", new CmdAddInput ( 
			obj->getParentView(), 
			prNode,
			&frntCtrl
		) );
	}
	
}
//------------------------------------------------------------------------------------------------------------
void GProcessorNodeController::unregisterObject ( GObject::Ptr gObj, PObject::Ptr pObj ) {
	CircuidView *view = gObj->getParentView();
	Graph::Ptr g = getRelatedGraph( view );
	Graph::Janitor::Ptr updater = g->getJanitor();
	// entfernen aus graph / CommandWorker loescht Objekt
	ProcessAdapter::Ptr adapter = boost::shared_dynamic_cast<ProcessAdapter, PObject > ( pObj );
	updater->remove ( adapter );
	updater.reset();
	// command worker entfern den muell
	CommandWorker::getCommandWorker()->addCommand ( 
		Command::Ptr ( new CmdRelease<PObject>( pObj ) )
	);
}
//------------------------------------------------------------------------------------------------------------
void GProcessorNodeController::getMidiMenu ( MidiEventProcessor::Ptr evP, 
									 GObject::Ptr obj,
									 ppiGui::menu::MenuEntryList &mL ) 
{
	CMenu::Ptr pCM = CSubMenu::create( obj->getParentView()->getFrame() );
	MenuEntryList &sub = pCM->getMenuEntries();

	Parameter::Ptr par = evP->getMidiChannelParameter(); 
	ADD_MENU_TITLE ( sub, "Midi properties:" );
	ADD_MENU_PARAMETER_ENTRY (
		sub, 
		par,
		new ppiGui::CmdPlaceGKnob( obj->getParentView(), obj->getPtr(), par, &frntCtrl ) 
	);
	ADD_SUB_MENU ( mL, "midi properties", pCM );
}
//============================================================================================================
// class GSwitchNodeController:
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
inline void GSwitchNodeController::getMenuEntryList ( GObject::Ptr obj, menu::MenuEntryList &mL ) {
	GProcessorNodeController *ctrl = 
		(GProcessorNodeController*) frntCtrl.getController(FrontController::CTRL_GPROCESSOR);
	ctrl->createMenu ( obj, mL );
}
//------------------------------------------------------------------------------------------------------------
void GSwitchNodeController::eventHandler ( void *src, const OnIdle &ev ) {
	// step
	StateNodes::iterator it = stateNodes.begin();
	for ( ; it!=stateNodes.end(); ++it ) {
		GObject::Ptr gObj = boost::shared_dynamic_cast<GObject, IHasState> (*it);
		if (!gObj) continue;
		PObject::Ptr pObj = frntCtrl.getViewRelations().get<PObject>(gObj);
		IHasState *st = dynamic_cast<IHasState*>( pObj.get() );
		if (!st) continue;
		(*it)->setState ( (size_t) st->getState() );
		gObj->setObjectDirty();
	}
}
//------------------------------------------------------------------------------------------------------------
void GSwitchNodeController::eventHandler ( void *src, const OnDestroy<GObject> &ev ) {
	GObject::Ptr vObj = ev.src->getPtr();
	IHasState::Ptr stN = boost::shared_dynamic_cast<IHasState, GObject> (vObj);
	if ( stN ) { // GOutputStepNode
		stateNodes.remove ( stN );
	}
	// alle weg go home idle listener
	if ( stateNodes.empty() ) {
		PpiEditor *ed = static_cast<PpiEditor*> ( vObj->getParentView()->getFrame()->getEditor() );
		ed->EventSender<OnIdle>::removeEventListener( this );
	}
}
//------------------------------------------------------------------------------------------------------------
void GSwitchNodeController::registerObject( GObject::Ptr vObj, PObject::Ptr mObj ) {
	// noch kein switch reg. => erstma idle listener anmelden
	if ( stateNodes.empty() ) {
		PpiEditor *ed = static_cast<PpiEditor*> ( vObj->getParentView()->getFrame()->getEditor() );
		ed->EventSender<OnIdle>::addEventListener( this );
	}

	IHasState::Ptr stN = boost::shared_dynamic_cast<IHasState, GObject> (vObj);
	if ( stN ) { // GOutputStepNode
		stateNodes.push_back ( stN );
	}
	vObj->EventSender< OnDestroy<GObject> >::addEventListener ( this );
}
//------------------------------------------------------------------------------------------------------------
void GSwitchNodeController::unregisterObject( GObject::Ptr vObj, PObject::Ptr mObj ) {
	IHasState::Ptr stN = boost::shared_dynamic_cast<IHasState, GObject> (vObj);
	if ( stN ) { // GOutputStepNode
		stateNodes.remove ( stN );
	}
	// alle weg go home idle listener
	if ( stateNodes.empty() ) {
		PpiEditor *ed = static_cast<PpiEditor*> ( vObj->getParentView()->getFrame()->getEditor() );
		ed->EventSender<OnIdle>::removeEventListener( this );
	}
	frntCtrl.getController(FrontController::CTRL_GPROCESSOR)->unregisterObject ( vObj, mObj );
}
//============================================================================================================
// class GKnobConnectionController:
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void GKnobConnectionController::getMenuEntryList ( GObject::Ptr obj, menu::MenuEntryList &mL ){
	// parameter a und b holen
	GConnectionPaPa::Ptr gc = boost::shared_dynamic_cast<GConnectionPaPa, GObject> (obj);
	if (!gc) return;
	Parameter::Ptr a = frntCtrl.getViewRelations().get<Parameter>( gc->getObjectA() );
	Parameter::Ptr b = frntCtrl.getViewRelations().get<Parameter>( gc->getObjectB() );
	if ( !a || !b ) throw com::ppiError::MapError ( "object not found.", __FILE__, __LINE__ );
	CircuidView *view = obj->getParentView();
	// connection ops.
	ADD_MENU_TITLE ( mL, a->getName() + " to " + b->getName() );
	ADD_MENU_LABEL ( mL, "add +/- operator", 
		new CmdAddConnectionOperator<parameter::InverseConnection> (a,b, view ) ); 
	ADD_MENU_LABEL ( mL, "add offset operator", 
		new CmdAddConnectionOperator<parameter::OffsetConnection> (a,b, view ) ); 
	ADD_MENU_LABEL ( mL, "add EXP/LOG operator", 
		new CmdAddConnectionOperator<parameter::ExpConnection> (a,b, view ) ); 
	ADD_MENU_LABEL ( mL, "add LOG/EXP operator", 
		new CmdAddConnectionOperator<parameter::LogConnection> (a,b, view ) ); 
	ADD_MENU_LABEL ( mL, "remove connection", new CmdRemoveGObject( view, gc ) );
	// connection op. parameter:
	ConnectionOperator::Container l = a->getConnectionOperators( b.get() );
	ConnectionOperator::Container::iterator it = l.begin();
	MenuEntryList aMl;
	ADD_MENU_TITLE ( aMl, "operator parameter:" );
	for ( ; it!=l.end(); ++it ){
		HasParameter::Ptr hP = boost::shared_dynamic_cast<HasParameter, ConnectionOperator> (*it);
		if (!hP) continue;
		entriesToMenuList<HasParameter_Tag> ( hP, aMl, obj, &frntCtrl, hP->getNumParameter() );
	}
	if ( aMl.size() > 1 ) mL.splice (  mL.end(), aMl );

}
//------------------------------------------------------------------------------------------------------------
void GKnobConnectionController::registerObject ( GObject::Ptr gObj, PObject::Ptr pObj ) {
}
//------------------------------------------------------------------------------------------------------------
void GKnobConnectionController::unregisterObject ( GObject::Ptr gObj, PObject::Ptr pObj ) {
	if ( !dynamic_cast<GConnectionPaPa*>( gObj.get() ) ) return;
	GConnection::Ptr gc = boost::shared_dynamic_cast<GConnection, GObject> (gObj);
	// Hole GObjects ...
	GObject::Ptr knA = gc->getObjectA();
	GObject::Ptr knB = gc->getObjectB();
	// ... und PObjects 
	Parameter::Ptr pA = frntCtrl.getViewRelations().get<Parameter> ( knA );
	Parameter::Ptr pB = frntCtrl.getViewRelations().get<Parameter> ( knB );
	if ( pA && pB ) pA->removeBiConnection ( pB.get() );
	TOLOG ( pA->getName() + ", " + pB->getName() + " connection removed" );
}
//============================================================================================================
// class GIOConnectionController:
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
inline void GIOConnectionController::getMenuEntryList ( GObject::Ptr obj, menu::MenuEntryList &mL ) {
	if ( !dynamic_cast<GConnectionIO*>(obj.get()) ) return;
	ADD_MENU_TITLE ( mL, "input/output connection : " );
	ADD_MENU_LABEL ( mL, "remove connection", 
		new CmdRemoveGObject ( obj->getParentView(), obj ) );
}
//------------------------------------------------------------------------------------------------------------
void GIOConnectionController::registerObject ( GObject::Ptr gObj, PObject::Ptr pObj ) {
	/*GConnection::Ptr gc = boost::shared_dynamic_cast<GConnection, GObject> (gObj);
	GObject::Ptr a = gc->getObjectA();
	GObject::Ptr b = gc->getObjectB();*/
}
//------------------------------------------------------------------------------------------------------------
void GIOConnectionController::unregisterObject ( GObject::Ptr gObj, PObject::Ptr pObj ) {
	GConnection::Ptr gc = boost::shared_dynamic_cast<GConnection, GObject> ( gObj );
	// Ordne Oberklassen Unterklassen zu. 
	GInputNode *input = NULL; GOutputNode *output = NULL; 
	relate< GInputNode, GOutputNode, GObject > 
		( &input, &output, gc->getObjectA().get(), gc->getObjectB().get() );
	if ( !input || !output ) return;
	ProcessorNode::Ptr prIn  = frntCtrl.getViewRelations().get<ProcessorNode> ( input->getPtr()  );
	ProcessorNode::Ptr prOut = frntCtrl.getViewRelations().get<ProcessorNode> ( output->getPtr() );
	if ( !prIn || !prOut ) return;
	// graph command uebergeben
	Graph::Ptr g = getRelatedGraph ( gc->getParentView() );
	Graph::Janitor::Ptr updater = g->getJanitor();
	updater->removeConnection ( prOut.get(), prIn.get() );
	TOLOG ( prOut->getName() + ", " + prIn->getName() + " connection removed" );
}
//============================================================================================================
// class GPluginController:
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void GPluginController::getMenuEntryList( GObject::Ptr obj, ppiGui::menu::MenuEntryList &mL ) {
	frntCtrl.getController(FrontController::CTRL_GPROCESSOR)->getMenuEntryList (obj, mL);
	Plugin::Ptr plug = frntCtrl.getViewRelations().get<Plugin> ( obj );
	// programs
	getPluginMenu(plug, obj, mL);
}
//------------------------------------------------------------------------------------------------------------
void GPluginController::getPluginMenu ( Plugin::Ptr plug,
										GObject::Ptr obj, 
										ppiGui::menu::MenuEntryList &mL ) 
{
	if ( plug->getNumPrograms() > 0 ) {  // program menu
		CMenu::Ptr sub = menu::CSubMenu::create ( obj->getParentView()->getFrame() );
		MenuEntryList &subEntr = sub->getMenuEntries();
		entriesToMenuList<PlugProgram_Tag> ( plug, subEntr, obj, &frntCtrl, plug->getNumPrograms() );
		ADD_SUB_MENU ( mL, "programs", sub );
		
							   
	}
	
	if ( plug->hasEditor() )  {	// create editor menu if exists
		CMenu::Ptr sub = menu::CSubMenu::create ( obj->getParentView()->getFrame() );
		MenuEntryList &subEntr = sub->getMenuEntries();
		ADD_MENU_TITLE ( subEntr, "Editor:" );
		// X
		Parameter::Ptr par = plug->getEditorPosX();
		ADD_MENU_PARAMETER_ENTRY (
			subEntr, 
			par,
			new ppiGui::CmdPlaceGKnob( obj->getParentView(), obj->getPtr(), par, &frntCtrl ) 
		);
		// Y
		par = plug->getEditorPosY();
		ADD_MENU_PARAMETER_ENTRY (
			subEntr, 
			par,
			new ppiGui::CmdPlaceGKnob( obj->getParentView(), obj->getPtr(), par, &frntCtrl ) 
		);
		// editor open
		par = plug->getEditorOpen();
		ADD_MENU_PARAMETER_ENTRY (
			subEntr, 
			par,
			new ppiGui::CmdPlaceGKnob( obj->getParentView(), obj->getPtr(), par, &frntCtrl ) 
		);
		ADD_SUB_MENU ( mL, "editor window", sub );
	}
}
//------------------------------------------------------------------------------------------------------------
void GPluginController::registerObject( GObject::Ptr vObj, processing::PObject::Ptr mObj ) {
	GVSTPlugNode::Ptr gPlug = boost::shared_dynamic_cast<GVSTPlugNode, GObject> (vObj);
	GButton *btn = gPlug->getEButton();
	btn->EventSender<OnMouseClick>::addEventListener (
		(GObjectController*)frntCtrl.getController(FrontController::CTRL_GOBJECT)
	);
	// new VSTPlugView
	// TODO: VSTPlugview > oberklasse == PlugView
	VSTPlugin::Ptr plug = frntCtrl.getViewRelations().get<VSTPlugin> ( gPlug );
	gPlug->setName ( plug->getPlugName() ); // nochmal name holen; koennte fehlermeldung enthalten
	// checken ob plug editor hatt
	//######################!!! ENDET HIER WENN KEIN EDITOR !!!
	if ( !plug->hasEditor() ) return; 
	gPlug->EventSender<ButtonClicked>::addEventListener (this);
	gPlug->EventSender< OnDestroy<GObject> >::addEventListener (this);
	// reg. handler : main editor closed -> close plug editors
	PpiEditor *ed = (PpiEditor*)gPlug->getParentView()->getEditor();
	VSTPlugView::Ptr view = VSTPlugView::create( ed, plug ); 
	vstPlugViewMap.insert ( VSTPlugViewMap::value_type( gPlug, view ) );
	view->EventSender<OnClose>::addEventListener (this);
	view->EventSender<OnMoving>::addEventListener (this);
	plug->EventSender<EditorPositionEvent>::addEventListener (this);
	plug->EventSender<EditorOpenParameterChanged>::addEventListener (this);
	if ( plug->getEditorOpen()->getValue() > 0.5f ) // editor open parmeter marks open
		openEdWindow ( gPlug ); 
}
//------------------------------------------------------------------------------------------------------------
void GPluginController::eventHandler(void *src, const ppiGui::ButtonClicked &ev) {
	GVSTPlugNode::Ptr node = boost::shared_dynamic_cast<GVSTPlugNode, GObject>( ( (GObject*)src)->getPtr() );
	if ( ev.src->getValue() ){ // button on
		openEdWindow ( node );
	} else { // button off
		closeEdWindow ( node );
	}
}
//------------------------------------------------------------------------------------------------------------
void GPluginController::eventHandler(void *src, const ppiGui::OnClose &ev) {
	VSTPlugView::Ptr view = ((VSTPlugView*) src)->getPtr();
	VSTPlugViewMap::right_map::iterator it = vstPlugViewMap.right.find ( view );
	if ( it == vstPlugViewMap.right.end() ) return;
	// set gPlugNode (e) == 0
	GVSTPlugNode::Ptr gPlug = it->second;
	GButton *btn = gPlug->getEButton();
	if ( btn->getValue() != 0.0f ) {
		btn->setValue(0.0f);
		btn->setObjectDirty();
	}
	// get related plugNode
	Plugin::Ptr plug = frntCtrl.getViewRelations().get<Plugin>( gPlug );
	if (!plug) return;
	// notify parameter skipping listener function:
	Parameter::ParameterListenerFunction oC = boost::bind( 
		&Plugin::paramEditorOpenChanged, plug.get(), _1, _2 
	);
	// search whether keepOpenState contains plug
	PlugNodeList::iterator pIt = com::find<PlugNodeList> ( keepOpenState, plug );
	if ( pIt == keepOpenState.end() ) // nothing found
		plug->getEditorOpen()->setValue ( 0.0f, oC );
    else keepOpenState.erase( pIt );
}
//------------------------------------------------------------------------------------------------------------
void GPluginController::eventHandler(void *src, const EditorOpenParameterChanged &ev) {
	Plugin::Ptr node = boost::shared_dynamic_cast<Plugin, PObject> ( ((PObject*)src)->getPtr() );
	if (!node) return;
	// get GVSTPlugNode
	list<GVSTPlugNode::Ptr> l;
	frntCtrl.getGObjectList<GVSTPlugNode> ( node, l );
	if ( l.empty() ) return;
	GVSTPlugNode::Ptr gPlug = l.front();
	if ( ev.open )
		openEdWindow( gPlug );
	else closeEdWindow ( gPlug );
}
//------------------------------------------------------------------------------------------------------------
// x = 0 => -screenWitdh; x = 1.0f => screenWitdh
inline CPoint float2ScreenCoord ( float x, float y ) {
	VSTGUI::CRect screenSize = getScreenSize();
	size_t rx = (size_t)( x * screenSize.width()  * 2 );
	size_t ry = (size_t)( y * screenSize.height() * 2 );
	return CPoint( rx - screenSize.width(), ry - screenSize.height() );
}
//------------------------------------------------------------------------------------------------------------
// x = 0 => 0.5f; x = 1 = 1.0f
inline pair<float, float> screenCoord2Float ( const CPoint &p ) {
	VSTGUI::CRect screenSize = getScreenSize();
	if ( screenSize.width() == 0 && screenSize.height() == 0 )
		return std::make_pair( 0.0f, 0.0f );
	float x = p.x / ( screenSize.width()  * 2.0f );
	float y = p.y / ( screenSize.height() * 2.0f );
	return std::make_pair ( x + 0.5f, y + 0.5f );
}
//------------------------------------------------------------------------------------------------------------
void GPluginController::eventHandler(void *src, const ppiGui::OnDestroy<GObject> &ev) {
	GObject::Ptr gO = ev.src->getPtr();
	GVSTPlugNode::Ptr gPlug = boost::shared_dynamic_cast<GVSTPlugNode, GObject>( gO );
	if ( !gPlug ) return;
	Plugin::Ptr plug = frntCtrl.getViewRelations().get<Plugin>( gPlug );
	if ( !plug ) return;
	//
	VSTPlugViewMap::left_map::iterator it = vstPlugViewMap.left.find ( gPlug );
	if ( it == vstPlugViewMap.left.end() ) return;
	VSTPlugView::Ptr view = it->second;
	// keeps openParameter on 1.0f
	if ( view->isOpen() ) // open while destroying
		keepOpenState.push_back( plug );
	// close view
	view->closeWindow();
	// rease from map
	vstPlugViewMap.left.erase (it);
	// remove listeners
	gPlug->EventSender< OnDestroy<GObject> >::removeEventListener(this);
	plug->EventSender<EditorPositionEvent>::removeEventListener (this);
	plug->EventSender<EditorOpenParameterChanged>::removeEventListener (this);
	view->EventSender<OnClose>::removeEventListener (this);
	view->EventSender<OnMoving>::removeEventListener (this);
}
//------------------------------------------------------------------------------------------------------------
void GPluginController::eventHandler(void *src, const ppiGui::OnMoving &ev) {
	VSTPlugView::Ptr view = ( (VSTPlugView*)src )->getPtr();
	VSTPlugViewMap::right_map::iterator it = vstPlugViewMap.right.find ( view );
	if ( it == vstPlugViewMap.right.end() ) return;
	Plugin::Ptr plug = frntCtrl.getViewRelations().get<Plugin>( it->second );
	if ( !plug ) return;
	pair<float, float> fp = screenCoord2Float ( ev.newPos );
	plug->getEditorPosX()->setValue(fp.first);
	plug->getEditorPosY()->setValue(fp.second);
}
//------------------------------------------------------------------------------------------------------------
void GPluginController::eventHandler(void *src, const ppiGui::EditorPositionEvent &ev) {
	// get plugnode
	PObject::Ptr pObj = ( (PObject*)src )->getPtr();
	// get GVSTPlugNode
	list<GVSTPlugNode::Ptr> l;
	frntCtrl.getGObjectList<GVSTPlugNode> ( pObj, l );
	if ( l.empty() ) return;
	// get editor
	VSTPlugViewMap::left_map::iterator it = vstPlugViewMap.left.find ( l.front() );
	if ( it == vstPlugViewMap.left.end() ) return;
	VSTPlugView::Ptr view = it->second;
	CPoint pos = float2ScreenCoord( ev.value.first, ev.value.second ); // x, y
	Window::Ptr win = view->getWindow();
	if (win)
		win->setPos ( pos );
}
//------------------------------------------------------------------------------------------------------------
void GPluginController::unregisterObject ( GObject::Ptr gObj, PObject::Ptr pObj ) { 
	GVSTPlugNode::Ptr gPlug = boost::shared_dynamic_cast<GVSTPlugNode, GObject> (gObj);
	Plugin::Ptr plug = boost::shared_dynamic_cast<Plugin, PObject> (pObj);
	// unregister listener
	gPlug->EventSender< OnDestroy<GObject> >::removeEventListener(this);
	plug->EventSender<EditorPositionEvent>::removeEventListener (this);
	plug->EventSender<EditorOpenParameterChanged>::removeEventListener (this);
	// entferne pobject
	VSTPlugViewMap::left_map::iterator it = vstPlugViewMap.left.find ( gPlug );
	if ( it != vstPlugViewMap.left.end() ) {
		VSTPlugView::Ptr view = it->second; 
		view->closeWindow();
		view->EventSender<OnClose>::removeEventListener (this);
		view->EventSender<OnMoving>::removeEventListener (this);
		vstPlugViewMap.left.erase (it);
	}
	ObjectController *ctrl = frntCtrl.getController ( FrontController::CTRL_GPROCESSOR );
	ctrl->unregisterObject ( gObj, pObj ); // entfernt pobj aus graph
}
//------------------------------------------------------------------------------------------------------------
void GPluginController::openEdWindow ( const GVSTPlugNode::Ptr &gPlug ) {
	// find related plugView
	VSTPlugViewMap::left_map::iterator it = vstPlugViewMap.left.find ( gPlug );
	if ( it == vstPlugViewMap.left.end() ) return;
	VSTPlugView::Ptr view = it->second;
	if ( view->isOpen() ) return;
	// get related plugNode
	Plugin::Ptr plug = frntCtrl.getViewRelations().get<Plugin>( gPlug );
	if (!plug) return;
	// notify parameter skipping listener function:
	Parameter::ParameterListenerFunction oC = boost::bind( 
		&Plugin::paramEditorOpenChanged, plug.get(), _1, _2 
	);
	plug->getEditorOpen()->setValue ( 1.0f, oC ); 
	// set (e) button
	GButton *btn = gPlug->getEButton();
	if ( btn->getValue() != 1.0f ) {
		btn->setValue(1.0f);
		btn->setObjectDirty();
	}
	// calculate position
	CPoint nP = float2ScreenCoord ( plug->getEditorPosX()->getValue(), plug->getEditorPosY()->getValue() );
	// open window at position via command: problems when open signal comes from processing object
	ppiGui::PpiEditor *ed = (PpiEditor *)gPlug->getParentView()->getEditor();
	Command::Ptr cmd( new CmdOpenEditor( view, nP ) );
	ed->addCommand ( cmd );
}
//------------------------------------------------------------------------------------------------------------
void GPluginController::closeEdWindow ( const GVSTPlugNode::Ptr &node ) {
	VSTPlugViewMap::left_map::iterator it = vstPlugViewMap.left.find ( node );
	if ( it == vstPlugViewMap.left.end() ) return;
	VSTPlugView::Ptr view = it->second;
	if ( !view->isOpen() ) return;
	// close window via command: problems when open signal comes from processing object
	ppiGui::PpiEditor *ed = (PpiEditor *)node->getParentView()->getEditor();
	ed->addCommand ( Command::Ptr( new CmdCloseEditor( view ) ) );
}
//------------------------------------------------------------------------------------------------------------
GPluginController::~GPluginController() {
}
} // namespace ppiGui

namespace {
//============================================================================================================
// DynMenuEntry List
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
template <typename Tag>
struct DynMenuCallbackType {
	typedef pair<int, int> StartEnd;
	StartEnd startEnd;
	typename Tag::TagType object;
	ppiGui::GObject::Ptr gObj;
	ppiGui::FrontController *ctrl;
	bool showTitle;
	DynMenuCallbackType ( const typename Tag::TagType &object, 
						  ppiGui::GObject::Ptr gObj, 
						  ppiGui::FrontController *ctrl,
						  StartEnd startEnd,
						  bool showTitle = true
						 ) : startEnd(startEnd), object(object), gObj(gObj), ctrl(ctrl), showTitle(showTitle) {} 
};
//------------------------------------------------------------------------------------------------------------
template <typename Tag>
void dynMenuCallback ( ppiGui::menu::MenuEntryList &mL, const DynMenuCallbackType<Tag> &cBkObject ) {
	BOOST_STATIC_ASSERT( sizeof(Tag) == 0 );
}
//------------------------------------------------------------------------------------------------------------
template <>
void dynMenuCallback<PlugProgram_Tag> ( ppiGui::menu::MenuEntryList &mL, 
									    const DynMenuCallbackType<PlugProgram_Tag> &cBkObject ) 
{
	using namespace ppiGui;
	using namespace ppiGui::menu;
	size_t s = cBkObject.startEnd.first;
	size_t e = cBkObject.startEnd.second;
	std::stringstream ss;
	ss << PlugProgram_Tag::getPrefix() << " (" << s << ".." << e << ")";
	if ( cBkObject.showTitle ) 
		ADD_MENU_TITLE ( mL, ss.str() );
	for ( size_t i=s; i<e; ++i ) {
		ADD_MENU_LABEL( mL, 
			cBkObject.object->getProgramName(i), 
			new CmdSetProgram( cBkObject.gObj, i, cBkObject.ctrl) 
		);
	}
}
//------------------------------------------------------------------------------------------------------------
template <>
void dynMenuCallback<HasParameter_Tag> ( ppiGui::menu::MenuEntryList &mL, 
									    const DynMenuCallbackType<HasParameter_Tag> &cBkObject ) 
{
	using namespace ppiGui;
	using namespace ppiGui::menu;
	size_t s = cBkObject.startEnd.first;
	size_t e = cBkObject.startEnd.second;
	std::stringstream ss;
	ss << HasParameter_Tag::getPrefix() << " (" << s << ".." << e << ")";
	if ( cBkObject.showTitle ) 
		ADD_MENU_TITLE ( mL, ss.str() );
	for ( size_t i=s; i<e; ++i ) {
		Parameter::Ptr par = cBkObject.object->getParameter(i);
		ADD_MENU_PARAMETER_ENTRY (
			mL, 
			par,
			new ppiGui::CmdPlaceGKnob( cBkObject.gObj->getParentView(), cBkObject.gObj, par, cBkObject.ctrl ) 
		);
	}
}
//------------------------------------------------------------------------------------------------------------
template <typename Tag>
void entriesSpread  ( const typename Tag::TagType &object, 
				      ppiGui::menu::MenuEntryList &mL,
					  ppiGui::GObject::Ptr src,
					  ppiGui::FrontController *ctrl,
					  size_t num_entries )
{
	using namespace ppiGui;
	using namespace ppiGui::menu;
	for ( size_t i = 0 ; i < num_entries; i+= Tag::MAX_ENTRIES ) {
		size_t start = i;
		size_t end   = i + Tag::MAX_ENTRIES < num_entries ? i + Tag::MAX_ENTRIES : num_entries;
		MyString name = Tag::getPrefix() + " " + MyString(start+1) + ".." + MyString(end);
		ADD_DYNSUB_MENU (												       // host knobs via callback
			mL, 														       // menuEntryList
			name,       												       // menuEntry Label
			DynMenuCallbackType<Tag>,                
			DynMenuCallbackType<Tag>( object, src, ctrl, std::make_pair( start, end ) ),           
			boost::bind
				( &dynMenuCallback<Tag>, _1, _2 ),                           // function
			src->getParentView()->getFrame()							     // CFrame 
		);
	}
}
//------------------------------------------------------------------------------------------------------------
template <typename Tag>
void allEntriesAtOnce ( const typename Tag::TagType &object, 
						ppiGui::menu::MenuEntryList &mL,
						ppiGui::GObject::Ptr src,
						ppiGui::FrontController *ctrl,
						size_t num_entries )
{
	using namespace ppiGui;
	using namespace ppiGui::menu;
	dynMenuCallback<Tag> ( mL, DynMenuCallbackType<Tag>( object, src, ctrl, std::make_pair(0, num_entries), false ) ); 
}
//------------------------------------------------------------------------------------------------------------
template <typename Tag>
inline void entriesToMenuList ( const typename Tag::TagType &object, 
							    ppiGui::menu::MenuEntryList &mL,
							    ppiGui::GObject::Ptr src,
							    ppiGui::FrontController *ctrl,
								size_t num_entries )
{
	if ( num_entries < Tag::MAX_ENTRIES )
		allEntriesAtOnce<Tag> ( object, mL, src, ctrl, num_entries );
	else 
		entriesSpread<Tag> ( object, mL, src, ctrl, num_entries );
}
} // namespace
