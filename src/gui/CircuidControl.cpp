#include "CircuidControl.h"
#include "ViewCommand.h"
#include "MainCtrl.h"
#include "processing/ConcreteProcessAdapter.h"
#include "PpiEditor.h"
#include "OS_Specific/WindowDef.h"

namespace ppiGui{
using namespace menu;
//============================================================================================================
// class CircuidControl:
// Controler Klasse fuer CircuidView gemaess MVC.
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void CircuidControl::initMouseActions() {
	comMActions = new MouseAction*[NUM_MACTIONS];
	comMActions[MOVE] = new MAMove ();
	comMActions[CONNECT] = new MAConnect( view );
	comMActions[USE] = new MAUse();
	currMouseAction = comMActions[USE];
}
//------------------------------------------------------------------------------------------------------------
CircuidControl::CircuidControl ( CircuidView *view, ViewRelations &vr ) : 
view(view), 
view2model(vr),
normCursor(true)
{
	initMouseActions();
	view->EventSender<OnMouseClick>::addEventListener ( this );
	view->EventSender<OnMouseDrag>::addEventListener ( this );
	gObjCtrlDirector = new FrontController ( view2model );
	gObjCtrlDirector->setMouseAction ( currMouseAction );
	// Fuege Selection Object ein
	selection = GSelection::create ( view );
	selection->setColor ( ppiGui::red );
	view->addGObject (selection, CircuidView::SELECTION);
	contextMenu = menu::CMenu::create( view->getFrame() );
	contextMenu->EventSender<OnMouseLeave>::addEventListener ( this );
	selectionRect = GRect::create ( view );
	view->addGObject ( selectionRect );
	selectionRect->setWidth (1);
	selectionRect->setLineStyle ( kLineOnOffDash );
	selectionRect->setDrawStyle ( kDrawStroked );
	selectionRect->setColor ( black );
	selectionRect->setVisible ( false );
	pluginCollection = PluginCollection::getPluginCollection();
	// idle listener
	PpiEditor* ed = static_cast<PpiEditor*> ( view->getFrame()->getEditor() );
	ed->EventSender<OnIdle>::addEventListener ( this );
}
//------------------------------------------------------------------------------------------------------------
void CircuidControl::initView() {
	Graph::Ptr g = getRelatedGraph(view);
	// * !! Output ist das ding oben ( schon tausendmal verwechselt )
	GOutputNode::Ptr out = GOutputNode::create ( view, Resources::MAIN_OUTPUT_NODE );
	gObjCtrlDirector->registerObject ( out, g->getStartNode() );
	out->offset ( ( SETTINGS->getWindowWidth()>>1 ) - ( out->getSize().getWidth()>>1 ), 50);
	view->addGObject (out);
	// * !! Input ist das ding unten
	GInputNode::Ptr in = GInputNode::create ( view, Resources::MAIN_INPUT_NODE );
	gObjCtrlDirector->registerObject ( in, g->getEndNode() );
	in->offset ( 
		( SETTINGS->getWindowWidth()>>1 ) - ( in->getSize().getWidth()>>1 ), SETTINGS->getWindowHeight() - 100 
	);
	view->addGObject (in);

}
//------------------------------------------------------------------------------------------------------------
void CircuidControl::eventHandler ( void *scr, const OnMouseLeave &ev ) { // menu focus verloren
	//contextMenu->hide();
	//view->CView::setDirty();
}
//------------------------------------------------------------------------------------------------------------
void CircuidControl::vstFolderMenuCallBack ( menu::MenuEntryList &mL, const SubMenuVSTFolderCallback &obj ) {
	createDynMenuPlugTree ( mL, obj.folder );
}
//------------------------------------------------------------------------------------------------------------
void CircuidControl::hostParameterMenuCallBack ( menu::MenuEntryList &mL, const SubMenuHostParameterCallback &obj ) {
	size_t s = obj.startEnd.first;
	size_t e = obj.startEnd.second;
	ADD_MENU_TITLE ( mL, "Parameter: " + MyString( s ) + "-" + MyString(e) );
	Graph::Ptr graph = getRelatedGraph (view);
	for ( int i=s; i<e; ++i ){
		ADD_MENU_LABEL ( mL, "add_host_knob " + MyString(i+1), new CmdCreateHostGKnob( view, i, gObjCtrlDirector ) );
	}
}
//------------------------------------------------------------------------------------------------------------
void CircuidControl::eventHandler( void *src, const OnMouseClick &ev ) {
	GObject::Ptr gSrc = ev.src ? ev.src->getPtr() : GObject::Ptr();
	selection->clear();
	selection->setObjectDirty();
	if ( ev.btn == kRButton ) { // Kontext Menu aufrufen
		if ( contextMenu->isVisible() ) contextMenu->hide();
		showContextMenu ( gSrc, ev.p );
		return;
	}
	if ( ev.btn == kLButton ){
		if ( contextMenu->isVisible() ) {
			contextMenu->hide();
			view->CView::setDirty();
		}
	}
}
//------------------------------------------------------------------------------------------------------------
void CircuidControl::eventHandler( void *src, const OnMouseDrag &ev ){
	if ( ev.btn == kLButton && currMouseAction != comMActions [MOVE] ) spanSelectionRect( ev );
	if ( ev.btn == kMButton || currMouseAction == comMActions [MOVE] ) dragView( ev );
}
//------------------------------------------------------------------------------------------------------------
inline void CircuidControl::dragView ( const OnMouseDrag &ev ){
	if ( ev.state == OnMouseDrag::DRAG ){
		selection->clear();
		view->offsetView ( MyPoint(ev.p) - MyPoint(ev.dragDirection) ); 
	}
}
//------------------------------------------------------------------------------------------------------------
inline void CircuidControl::spanSelectionRect ( const OnMouseDrag &ev ){
	if ( ev.state == OnMouseDrag::DRAG_START ){
		VSTGUI::CRect r = selectionRect->getSize();
		r.left = ev.p.x;
		r.top = ev.p.y;
		r.right = ev.p.x;
		r.bottom = ev.p.y;
		selectionRect->setRect ( r );
		selectionRect->setVisible ( true );
	}
	if ( ev.state == OnMouseDrag::DRAG ){
		VSTGUI::CRect r = selectionRect->getSize();
		r.right = ev.p.x;
		r.bottom = ev.p.y;
		selectionRect->setRect ( r );
		selectionRect->setObjectDirty();
	}
	if ( ev.state == OnMouseDrag::DRAG_STOP ){
		selectionRect->setVisible ( false );
		selectionRect->setObjectDirty();
		selection->clear(); // erst mal loeschen
		list<GObject::Ptr> oL;
		view->getObjectsInArea ( normalizeRect( selectionRect->getSize() ) , oL ); // hole objekte im bereich selRect
		selection->addGObjects (oL);
		view->CView::setDirty();
	}
}
//------------------------------------------------------------------------------------------------------------
inline void CircuidControl::showContextMenu( GObject::Ptr obj, CPoint& point ){
	menu::MenuEntryList &mList = contextMenu->getMenuEntries();
	if ( obj ) gObjCtrlDirector->getMenuEntryList ( mList, obj );
	else getMenuEntryList ( mList );
	contextMenu->showAt ( point );
}
//------------------------------------------------------------------------------------------------------------
// wird von eventHandler( void *src, const OnGetVSTFolder &ev ) aufgerufen
inline void CircuidControl::createDynMenuPlugTree ( menu::MenuEntryList &mE, const PluginCollection::Folder &folder ) 
{
	using namespace processing;
	typedef PluginCollection::PluginInfoList PluginInfoList;
	// get folders plugs
	PluginInfoList pluginInfoList;
	pluginCollection->getPlugInfoList ( folder, pluginInfoList, /*SETTINGS->isFastScan()*/true );
	// get folders subfolder
	PluginCollection::Folders folders; 
	pluginCollection->getSubFolders( GET_FOLDER_ID(folder), folders );
	if ( pluginInfoList.empty() && folders.empty() ) return;
	
	ADD_MENU_TITLE ( mE, GET_FOLDER_NAME(folder) ); // menu title
	// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> add subfolder
	for ( PluginCollection::Folders::iterator it = folders.begin(); it!=folders.end(); ++it ) {
		ADD_DYNSUB_MENU ( 
			mE,     					                                   // menuEntryList
			GET_FOLDER_NAME(*it),						                   // menuEntry Label
			SubMenuVSTFolderCallback,                
			SubMenuVSTFolderCallback( *it ),					           //Path, Menu title
			boost::bind
				( &CircuidControl::vstFolderMenuCallBack, this, _1, _2  ), // function
			view->getFrame()											   // CFrame 
		);
	} 
	// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> add plugs
	for ( PluginInfoList::iterator it = pluginInfoList.begin(); it!=pluginInfoList.end(); ++it ) {
		string cue;
		if ( (*it).access == PluginInfo::FAILED ) cue = " <failed>"; // konnte bei init. nicht geladen werden.
		if ( (*it).access == PluginInfo::NOT_CHECKED ) cue = ""; // wurde nicht geprueft
		ADD_MENU_LABEL ( 
			mE, 
			string( it->isSynth ? "(i)" : "" ) + it->name + cue, 
			new CmdAddVSTPlugNode( view, pluginCollection, *it, gObjCtrlDirector ) 
		);
	}
}
//------------------------------------------------------------------------------------------------------------
// erzeugt plugin-submenu eintraege. folgende moegl.:
//		- kein plugin verz.    => nur titel
//	    - ein plugin. verz.    => plugins als direktes submenu
//      - mehrere plugin verz. => mehrere submenus mit verzeichnissen
inline void CircuidControl::createVSTPluginDynSubMenu ( menu::MenuEntryList &mE ) {
	static const string str_add = "add_plugin";
	
	// plugin verz.
	CMenu::Ptr plugMenu = CSubMenu::create( view->getFrame() );
	MenuEntryList &plugMenuL = plugMenu->getMenuEntries();
	/*get root sub folders*/
	PluginCollection::Folders folders;
	pluginCollection->getSubFolders ( PluginCollection::ROOT_FOLDER_ID, folders );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> null folder setted
	if ( folders.empty() ) {
		ADD_MENU_TITLE ( plugMenuL, "No VST Directories setted!" );
		// zu menu list hinz.
		ADD_SUB_MENU ( mE, str_add, plugMenu );
		return;
	}
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> one folder setted
	if ( folders.size() == 1 ) { // nur ein folder
		ADD_DYNSUB_MENU ( 
			mE,															   // menuEntryList
			str_add,													   // menuEntry Label
			SubMenuVSTFolderCallback, 
								    
			SubMenuVSTFolderCallback(  folders.front()  ), 
			boost::bind
				( &CircuidControl::vstFolderMenuCallBack, this, _1, _2  ), // function
			view->getFrame()									           // CFrame 
		);
		return;
	}
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> several folders setted
	ADD_MENU_TITLE ( plugMenuL, "VST Directories:" );
	for ( PluginCollection::Folders::iterator it = folders.begin(); it!=folders.end(); ++it ){
		ADD_DYNSUB_MENU ( 
			plugMenuL,					                                   // menuEntryList
			GET_FOLDER_NAME(*it),										   // menuEntry Label
			SubMenuVSTFolderCallback,                
			SubMenuVSTFolderCallback( *it ),							   // folder
			boost::bind
				( &CircuidControl::vstFolderMenuCallBack, this, _1, _2  ), // function
			view->getFrame()											   // CFrame 
		);
	}
	// zu menu list hinz.
	ADD_SUB_MENU ( mE, str_add, plugMenu );
}
//------------------------------------------------------------------------------------------------------------
inline void CircuidControl::getMenuEntryList ( menu::MenuEntryList &mE ){
	ADD_MENU_TITLE ( mE, "things you can do on this view:" );
	createVSTPluginDynSubMenu (mE);
	// pr. nodes
	ADD_MENU_LABEL ( mE, "add_volume_node", new CmdCreateVolumeNode ( view, gObjCtrlDirector ) );
	ADD_MENU_LABEL ( mE, "add_pan_node", new CmdCreatePanAdapter ( view, gObjCtrlDirector ) );
	ADD_MENU_LABEL ( mE, "add_input_step_node", new CmdCreateIStepNode ( view, gObjCtrlDirector ) );
	ADD_MENU_LABEL ( mE, "add_output_step_node", new CmdCreateOStepNode ( view, gObjCtrlDirector ) );
	ADD_MENU_LABEL ( mE, "add_input_switch_node", new CmdCreateInputSwitchNode ( view, gObjCtrlDirector ) );
	ADD_MENU_LABEL ( mE, "add_output_switch_node", new CmdCreateOutputSwitchNode ( view, gObjCtrlDirector ) );
	ADD_MENU_LABEL ( mE, "add_peak_tracker", new CmdCreatePeakTracker ( view, gObjCtrlDirector ) );
	ADD_MENU_LABEL ( mE, "add_adsr_trigger", new CmdCreateADSRTriggerNode ( view, gObjCtrlDirector ) );
	ADD_MENU_LABEL ( mE, "add_midi_receiver", new CmdCreateMidiProcessor ( view, gObjCtrlDirector ) );
	ADD_MENU_LABEL ( mE, "add_free_knob", new CmdCreateFreeGKnob( view, gObjCtrlDirector ) );
	
	CMenu::Ptr pCM = CSubMenu::create( view->getFrame() );
	MenuEntryList &sub = pCM->getMenuEntries();
	// add dynamic submenues for host parameter:
	// 0..n; n..2n; 2n..3n; ...
	static const size_t STEP = 100;
	for ( size_t i = 0 ; i < com::Settings::PROGRAM_PARAMETER; i+=STEP ) {
		size_t start = i;
		size_t end   = i + STEP < com::Settings::PROGRAM_PARAMETER ? i + STEP : com::Settings::PROGRAM_PARAMETER;
		MyString name = "host_knobs " + MyString(start+1) + ".." + MyString(end);
		ADD_DYNSUB_MENU (												       // host knobs via callback
			sub, 														       // menuEntryList
			name,       												       // menuEntry Label
			SubMenuHostParameterCallback,                
			SubMenuHostParameterCallback( std::make_pair( start, end ) ),      
			boost::bind
				( &CircuidControl::hostParameterMenuCallBack, this, _1, _2  ), // function
			view->getFrame()									               // CFrame 
		);
	}
	ADD_SUB_MENU ( mE, "host_knobs", pCM );
}
//------------------------------------------------------------------------------------------------------------
CircuidControl::~CircuidControl(){
	if ( contextMenu->isVisible() ) contextMenu->hide();
	// remove idle listener
	PpiEditor* ed = static_cast<PpiEditor*> ( view->getFrame()->getEditor() );
	ed->EventSender<OnIdle>::removeEventListener ( this );
	for ( int i=0; i<NUM_MACTIONS; i++ ) delete comMActions[i];
	delete comMActions;
	delete gObjCtrlDirector;
}
//------------------------------------------------------------------------------------------------------------
void CircuidControl::eventHandler( void *scr, const ToolBoxBtnPressed &ev ) {
	switch ( ev.id ){
		case MToolBox::tBtnUse :
			currMouseAction = comMActions [USE];
			break;
		case MToolBox::tBtnConnect :
			currMouseAction = comMActions [CONNECT];
			break;
		case MToolBox::tBtnMove :
			currMouseAction = comMActions [MOVE];
			break;
	}
	gObjCtrlDirector->setMouseAction ( currMouseAction );
}

//------------------------------------------------------------------------------------------------------------
void CircuidControl::save ( oArchive &ar ) {
	// serialisierbare objekte sammeln:
	GObjectList objs;
	CircuidView::GObjectStageBuffer::iterator it = view->gObjectBuffer.begin();
	LOG_ASSERT ( it != view->gObjectBuffer.end() );
	for ( ; it != view->gObjectBuffer.end(); ++it ){ // druchlaufe alle GObjects auf der View
		Serializable *obj = dynamic_cast< Serializable* > ( (*it).second.get() );
		if (!obj) continue;
		objs.push_back ( (*it).second );
	}
	// serialisieren:
	ar<<objs;
	TOLOG ( MyString( objs.size() ) + " objects saved.");
}
//------------------------------------------------------------------------------------------------------------
void CircuidControl::load ( iArchive &ar ) {
	// serialisieren:
	GObjectList objs;
	ar>>objs;
	GObjectList::iterator it = objs.begin();
	for ( ;it!=objs.end(); ++it ){
		view->addGObject ( *it, (*it)->getZPos() ); 
	}
	TOLOG ( MyString( objs.size() ) + " objects loaded.");
}
//------------------------------------------------------------------------------------------------------------
void CircuidControl::eventHandler ( void *src, const OnIdle &ev ) {
	if ( contextMenu->isVisible() ) return;
	CPoint p;
	view->getMouseLocation(p);
	GObject::Ptr obj = view->getGObjectAt ( p, CircuidView::TOP, CircuidView::BOTTOM );
	if ( dynamic_cast<PlaceGObject*>( obj.get() ) ) return; 
	if ( !obj ) { // maus ins leere
		if ( onMouseObj ) {
			onMouseObj->onMouseLeave ( p );
			onMouseObj.reset();
		}
		return;
	}
	if ( onMouseObj == obj ) return; // nix geaendert
	if ( onMouseObj ) onMouseObj->onMouseLeave ( p );
	onMouseObj = obj;
	onMouseObj->onMouseEnter(p);
}
} // namespace ppiGui
