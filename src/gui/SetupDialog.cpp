#include "SetupDialog.h"
#include "com/MyString.h"
#include "Resources.h"
#include "com/one4All.h"
#include "PpiEditor.h"
#include "OS_Specific/OS_gui.h"

namespace {
	enum { ROCKER_RESIZE = 10 };
}

namespace ppiGui {
//============================================================================================================
// Klasse SetupDialog
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
SetupDialog::SetupDialog( VSTGUI::CFrame *parent ) : 
	WindowView ( parent, resources->getResourceBitmap( Resources::DLG_SETTINGS_FRAME ) ),
	settings( Settings::getSettings() )
{	
	listBox = new ListBox( VSTGUI::CRect( 0, 0, 461, 209), parent ),
	initControls();
	readFromSetup();
}
//------------------------------------------------------------------------------------------------------------
void SetupDialog::initControls() {
	// ListBox ========--------........
	VSTGUI::CRect r; listBox->getViewSize( r );
	r.offset ( 23, 60 );
	listBox->setViewSize ( r );
	listBox->setMouseableArea ( r );
	
	addView (listBox);
	// Add Dir Button ========--------........
	static const CCoord X = 495;
	static const CCoord Y = 116;
	static const CCoord MARGIN = 50;
	CBitmap *bmp = resources->getResourceBitmap( Resources::DLG_BTN_ADD_DIR );
	VSTGUI::CRect size = VSTGUI::CRect( 0, 0, bmp->getWidth(), bmp->getHeight()/2 );
	size.offset ( X, Y );
	CPoint p;
	size_t c = 0;
	controls[c] = new CMovieButton ( size, this, tAddDir, bmp->getHeight()/2, bmp, p );
	addView ( controls[c++] );
	// chDir Button ========--------........
	bmp = resources->getResourceBitmap( Resources::DLG_BTN_CHDIR );
	size = VSTGUI::CRect( 0, 0, bmp->getWidth(), bmp->getHeight()/2 );
	size.offset ( X, Y + MARGIN );
	controls[c] = new CMovieButton ( size, this, tChDir, bmp->getHeight()/2, bmp, p );
	addView ( controls[c++] );
	// rmDir Button ========--------........
	bmp = resources->getResourceBitmap( Resources::DLG_BTN_REMOVE_DIR );
	size = VSTGUI::CRect( 0, 0, bmp->getWidth(), bmp->getHeight()/2 );
	size.offset ( X, Y + MARGIN*2 );
	controls[c] = new CMovieButton ( size, this, tRmDir, bmp->getHeight()/2, bmp, p );
	addView ( controls[c++] );
	// OK Button ========--------........
	bmp = resources->getResourceBitmap( Resources::DLG_BTN_OK );
	size = VSTGUI::CRect( 0, 0, bmp->getWidth(), bmp->getHeight()/2 );
	size.offset ( 385, 376 );
	controls[c] = new CMovieButton ( size, this, tOk, bmp->getHeight()/2, bmp, p );
	addView ( controls[c++] );
	// CANCEL Button ========--------........
	bmp = resources->getResourceBitmap( Resources::DLG_BTN_CANCEL );
	size = VSTGUI::CRect( 0, 0, bmp->getWidth(), bmp->getHeight()/2 );
	size.offset ( 477, 376 );
	controls[c] = new CMovieButton ( size, this, tCancel, bmp->getHeight()/2, bmp, p );
	addView ( controls[c++] );
	// FastScan ChkBx ========--------........
	bmp = resources->getResourceBitmap( Resources::DLG_CHKBX_FASTSCAN );
	size = VSTGUI::CRect( 0, 0, bmp->getWidth(), bmp->getHeight()/2 );
	size.offset ( 309, 278 );
	controls[c] = new CMovieButton ( size, this, tFastScan, bmp->getHeight()/2, bmp, p );
	addView ( controls[c++] );
	// Add Dir Button ========--------........
	static const CCoord X2 = 30;
	static const CCoord Y2 = 335;
	static const CCoord MARGIN2 = 25;
	// Resize Rocker ========--------........
	bmp = resources->getResourceBitmap( Resources::DLG_RESIZE_ROCKER );
	size = VSTGUI::CRect( 0, 0, bmp->getWidth(), bmp->getHeight()/3 );
	size.offset ( X2, Y2 );
	controls[c] = new CRockerSwitch ( size, this, tSizeRocker, bmp->getHeight()/3, bmp, p );
	addView ( controls[c++] );
	// Resize Rocker ========--------........
	bmp = resources->getResourceBitmap( Resources::DLG_RESIZE_ROCKER_WIDTH );
	size = VSTGUI::CRect( 0, 0, bmp->getWidth(), bmp->getHeight()/3 );
	size.offset ( X2, Y2 +  MARGIN2 );
	controls[c] = new CRockerSwitch ( size, this, tSizeRockerWidth, bmp->getHeight()/3, bmp, p );
	addView ( controls[c++] );
	// Resize Rocker ========--------........
	bmp = resources->getResourceBitmap( Resources::DLG_RESIZE_ROCKER_HEIGHT );
	size = VSTGUI::CRect( 0, 0, bmp->getWidth(), bmp->getHeight()/3 );
	size.offset ( X2, Y2 +  MARGIN2 * 2 );
	controls[c] = new CRockerSwitch ( size, this, tSizeRockerHeight, bmp->getHeight()/3, bmp, p );
	addView ( controls[c++] );
	// Scan Now ========--------........
	bmp = resources->getResourceBitmap( Resources::DLG_SCAN_NOW );
	size = VSTGUI::CRect( 0, 0, bmp->getWidth(), bmp->getHeight()/2 );
	size.offset ( 413, 280 );
	controls[c] = new CMovieButton ( size, this, tScanNow, bmp->getHeight()/2, bmp, p );
	addView ( controls[c++] );
}
//------------------------------------------------------------------------------------------------------------
void SetupDialog::_valueChanged ( CDrawContext *cD, CControl *pC ) {
	float firstValue = pC->getValue();
	long tag = pC->getTag();
	switch ( tag ) { // checkbox(es)
		case tFastScan:
			return;
	}
	// buttons
	while ( cD->waitDrag() ) {}
	pC->setValue(0.0f);
	pC->setDirty();
	switch ( tag ) {
		case tAddDir: 
			addDir();
			break;
		case tChDir: 
			chDir();
			break;
		case tRmDir: 
			rmDir();
			break;
		case tCancel: 
			cancel();
			break;
		case tOk:
			ok();
			break;
		case tScanNow:
			scanNow();
			break;
		case tSizeRocker:
		case tSizeRockerWidth:
		case tSizeRockerHeight:
			sizeRocker( firstValue, tag );
			break;
	}
}
//------------------------------------------------------------------------------------------------------------
void SetupDialog::sizeRocker ( float value, long tag ) {
	if ( value == 0 ) return;
	VSTGUI::CRect mainSz; getFrame()->getViewSize ( mainSz );
	int w,h;
	w = h = 0;
	int m = ( value > 0.0 ) ? -1 : 1;
	switch ( tag ) {
		case tSizeRockerWidth:
			w = ROCKER_RESIZE * m;
			break;
		case tSizeRockerHeight:
			h = ROCKER_RESIZE * m;
			break;
		case tSizeRocker:
			h = w = ROCKER_RESIZE * m;
			break;

		default: return;
	}
	
	mainSz.inset (w,h);
	
	settings->setWindowWidth ( mainSz.getWidth() );
	settings->setWindowHeight ( mainSz.getHeight() );
	getFrame()->setSize ( mainSz.getWidth(), mainSz.getHeight() );
}
//------------------------------------------------------------------------------------------------------------
void SetupDialog::writeInSetup() {
	settings->pluginDirectories.clear();
	// directories
	for ( int i=0; i<listBox->getNumEntries(); ++i ) {
		settings->pluginDirectories.insert( listBox->getString(i) );
	}
	settings->fastScan = getControl( tFastScan )->getValue() == 1.0f ? true : false;
	settings->saveConfigFile();
}
//------------------------------------------------------------------------------------------------------------
void SetupDialog::resetSetup() {
	settings->reloadConfigFile();
	readFromSetup();
	getFrame()->setSize ( settings->getWindowWidth(), settings->getWindowHeight() );
}
//------------------------------------------------------------------------------------------------------------
void SetupDialog::readFromSetup() {
	listBox->clear();
	const Settings::PathnameSet &dirs = settings->getPluginDirectoryList();
	Settings::PathnameSet::const_iterator it = dirs.begin();
	for ( ; it!=dirs.end(); ++it ) {
		listBox->addString ( *it );
	}
	getControl( tFastScan )->setValue( settings->isFastScan()==true ? true : false );
}
//------------------------------------------------------------------------------------------------------------
void SetupDialog::addDir() {
	AudioEffectX *aEff = static_cast<AudioEffectX*>(((AEffGUIEditor*)getFrame()->getEditor())->getEffect() );
	string dir = selectDirectory ( "select VST-Plugin Directory:", "", aEff );

	if ( dir.length() == 0 ) return;

	listBox->addString ( dir );
}
//------------------------------------------------------------------------------------------------------------
void SetupDialog::chDir() {
	int index = listBox->getSelectedIndex();
	if ( index<0 ) return;
	AudioEffectX *aEff = static_cast<AudioEffectX*>(((AEffGUIEditor*)getFrame()->getEditor())->getEffect() );
	string strSel = listBox->getString ( (size_t) index );
	string dir = selectDirectory ( "select VST-Plugin Directory:", strSel, aEff );
	
	if ( dir.length() == 0 ) return;

	listBox->changeEntry ( index, dir );
}
//------------------------------------------------------------------------------------------------------------
void SetupDialog::rmDir() {
	int index = listBox->getSelectedIndex();
	if ( index<0 ) return;
	listBox->removeEntry ( (size_t)index );
	listBox->setDirty();
}
//------------------------------------------------------------------------------------------------------------
void SetupDialog::ok() {
	writeInSetup();
	EventSender<OnOk>::notifyEventListeners( this, OnOk(tOk) );
}
//------------------------------------------------------------------------------------------------------------
void SetupDialog::scanNow() {
	writeInSetup();
	EventSender<OnOk>::notifyEventListeners( this, OnOk(tScanNow) );
}
//------------------------------------------------------------------------------------------------------------
void SetupDialog::cancel() {
	resetSetup();
	EventSender<OnClose>::notifyEventListeners(this, OnClose());
}
//------------------------------------------------------------------------------------------------------------
SetupDialog::~SetupDialog() {
	removeView (listBox, false); 
	delete listBox;
}
//============================================================================================================
// Klasse ScanningDialog:
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
ScanningDialog::ScanningDialog( VSTGUI::CFrame *parent ) : 
	WindowView ( parent, resources->getResourceBitmap( Resources::DLG_SCANNING_FRAME ) )
{	
	listBox = new ListBox ( VSTGUI::CRect( 0, 0, 578, 458 ), parent );
	initControls();
}
//------------------------------------------------------------------------------------------------------------
void ScanningDialog::initControls() {
	// ListBox ========--------........
	VSTGUI::CRect r; listBox->getViewSize( r );
	r.offset ( 26, 62 );
	listBox->setViewSize ( r );
	listBox->setMouseableArea ( r );
	addView (listBox);
	// OK Button ========--------........
	CBitmap *bmp = resources->getResourceBitmap( Resources::DLG_BTN_OK );
	VSTGUI::CRect size( 0, 0, bmp->getWidth(), bmp->getHeight()/2 );
	size.offset ( 285, 538 );
	int c=0;
	CPoint p;
	controls[c] = new CMovieButton ( size, this, tOk, bmp->getHeight()/2, bmp, p );
	addView ( controls[c++] );
}
//------------------------------------------------------------------------------------------------------------
void ScanningDialog::_valueChanged ( CDrawContext *cD, CControl *pC ) {
	while ( cD->waitDrag() ) {}
	pC->setValue(0.0f);
	pC->setDirty();
	switch ( pC->getTag() ) {
		case tOk: 
			ok();
			break;
	}
}
//------------------------------------------------------------------------------------------------------------
void ScanningDialog::eventHandler(void *src, const com::OnLoadFile &ev) {
	listBox->addString ( ev.filename );
	listBox->setDirty();
	((AEffGUIEditor*) getFrame()->getEditor() )->doIdleStuff();
}
//------------------------------------------------------------------------------------------------------------
void ScanningDialog::eventHandler(void *src, const com::OnFileLoaded &ev) {
	if ( ev.info.access == PluginInfo::NOT_CHECKED ) return;
	string info(" -> ");
	info += ( ev.info.access == PluginInfo::SUCCEED ) ? "OK!" : "FAILED!";
	listBox->changeEntry ( listBox->getNumEntries() - 1, ev.filename + info );
}
//------------------------------------------------------------------------------------------------------------
void ScanningDialog::ok() {
	EventSender<OnOk>::notifyEventListeners( this, OnOk(0) );
}
//------------------------------------------------------------------------------------------------------------
ScanningDialog::~ScanningDialog() {
	removeView (listBox, false);
	delete listBox;
}

//============================================================================================================
// Klasse SetupCtrl:
//============================================================================================================
SetupCtrl::SetupCtrl ( CFrame *frame ) : 
	scanStart ( tNone ), scanLock(false)
{
	dlgSetup = new SetupDialog ( frame );
	dlgScanning = new ScanningDialog ( frame );
	
	modView = new ModalView ( VSTGUI::CRect ( 0, 0, SETTINGS->getWindowWidth(), 
		                      SETTINGS->getWindowHeight() ), frame );

	dlgSetup->EventSender<OnClose>::addEventListener( this );
	dlgSetup->EventSender<OnOk>::addEventListener( this );

	dlgScanning->EventSender<OnClose>::addEventListener( this );
	dlgScanning->EventSender<OnOk>::addEventListener( this );

}
//------------------------------------------------------------------------------------------------------------
void SetupCtrl::eventHandler(void *src, const ppiGui::OnClose &ev) {
	WindowView *win = static_cast<WindowView*>(src);
	modView->removeWindowView(win);
	if ( scanStart == tScanNow && src == dlgScanning ) return;
	close();
}
//------------------------------------------------------------------------------------------------------------
void SetupCtrl::eventHandler(void *src, const ppiGui::OnOk &ev) {
	if ( src == dlgSetup ) setupOk(ev.tag);
	if ( src == dlgScanning ) scanningOk();
}
//------------------------------------------------------------------------------------------------------------
void SetupCtrl::show() {
	modView->addWindowView ( dlgSetup );
	modView->show();
}
//------------------------------------------------------------------------------------------------------------
void SetupCtrl::setupOk( long tag ) {
	if ( tag == SetupDialog::tOk ) {
		modView->removeWindowView ( dlgSetup );
		scanStart = tOk;
	}
	else scanStart = tScanNow;
	scan();
}
//------------------------------------------------------------------------------------------------------------
void SetupCtrl::scan() {
	if (scanLock) return; // avoid double execution ( happens e.g when doubleclicked on fastscan )
	scanLock = true;
	PluginCollection::Ptr pC = PluginCollection::getPluginCollection();
	if ( pC->isAllScanned() && scanStart != tScanNow ) {
		eventHandler ( dlgScanning, OnClose() );
		scanLock = false;
		return;
	}
	PpiEditor *ed = static_cast<PpiEditor*>( dlgScanning->getFrame()->getEditor() );
	// add window
	modView->addWindowView ( dlgScanning );
	modView->setDirty();
	pC->EventSender<OnLoadFile>::addEventListener ( dlgScanning );
	pC->EventSender<OnFileLoaded>::addEventListener ( dlgScanning );
	pC->EventSender<ScanFinished>::addEventListener ( this );
	pC->EventSender<CleaningUpDataBase>::addEventListener ( this );
	
	ed->addExtraTimerCmd ( SystemCommand::Ptr( new CmdUpdatePluginCollection( pC, ed->getGraph() ) ) );
}
//------------------------------------------------------------------------------------------------------------
void SetupCtrl::eventHandler(void *src, const com::ScanFinished &ev) {

	PluginCollection::Ptr pC = PluginCollection::getPluginCollection();
	pC->EventSender<OnLoadFile>::removeEventListener ( dlgScanning ); // wichtig!
	pC->EventSender<OnFileLoaded>::removeEventListener ( dlgScanning );
	pC->EventSender<ScanFinished>::removeEventListener ( this );
	pC->EventSender<CleaningUpDataBase>::removeEventListener ( this );
	ListBox &lB = dlgScanning->getListBox();
	// suma sumarum
	lB.addString("=========================================================");
	lB.addString( MyString( pC->getNumSucceed() )     + " succeed." );
	lB.addString( MyString( pC->getNumFailed() )      + " failed." );
	lB.addString( MyString( pC->getNumNotChecked() )  + " not checked." );
	lB.addString("=========================================================");
	lB.addString( "Don't forget to rescan when folder content changed!" );

	
}
//------------------------------------------------------------------------------------------------------------
void SetupCtrl::eventHandler(void *src, const com::CleaningUpDataBase &ev) {
	ListBox &lB = dlgScanning->getListBox();
	lB.addString("cleaning up database ...");
}
//------------------------------------------------------------------------------------------------------------
void SetupCtrl::close() {
	EventSender<OnClose>::notifyEventListeners( this, OnClose() );
	modView->hide();
}
//------------------------------------------------------------------------------------------------------------
SetupCtrl::~SetupCtrl() {
	PluginCollection::Ptr pC = PluginCollection::getPluginCollection();
	pC->EventSender<OnLoadFile>::removeEventListener ( dlgScanning ); // wichtig!
	pC->EventSender<OnFileLoaded>::removeEventListener ( dlgScanning );
	pC->EventSender<ScanFinished>::removeEventListener ( this );
	pC->EventSender<CleaningUpDataBase>::removeEventListener ( this );
	
	if ( pC->isScanning() ) {
		pC->stopScanning();
		EventSender<ScanInterrupted>::notifyEventListeners ( this, ScanInterrupted() );
	}
	
	modView->removeView(dlgSetup, false);
	modView->removeView(dlgScanning, false);
	
	modView->hide();

	delete dlgSetup;
	delete dlgScanning;
	delete modView;
}
//------------------------------------------------------------------------------------------------------------
void SetupCtrl::scanningOk() {
	dlgScanning->getListBox().clear();
	scanLock = false;
	if ( scanStart == tScanNow ) {
		modView->removeWindowView ( dlgScanning );
		return;
	}
	eventHandler ( dlgScanning, OnClose() );
}
} // ppiGui