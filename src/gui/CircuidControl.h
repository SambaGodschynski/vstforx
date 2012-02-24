/*
 * ===========================================================================================================
 * CircuidControl.h
 *      Author: Johannes Unger
 * ===========================================================================================================
 */

#ifndef CIRCUID_CONTROL
#define CIRCUID_CONTROL

#include "CircuidView.h"
#include "MouseAction.h"
#include "ViewEvents.h"
#include "processing/processing.h"
#include "CMenu.h"
#include "GObjectController.h"
#include "com/PluginCollection.h"

namespace ppiGui{
using namespace std;
//============================================================================================================
// Klasse ToolBoxEvent
//============================================================================================================
struct ToolBoxEvent : Event {};
//============================================================================================================
// Klasse ToolBoxBtnPressed
//============================================================================================================
struct ToolBoxBtnPressed : ToolBoxEvent {
	long id;
	ToolBoxBtnPressed ( long id ) : id(id) {}
};
//------------------------------------------------------------------------------------------------------------
struct SubMenuVSTFolderCallback {
	PluginCollection::Folder folder;
	SubMenuVSTFolderCallback ( const PluginCollection::Folder folder ) : folder(folder) {}
};
//------------------------------------------------------------------------------------------------------------
struct SubMenuHostParameterCallback {
	typedef pair<int, int> StartEnd;
	StartEnd startEnd;
	SubMenuHostParameterCallback ( StartEnd startEnd ) : startEnd(startEnd) {} 
};

//============================================================================================================
// class CircuidControl:
// Controler Klasse fuer CircuidView gemaess MVC.
//============================================================================================================
class CircuidControl : 
	public EventListener< ToolBoxBtnPressed >,
	public EventListener< OnMouseClick >,
	public EventListener< OnMouseDrag >,
	public EventListener< OnMouseLeave >,
	public EventListener< OnIdle >
{
private:
	//--------------------------------------------------------------------------------------------------------
	GObject::Ptr onMouseObj; // objekt das aktuell unter maus liegt
	//--------------------------------------------------------------------------------------------------------
	bool normCursor;
	//--------------------------------------------------------------------------------------------------------
	PluginCollection::Ptr pluginCollection;
	//--------------------------------------------------------------------------------------------------------
	GRect::Ptr selectionRect;
	//--------------------------------------------------------------------------------------------------------
	menu::CMenu::Ptr contextMenu;
	//--------------------------------------------------------------------------------------------------------
	ViewRelations &view2model; //muss extern sein da relationen bestehen muessen auch wenn editor closed.
	//--------------------------------------------------------------------------------------------------------
	MouseAction **comMActions;
	//--------------------------------------------------------------------------------------------------------
	FrontController *gObjCtrlDirector;
	//--------------------------------------------------------------------------------------------------------
	// Aktuelle Maus Aktion
	MouseAction *currMouseAction;
	//--------------------------------------------------------------------------------------------------------
	void initMouseActions();
	//--------------------------------------------------------------------------------------------------------
	CircuidView *view;
	//--------------------------------------------------------------------------------------------------------
	GSelection::Ptr selection;
	//--------------------------------------------------------------------------------------------------------
	void spanSelectionRect ( const OnMouseDrag &ev );
	//--------------------------------------------------------------------------------------------------------
	void dragView ( const OnMouseDrag &ev );
	//--------------------------------------------------------------------------------------------------------
	void showContextMenu( GObject::Ptr obj, CPoint &p );
	//--------------------------------------------------------------------------------------------------------
	void createVSTPluginDynSubMenu ( menu::MenuEntryList &me );
	//--------------------------------------------------------------------------------------------------------
	void createDynMenuPlugTree ( menu::MenuEntryList &me, const PluginCollection::Folder &folder );
	//--------------------------------------------------------------------------------------------------------
	void getMenuEntryList ( menu::MenuEntryList &me );
protected:
public:
	//========================================================================================================
	// MouseActions:
	// Kontext fuer eine Drag Aktion auf einem Objekt. 
	// Wird durch Toolbox knoepfe ausgewaehlt.
	//========================================================================================================
	enum MouseActions{
		   MOVE, 
		   CONNECT,
		   USE,
		   NUM_MACTIONS
	};
	//--------------------------------------------------------------------------------------------------------
	ViewRelations & getViewRelations(){ return view2model; }
	//--------------------------------------------------------------------------------------------------------
	CircuidControl ( CircuidView *view, ViewRelations &viewRelations );
	//--------------------------------------------------------------------------------------------------------
	virtual ~CircuidControl ();
	//--------------------------------------------------------------------------------------------------------
	void eventHandler ( void *src, const OnMouseClick &ev );
	//--------------------------------------------------------------------------------------------------------
	void eventHandler ( void *src, const OnMouseDrag &ev );
	//--------------------------------------------------------------------------------------------------------
	void vstFolderMenuCallBack ( menu::MenuEntryList &mL, const SubMenuVSTFolderCallback &obj );
	//--------------------------------------------------------------------------------------------------------
	void hostParameterMenuCallBack ( menu::MenuEntryList &mL, const SubMenuHostParameterCallback &obj );
	//--------------------------------------------------------------------------------------------------------
	CircuidView * getView (){ return view; } 
	//--------------------------------------------------------------------------------------------------------
	void initView();
	//--------------------------------------------------------------------------------------------------------
	// Toolbox Knoepfe:
	void eventHandler ( void *scr, const ToolBoxBtnPressed &ev );
	//--------------------------------------------------------------------------------------------------------
	// Context Menu Focuslistener
	void eventHandler ( void *scr, const OnMouseLeave &ev );
	//--------------------------------------------------------------------------------------------------------
	void save ( oArchive &ar );
	//--------------------------------------------------------------------------------------------------------
	void load ( iArchive &ar );
	//--------------------------------------------------------------------------------------------------------
	FrontController & getFrontController() { return *gObjCtrlDirector; }
	//--------------------------------------------------------------------------------------------------------
	// schaut was fuer objekt unter maus sitzt.
	void eventHandler ( void *src, const OnIdle &ev );
};
} // namespace ppiGui

#endif


