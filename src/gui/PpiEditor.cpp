#include "PpiEditor.h"
#include "com/RegisterBoostTypes.h"
#include <fstream>

template < typename A >
extern void register_types ( A& );


namespace {
	VSTGUI::CRect getCircuidViewSize() {
		com::Settings::Ptr settings = Settings::getSettings();
		// TODO x,y werte abhaengig von fenster einstellung ( zb. xp-style )
		return VSTGUI::CRect ( -1,24, settings->getWindowWidth() , settings->getWindowHeight() );
	}
	const int IDLE_TIME_MS = 25;
}

using namespace com;
namespace ppiGui {
//============================================================================================================
//	Klasse OwnIdleTimer:
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
OwnIdleTimer::OwnIdleTimer(ppiGui::PpiEditor *ed) : ed(ed), SysTimer(IDLE_TIME_MS) {
}
//------------------------------------------------------------------------------------------------------------
void OwnIdleTimer::callBack() {
	//ed->_idle();
}
//============================================================================================================
Resources::Ptr resources;
//============================================================================================================
//  Relation View=>Graph :
//============================================================================================================
View2Graph view2Graph;
//------------------------------------------------------------------------------------------------------------
Graph::Ptr getRelatedGraph ( CircuidView *view) {
	View2Graph::iterator it = view2Graph.find(view);
	if ( it==view2Graph.end() ) return Graph::Ptr();
	return (*it).second;
}
//------------------------------------------------------------------------------------------------------------
void addViewRelation ( CircuidView *view, Graph::Ptr graph ) {
	view2Graph.insert ( pair< CircuidView*, Graph::Ptr >( view, graph ) );
}
//------------------------------------------------------------------------------------------------------------
void removeViewRelation ( CircuidView *view, Graph::Ptr graph ) {
	View2Graph::iterator it = view2Graph.find(view);
	if ( it==view2Graph.end() ) return;
	view2Graph.erase (it);
}
//============================================================================================================
//	Klasse PpiEditor:
//  Haupt PPI GUI Klasse. Generalisiert AeffGUIEditor von vstgui.
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
int PpiEditor::instances = 0;
//------------------------------------------------------------------------------------------------------------
PpiEditor::PpiEditor ( void *ptr ) : 
	AEffGUIEditor((AudioEffect*)ptr), 
	scanInterrupted(false)
{
	instances++;
	rect.left   = 0;
	rect.top    = 0;
	rect.right  = SETTINGS->getWindowWidth();
	rect.bottom = SETTINGS->getWindowHeight();
	frame = NULL;
	TOLOG ( MyString ("+PPIEditor.") );
	firstInit = true;
	settings = Settings::getSettings();
	sysExec = CommandWorker::getCommandWorker();
}

//------------------------------------------------------------------------------------------------------------
PpiEditor::~PpiEditor () {
	if ( frame ) {
		close();
	}
	try {
		graph.reset();
	} catch ( ... ) {
		throw com::ppiError::PPIError ( "Exception!" , __FILE__, __LINE__ );
	}
	TOLOG ( MyString ("-PPIEditor.") );
}

//------------------------------------------------------------------------------------------------------------
bool PpiEditor::open( void *ptr ){
	if ( !graph ) {
		throw com::ppiError::NullPointer ("graph unsetted", __FILE__, __LINE__ );
	}
	TOLOG ("open");
	AEffGUIEditor::open (ptr);
	resources = Resources::initResources ( this );
	VSTGUI::CRect size (rect.left , rect.top, rect.right, rect.bottom);
	mainCtrl = MainCtrl::Ptr ( new ppiGui::MainCtrl (size, ptr, this) );
	this->frame = mainCtrl.get();
	if ( firstInit ) {
		firstInit = false;
		circuidView = new CircuidView ( getCircuidViewSize() );
		addViewRelation ( circuidView, graph ); // registriere View=>Graph
		frame->addView ( (CView*)circuidView );
		// erst nach addView weil: NULL Pointer bei getFrame()
		// da addView() protected Member Atribute der View initalisiert.
		circuidControl = new CircuidControl ( circuidView, view2model );
		mainCtrl->getToolBox()->EventSender<ToolBoxBtnPressed>::addEventListener ( circuidControl );
		circuidControl->initView();
	}
	else {
		loadView();
		MToolBox *t = mainCtrl->getToolBox();
		t->EventSender<ToolBoxBtnPressed>::addEventListener ( circuidControl );
	}

	mainCtrl->EventSender<OnResize>::addEventListener ( this );
	mainCtrl->getSetupCtrl()->EventSender<ScanInterrupted>::addEventListener ( this );

	if ( scanInterrupted ) {
		com::MessageBox ( "VSTForx", 
			              "Scan progess was interrupted. Please restart scan.", 
						  com::MSG_ALERT );
		scanInterrupted = false;
	}
	/*idleTimer = new OwnIdleTimer( this );
	idleTimer->start();*/
	return true;
}

//------------------------------------------------------------------------------------------------------------
void PpiEditor::save(oArchive &ar) {
	com::MethodMessage<PpiEditor> methodMessage("save()");
	if ( frame ) saveView();
	ar << view2model;
	string viewStr = viewbuffer.str();
	ar << viewStr;
}
//------------------------------------------------------------------------------------------------------------
void PpiEditor::saveView() {
	com::MethodMessage<PpiEditor> methodMessage("saveView()");
	viewbuffer.str("");
	viewbuffer.clear();
	oArchive oa(viewbuffer);
	register_types<oArchive> (oa);
	oa << circuidView;
	circuidControl->save ( oa );
	view2model.saveViewRelation ( oa );
}
//------------------------------------------------------------------------------------------------------------
void PpiEditor::load( iArchive &ar, processing::Graph::Ptr &graph ) {
	com::MethodMessage<PpiEditor> methodMessage("load()");
	TRY_TO_LOCK_TIMED(mutex);
	this->graph = graph;
	firstInit = false; // !!
	if ( frame ) {
		//frame->advanceNextFocusView ( circuidView ); // ?? ohne dies -> probleme mit focus wenn vorher
		releaseView();								 // menu offen war. 
	}
	ar >> view2model;
	string viewStr;
	ar >> viewStr;
	viewbuffer.str("");
	viewbuffer.clear();
	viewbuffer.str(viewStr);
	if ( frame ) {
		loadView();
		getMainCtrl()->getToolBox()->EventSender<ToolBoxBtnPressed>::addEventListener ( circuidControl );
	}
}
//------------------------------------------------------------------------------------------------------------
void PpiEditor::loadView() {
	com::MethodMessage<PpiEditor> methodMessage("loadView()");
	iArchive ia(viewbuffer);
	register_types<iArchive> (ia);
	ia >> circuidView;
	VSTGUI::CRect vS = getCircuidViewSize();
	circuidView->setViewSize ( vS );
	addViewRelation ( circuidView, graph ); // registriere View=>Graph
	frame->addView ( (CView*)circuidView );
	circuidControl = new CircuidControl ( circuidView, view2model );
	circuidControl->load ( ia );
	view2model.loadViewRelation ( ia );
	circuidControl->getFrontController().reRegisterObjects();
}
//------------------------------------------------------------------------------------------------------------
void PpiEditor::close(){
	TRY_TO_LOCK_TIMED2( mutex, 15 );
	try {                     
		TOLOG ("close");
		// serialisiere view
		saveView();
		frame->removeAll (false);
		releaseView();
		executeCommands(); //!! uebrig gebl. ausf. bevor mainCtrl released
		mainCtrl.reset();
		frame = NULL;
	} catch (...) {
		throw com::ppiError::PPIError ( "Exception!" , __FILE__, __LINE__ );
	}
}
//------------------------------------------------------------------------------------------------------------
void PpiEditor::eventHandler(void *src, const ppiGui::OnResize &ev) {
	CCoord w = settings->getWindowWidth();
	CCoord h = settings->getWindowHeight();
	rect.left   = 0;
	rect.top    = 0;
	rect.right  = w;
	rect.bottom = h;
	VSTGUI::CRect r = getCircuidViewSize();
	circuidView->CView::setViewSize ( r );
	circuidView->CView::setDirty();
}
//------------------------------------------------------------------------------------------------------------
void PpiEditor::eventHandler(void *src, const ScanInterrupted &ev) {
	scanInterrupted = true;
}
//------------------------------------------------------------------------------------------------------------
void PpiEditor::releaseView(){
	com::MethodMessage<PpiEditor> methodMessage("releaseView()");
	/* !! Reihenfolge wichtig!
		   circuidView.clear() => unregister im controller
		   frontController.clear() => gobjects werden geloescht */
	circuidView->clear();
	circuidControl->getFrontController().clear();
	getMainCtrl()->getToolBox()->EventSender<ToolBoxBtnPressed>::removeEventListener ( circuidControl );
	frame->removeView( circuidView, false );
	removeViewRelation ( circuidView, graph );
	delete circuidControl;
	delete circuidView;	
}
//------------------------------------------------------------------------------------------------------------
void PpiEditor::idle() {
	EventSender<OnIdle>::notifyEventListeners ( this, OnIdle() );
	AEffGUIEditor::idle();
	//:::::::::=============---------__________________
	TRY_TO_LOCK_TIMED ( mutex );
	//:::::::::=============---------__________________
	//unbedingt als letztes ausfuehren! 
	// Mutex verhindert das close() ausfuehren kann. Davor sollten alle commands abgearbeitet werden.
	executeCommands(); 
}
} //namespace ppiGui


