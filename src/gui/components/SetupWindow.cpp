/*
 * SetupWindow.cpp
 *
 *  Created on: Thu Oct 25 10:16:26 2012
 *      Author: Johannes Unger
 */

#include "SetupWindow.hpp"
#include <gui/components/ui/FrxLookAndFeel.hpp>
#include <sambag/disco/components/ui/ALookAndFeel.hpp>
#include <sambag/disco/components/ui/UIManager.hpp>
#include <sambag/disco/components/BorderLayout.hpp>
#include <sambag/disco/components/GridLayout.hpp>
#include <sambag/disco/components/BoxLayout.hpp>
#include <sambag/disco/components/Panel.hpp>
#include <sambag/disco/components/ScrollPane.hpp>
#include <sambag/disco/components/Button.hpp>
#include <sambag/disco/components/TitledBorder.hpp>
#include <sambag/disco/components/SolidBorder.hpp>
#include <sambag/disco/components/Timer.hpp>
#include <sambag/disco/svg/graphicElements/Style.hpp>
#include <sambag/com/ArithmeticWrapper.hpp>
#include <boost/foreach.hpp>
#include <sambag/com/Thread.hpp>
#include <queue>
#include <boost/unordered_map.hpp>
#include <com/one4All.h>
#include <sambag/disco/components/SvgComponent.hpp>
#include <sambag/com/Filesystem.hpp>

namespace frx { namespace gui { namespace components {
namespace {
    template <class C>
    struct _StyleVis : public sc::IWalkerVisitor {
        C &c;
        _StyleVis(C &c) : c(c) {}
        virtual bool changeDirectory (const sc::Location & path){
            return true;
        }
        virtual void file ( const sc::Location & file ) {
            // search for preview.svg
            if (file.filename()=="preview.svg") {
                c.push_back(file.parent_path().string());
            }
        }
    };
    template <class C>
    void _scanForStyles(const std::string &root, C &out) {
        _StyleVis<C> vis(out);
        sc::dirWalker(root, vis);
    }
}

///////////////////////////////////////////////////////////////////////////////
//=============================================================================
//  Class ScanningDialog
//=============================================================================
class ScanningDialog : public sdc::FramedWindow {
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<ScanningDialog> Ptr;
	//-------------------------------------------------------------------------
	typedef sdc::FramedWindow Super;
protected:
	//-------------------------------------------------------------------------
	void syncEntries();
	//-------------------------------------------------------------------------
	sambag::com::RecursiveMutex mutex;
	//-------------------------------------------------------------------------
	// for future impl. with own listcell renderer:
	// use SetupCtrl FileSatus instead of a pain string and prepare 
	// each element with it.
	typedef std::pair<std::string, std::string> FileAndStatus;
	//-------------------------------------------------------------------------
	std::queue<FileAndStatus> tmpEntries;
	//-------------------------------------------------------------------------
	typedef boost::unordered_map<std::string, size_t> File2Listindex;
	//-------------------------------------------------------------------------
	File2Listindex file2listindex;
	//-------------------------------------------------------------------------
	FrxCircuidViewPtr view;
	//-------------------------------------------------------------------------
	sdc::ButtonPtr btnOk, btnCancel;
	//-------------------------------------------------------------------------
	ScanningDialog(sdc::Window::Ptr parent=sdc::Window::Ptr()) 
		: sdc::FramedWindow (parent)
	{
	}
	//-------------------------------------------------------------------------
	typedef void (ScanningDialog::*BtnActionFunc)
		(void *, const sdc::events::ActionEvent&);
	//-------------------------------------------------------------------------
	sdc::ButtonPtr createBtn(BtnActionFunc f, const std::string &txt);
	//-------------------------------------------------------------------------
	virtual void postConstructor();
	//-------------------------------------------------------------------------
	sdc::AContainerPtr createListPane();
	//-------------------------------------------------------------------------
	sdc::AContainerPtr createMainBtnPane();
	//-------------------------------------------------------------------------
	sdc::StringList::Ptr list;
	//-------------------------------------------------------------------------
	sdc::ScrollPane::Ptr dirListScrollPane;
	//-------------------------------------------------------------------------
	SetupCtrl::Ptr ctrl;
	//-------------------------------------------------------------------------
	sdc::Timer::Ptr timer;
	//-------------------------------------------------------------------------
	sambag::com::ArithmeticWrapper<bool> scanFailed;
	//-------------------------------------------------------------------------
	void __onScanCompleted(int succeed, int failed, int skipped);
    //-------------------------------------------------------------------------
    void startRefreshingTimerIfNecessary();
    //-------------------------------------------------------------------------
    void stopRefreshingTimer();
public:
	//-------------------------------------------------------------------------
	static Ptr create(sdc::Window::Ptr parent = sdc::Window::Ptr()) { 																		
		Ptr neu(new ScanningDialog(parent));								
		neu->self = neu;  
		neu->postConstructor();                                            
		neu->initWindow();
		return neu;															
	}
    //-------------------------------------------------------------------------
	void startScan(SetupCtrl::Ptr ctrl);
    //-------------------------------------------------------------------------
    void onClose(void *, const sdc::OnCloseEvent &ev);
	//-------------------------------------------------------------------------
	void onBtnOk(void *, const sdc::events::ActionEvent& ev);
	//-------------------------------------------------------------------------
	void onBtnCancel(void *, const sdc::events::ActionEvent& ev);
	//-------------------------------------------------------------------------
	void onFileEvent(const std::string &file, SetupCtrl::FileStatus fst);
	//-------------------------------------------------------------------------
	void onScanCompleted(int succeed, int failed, int skipped);
	//-------------------------------------------------------------------------
	void onScanFailed(const std::string msg);
	//-------------------------------------------------------------------------
	/**
	 * redraw timer
	 */
	void onRefresh(void *, const sdc::TimerEvent &ev);
};
//-----------------------------------------------------------------------------
void ScanningDialog::postConstructor() {
	Super::postConstructor();
	sdc::ui::UIManager::instance().installLookAndFeel(getRootPane(),
		ui::FrxLookAndFeel::create()
	);
	getContentPane()->add(createListPane(), sdc::BorderLayout::CENTER, APPEND);
	getContentPane()->add(createMainBtnPane(), sdc::BorderLayout::SOUTH, APPEND);
	setTitle("Scan results:");
	windowImpl->setFlag(sdc::WindowFlags::WND_NO_SYSTEM_MENU, true);
	windowImpl->setFlag(sdc::WindowFlags::WND_ALWAYS_ON_TOP, true);
}
//-----------------------------------------------------------------------------
sdc::AContainerPtr ScanningDialog::createListPane() {
	list = sdc::StringList::create();
	dirListScrollPane = sdc::ScrollPane::create(list);
	dirListScrollPane->setPreferredSize(sd::Dimension(730., 300.));
	return dirListScrollPane;
}
//-----------------------------------------------------------------------------
sdc::AContainerPtr ScanningDialog::createMainBtnPane() {
	sdc::Panel::Ptr mainBtnPane = sdc::Panel::create();
	btnOk = createBtn(&ScanningDialog::onBtnOk, "OK");
	mainBtnPane->add(btnOk);
	
	btnCancel = createBtn(&ScanningDialog::onBtnCancel, "Cancel");
	mainBtnPane->add(btnCancel);
	return mainBtnPane;
}
//-----------------------------------------------------------------------------
sdc::ButtonPtr ScanningDialog::createBtn(BtnActionFunc f, const std::string &txt) 
{
	sdc::Button::Ptr res = sdc::Button::create();
	res->setText(txt);
	res->sce::EventSender<sdc::events::ActionEvent>::addEventListener(
		boost::bind(f, this, _1, _2)
	);
	return res;
}
//-----------------------------------------------------------------------------
void ScanningDialog::onBtnOk(void *, const sdc::events::ActionEvent &ev) {
	close();
}
//-----------------------------------------------------------------------------
void ScanningDialog::onBtnCancel(void *, const sdc::events::ActionEvent &ev) 
{
	if (ctrl && !scanFailed) {
		ctrl->stopScanning();
		ctrl->joinScan();
	}
	close();
}
//-----------------------------------------------------------------------------
void ScanningDialog::startRefreshingTimerIfNecessary() {
    if (timer) {
        return;
    }
    timer = sdc::Timer::create(100);
	timer->sce::EventSender<sdc::TimerEvent>::addTrackedEventListener(
		boost::bind(&ScanningDialog::onRefresh, this, _1, _2),
		getPtr()
	);
	timer->setNumRepetitions(-1);
	timer->start();
}
//-----------------------------------------------------------------------------
void ScanningDialog::stopRefreshingTimer() {
    if (!timer) {
        return;
    }
    timer->stop();
}
//-----------------------------------------------------------------------------
void ScanningDialog::startScan(SetupCtrl::Ptr ctrl) {
	this->ctrl = ctrl;
	if (!ctrl)
		return;
	btnOk->setEnabled(false);
	ctrl->startScan(
		boost::bind(&ScanningDialog::onFileEvent, this, _1, _2),
		boost::bind(&ScanningDialog::onScanCompleted, this, _1, _2, _3),
		boost::bind(&ScanningDialog::onScanFailed, this, _1)
	);
}
//-----------------------------------------------------------------------------
void ScanningDialog::onFileEvent(const std::string &file, SetupCtrl::FileStatus fst) 
{
    startRefreshingTimerIfNecessary();
	SAMBAG_BEGIN_SYNCHRONIZED(mutex)
		/**
		 * do not invoke any redrawing stuff inhere: concurrency prolems!
		 */
		std::string stStr;
		switch (fst) {
			case SetupCtrl::Succeed:
				stStr = "SUCCEED"; break;
			case SetupCtrl::Failed:
				stStr = "FAILED"; break;
			case SetupCtrl::Skipped:
				stStr = "SKIPPED"; break;
			default:
				break;
		}
		tmpEntries.push(FileAndStatus(file, stStr));
	SAMBAG_END_SYNCHRONIZED
}
//-----------------------------------------------------------------------------
void ScanningDialog::__onScanCompleted(int succeed, int failed, int skipped) {
	btnOk->setEnabled(true);
	btnCancel->setEnabled(false);
	syncEntries();
	list->addElement("=====================================================");
	list->addElement("Scan complete.");
	list->addElement("Succeed: " + sambag::com::toString(succeed) + ".");
	list->addElement("Failed: " + sambag::com::toString(failed) + ".");
	list->addElement("Skipped: " + sambag::com::toString(skipped) + ".");
	list->addElement("Don't forget to rescan when folder content changed!");
	list->addElement("=====================================================");

	dirListScrollPane->revalidate();
	int i = list->ListModel::getSize() - 1;
	list->ensureIndexIsVisible(i);
	list->revalidate();
	list->redraw();
}
//-----------------------------------------------------------------------------
void ScanningDialog::onScanCompleted(int succeed, int failed, int skipped) {
	stopRefreshingTimer();
    
	sdc::Timer::Ptr t = sdc::Timer::create(50);
	t->sce::EventSender<sdc::TimerEvent>::addTrackedEventListener(
		boost::bind(&ScanningDialog::__onScanCompleted, 
				this,  
				succeed, 
				failed, 
				skipped),
		getPtr()
	);
    t->setNumRepetitions(0);
	t->start();
}
//-----------------------------------------------------------------------------
void ScanningDialog::onScanFailed(const std::string msg) {
	scanFailed = true;
	timer->stop();
	list->addElement("Scan Failed:");
	list->addElement(msg);
	list->revalidate();
	list->redraw();
}
//-----------------------------------------------------------------------------
void ScanningDialog::syncEntries() {
	SAMBAG_BEGIN_SYNCHRONIZED(mutex)
		while(!tmpEntries.empty()) {
			const std::string &file = tmpEntries.front().first;
			const std::string &status = tmpEntries.front().second;
			File2Listindex::const_iterator it =
				file2listindex.find(file);
			if (it == file2listindex.end()) {
				list->addElement(file);
				file2listindex[file] = list->ListModel::getSize() - 1;
			} else {
				list->set(it->second, file + "->" + status);
			}
			tmpEntries.pop();
		}
	SAMBAG_END_SYNCHRONIZED
	dirListScrollPane->revalidate();
	int i = list->ListModel::getSize() - 1;
	list->ensureIndexIsVisible(i);
	list->revalidate();
	list->redraw();
}
//-----------------------------------------------------------------------------
void ScanningDialog::onRefresh(void *, const sdc::TimerEvent &ev) {
	syncEntries();
}
//=============================================================================
// SetupWindow::ResizeBtnHandler
//=============================================================================
namespace {
	enum ResizeDirection{EdPlusW, EdMinusW, EdPlusH, EdMinusH};
}
struct SetupWindow::ResizeBtnHandler {
	typedef boost::shared_ptr<ResizeBtnHandler> Ptr;
	static Ptr create(SetupWindow *host);
	void registerBtn(sdc::AComponentPtr c, ResizeDirection dir);
	void onMouse(void *src, const sdc::events::MouseEvent &ev, ResizeDirection dir);
	void mousePressed(const sdc::events::MouseEvent &ev);
	void mouseReleased(const sdc::events::MouseEvent &ev);
	void onTimer(void *src, const sdc::TimerEvent &ev);
	void performResize(ResizeDirection dir, int ammount);
	ResizeDirection currDir;
	SetupWindow *host;
	sdc::Timer::Ptr timer;
};
///////////////////////////////////////////////////////////////////////////////
SetupWindow::ResizeBtnHandler::Ptr 
SetupWindow::ResizeBtnHandler::create(SetupWindow *host) 
{
	Ptr res(new ResizeBtnHandler());
	res->host = host;
	res->timer = sdc::Timer::create(100);
	res->timer->setInitialDelay(1000);
	res->timer->sce::EventSender<sdc::TimerEvent>::addTrackedEventListener(
		boost::bind(&ResizeBtnHandler::onTimer, res.get(), _1, _2),
		res
	);
	res->timer->setNumRepetitions(-1);
	return res;
}
//-----------------------------------------------------------------------------
void SetupWindow::ResizeBtnHandler::
registerBtn(sdc::AComponentPtr c, ResizeDirection dir)
{
	c->sce::EventSender<sdc::events::MouseEvent>::addEventListener(
		boost::bind(&ResizeBtnHandler::onMouse, this, _1, _2, dir)
	);
}
//-----------------------------------------------------------------------------
void SetupWindow::ResizeBtnHandler::
onMouse(void *src, const sdc::events::MouseEvent &ev, ResizeDirection dir)
{
	using namespace sdc::events;
	currDir = dir;
	enum { Filter = MouseEvent::DISCO_MOUSE_PRESSED | 
		MouseEvent::DISCO_MOUSE_RELEASED 
	};
	MouseEventSwitch<Filter>::delegate(ev, *this);
}
//-----------------------------------------------------------------------------
void SetupWindow::ResizeBtnHandler::
mousePressed(const sdc::events::MouseEvent &ev)
{
	performResize(currDir, 5);
	timer->start();
}
//-----------------------------------------------------------------------------
void SetupWindow::ResizeBtnHandler::
mouseReleased(const sdc::events::MouseEvent &ev)
{
	timer->stop();
}
//-----------------------------------------------------------------------------
void SetupWindow::ResizeBtnHandler::
onTimer(void *src, const sdc::TimerEvent &ev)
{
	performResize(currDir, 5);
}
//-----------------------------------------------------------------------------
void SetupWindow::ResizeBtnHandler::performResize(ResizeDirection dir, int ammount) 
{
	SetupCtrl::Ptr ctrl = host->getCtrl();
	if (!ctrl)
		return;
	sambag::disco::Dimension size = ctrl->getEditorSize();
	if (size==NULL_DIMENSION) {
		return;
	}
	switch (dir) {
		case EdPlusW:
			size.width( size.width() + ammount);
			break;
		case EdMinusW:
			size.width( size.width() - ammount);
			break;
		case EdPlusH:
			size.height( size.height() + ammount);
			break;
		case EdMinusH:
			size.height( size.height() - ammount);
			break;
	}
	ctrl->setEditorSize(size);
}
//=============================================================================
//  Class SetupWindow
//=============================================================================
//-----------------------------------------------------------------------------
void SetupWindow::setCtrl(SetupCtrl::Ptr ctrl) {
	this->ctrl = ctrl;
	setTitle("VSTForx-" + ctrl->getStringValue("version") + " Setup");
	updateSettings();
}
//-----------------------------------------------------------------------------
void SetupWindow::updateSettings() {
	if (!ctrl)
		return;
	std::list<std::string> dirs;
	ctrl->getPluginFolders(dirs);
	BOOST_FOREACH(const std::string &str, dirs) {
		dirList->addElement(str);
	}
	chkbxFS->setButtonSelected(ctrl->getBooleanValue("fastScan"));
	revalidate();
	redraw();
}
//-----------------------------------------------------------------------------
void SetupWindow::saveSettings() {
	if (!ctrl)
		return;
	// directories already at place
	// fastscan
    ctrl->setBooleanValue("fastScan", chkbxFS->isButtonSelected());
	// window size
	sambag::disco::Dimension size = ctrl->getEditorSize();
	if (size!=NULL_DIMENSION) {
		ctrl->setIntegerValue("editorWidth", (int)size.width());
		ctrl->setIntegerValue("editorHeight", (int)size.height());
	}
    // look and feel
    if (styleList) {
        const std::string *sel = styleList->getSelectedValue();
        if (!sel || sel->empty()) {
            return;
        }
        if (*sel != ::com::getSettings().getStyle()) {
            ::com::osMessageBox (
                "Look And Feel changed!", std::string("you need to reopen the editor."), ::com::MSG_HINT
            );
            ctrl->setStyle(*sel);
        }
    }
	ctrl->saveSettings();
}
//-----------------------------------------------------------------------------
void SetupWindow::cancelSettings() {
	if (!ctrl)
		return;
	using namespace sambag::disco;
	// window size
	Dimension size;
	size.width( (Coordinate)ctrl->getIntegerValue("editorWidth") );
	size.height( (Coordinate)ctrl->getIntegerValue("editorHeight") );
	ctrl->setEditorSize(size);
	
}
//-----------------------------------------------------------------------------
SetupCtrl::Ptr SetupWindow::getCtrl() const {
	return ctrl;
}
//-----------------------------------------------------------------------------
void SetupWindow::postConstructor() {

	Super::postConstructor();
	sdc::ui::UIManager::instance().installLookAndFeel(getRootPane(),
		ui::FrxLookAndFeel::create()
	);
	sdsg::Style style = sdsg::Style::DEFAULT_STYLE;
	sdcu::UIManager::instance().getProperty("SetupWindow.style", style);
	getContentPane()->setBackground(style.fillPattern());
	getContentPane()->setForeground(style.strokePattern());
	getContentPane()->setFont(style.font());
	getContentPane()->add(createSetupPane(), sdc::BorderLayout::CENTER, APPEND);
	getContentPane()->add(createMainBtnPane(), sdc::BorderLayout::SOUTH, APPEND);
	setWindowSize(sd::Dimension(623., 462.));
	windowImpl->setFlag(sdc::WindowFlags::WND_RESIZEABLE, true);
}
//-----------------------------------------------------------------------------
SetupWindow::~SetupWindow() {
}
//-----------------------------------------------------------------------------
sdc::AContainerPtr SetupWindow::createSetupPane() {
	sdc::Panel::Ptr pane = sdc::Panel::create();
	pane->setOpaque(false);
	pane->add(createDirListPane());
	pane->add(createDirListBtnPane());
	pane->add(createMiscPane());
	return pane;
}
//-----------------------------------------------------------------------------
sdc::AContainerPtr SetupWindow::createDirListPane() {
	dirListPane = createDirListScrollPane();
	sdc::Panel::Ptr pane = sdc::Panel::create();
	pane->add(dirListPane);
	pane->setBorder(sdc::SolidBorder::create());
	return pane;
}	
//-----------------------------------------------------------------------------
void SetupWindow::onFastScanSelected(void *, const sdc::events::ActionEvent &ev)
{
}
//-----------------------------------------------------------------------------
sdc::AContainerPtr SetupWindow::createMiscPane() {
	sdc::Panel::Ptr pane = sdc::Panel::create();
	pane->add( createWindowSizePane() );
    pane->add( createStylePane() );
	pane->setBackground(getContentPane()->getBackgroundPattern());
	return pane;
}
//-----------------------------------------------------------------------------
void SetupWindow::updatePreview(const std::string &path) {
    if (!preview) {
        return;
    }
    sdc::SvgComponent::Ptr img =
        boost::dynamic_pointer_cast<sdc::SvgComponent>(preview->getComponent(0));
    if(!img) {
        return;
    }
    std::string file = path;
    if (path.empty()) {
        file = com::getSettings().getStylePath();
    }
    try {
        file = file + "/preview.svg";
        img->setSvgFilename(file);
        img->setStretchToFit(true);
        img->setSize(sd::Dimension(80,80));
        img->setPreferredSize(sd::Dimension(80,80));
        img->setMaximumSize(sd::Dimension(80,80));
        img->revalidate();
        img->redraw();
    } catch (const std::exception &ex) {
        SAMBAG_LOG_ERR<<"updatePreview("<<file<<"): "<<ex.what();
    } catch (...) {
        SAMBAG_LOG_ERR<<"updatePreview("<<file<<"): failed";
    }
    
}
//-----------------------------------------------------------------------------
void SetupWindow::onStyleChanged() {
    if (!styleList) {
        return;
    }
    const std::string *sel = styleList->getSelectedValue();
    if (!sel) {
        return;
    }
    updatePreview(com::getSettings().getStyleRootPath() + "/" + *sel);
}
//-----------------------------------------------------------------------------
sdc::AContainerPtr SetupWindow::createStylePane() {
    sdc::Panel::Ptr pane = sdc::Panel::create();
    sdc::TitledBorder::Ptr border = sdc::TitledBorder::create();
    pane->setName("Look&Feel");
    pane->setBorder(border);
    // create list
    styleList = sdc::StringList::create();
    styleList->sce::EventSender<sdce::ListSelectionEvent>::addEventListener(
        boost::bind(&SetupWindow::onStyleChanged, this)
    );
	styleListScrollPane = sdc::ScrollPane::create(styleList);
	styleListScrollPane->setPreferredSize(sd::Dimension(183, 83));
    pane->add(styleListScrollPane);
    // fill list
    std::vector<std::string> styles;
    std::string root = com::getSettings().getStyleRootPath();
    std::string curr = com::getSettings().getStyle();
    _scanForStyles(root, styles);
    int c=0, sel=0;
    BOOST_FOREACH(const std::string &x, styles) {
        size_t e = x.find(root); // remove root
        if (e==std::string::npos) {
            continue;
        }
        e+=root.length()+1;
        if (e>=x.length()) {
            continue;
        }
        std::string entry(x.begin()+e, x.end());
        styleList->addElement(entry);
        if (entry==curr) {
            sel=c;
        }
        c++;
    }
    
    // create preview
    preview = sdc::Panel::create();
    sdc::SvgComponent::Ptr img = sdc::SvgComponent::create();
    preview->add(img);
    pane->add(preview);
    styleList->setSelectedIndex(sel);
    return pane;
}
//-----------------------------------------------------------------------------
sdc::AContainerPtr SetupWindow::createWindowSizePane() {
	sdc::Panel::Ptr ueberpane = sdc::Panel::create();
	ueberpane->setOpaque(false);
	sdc::Panel::Ptr pane = sdc::Panel::create();
	pane->setOpaque(false);
	sdc::Panel::Ptr labelpane = sdc::Panel::create();
	sdc::Panel::Ptr btnpane = sdc::Panel::create();
	rszBtnHandler = ResizeBtnHandler::create(this);
	
	sdc::TitledBorder::Ptr border = sdc::TitledBorder::create();
	ueberpane->setName("Window Size");
	ueberpane->setBorder(border);
	ueberpane->add(pane);
	
	btnpane->setLayout(sdc::GridLayout::create(0,2,2,5));
    btnpane->setBackground(getContentPane()->getBackground());
	// ed-w
	sdc::Button::Ptr btn = sdc::Button::create();
	rszBtnHandler->registerBtn(btn, EdMinusW);
	btn->setText("-");
    btn->putClientProperty("Button.cornerRadius", sd::Coordinate(0.));
	btnpane->add(btn);

	// ed+w
	btn = sdc::Button::create();
	btn->setText("+");
    btn->putClientProperty("Button.cornerRadius", sd::Coordinate(0.));
	rszBtnHandler->registerBtn(btn, EdPlusW);
	btnpane->add(btn);
	
	// ed-h
	btn = sdc::Button::create();
	btn->setText("-");
    btn->putClientProperty("Button.cornerRadius", sd::Coordinate(0.));
	rszBtnHandler->registerBtn(btn, EdMinusH);
	btnpane->add(btn);

	// ed+h
	btn = sdc::Button::create();
	btn->setText("+");
    btn->putClientProperty("Button.cornerRadius", sd::Coordinate(0.));
	rszBtnHandler->registerBtn(btn, EdPlusH);
	btnpane->add(btn);
	// label
	sdc::Label::Ptr label = sdc::Label::create();
	labelpane->setLayout(sdc::GridLayout::create(2,0));
	labelpane->setOpaque(false);
	label->setOpaque(false);
	label->setText("Window Width");
	labelpane->add(label);
	label = sdc::Label::create();
	label->setOpaque(false);
	label->setText("Window Height");
	labelpane->add(label);

	pane->add(btnpane);
	pane->add(labelpane);
	return ueberpane;
}
//-----------------------------------------------------------------------------
sdc::AContainerPtr SetupWindow::createDirListBtnPane() {
	dirListBtnPane = sdc::Panel::create();
	dirListBtnPane->setBackground(getContentPane()->getBackground());
	dirListBtnPane->setLayout(sdc::GridLayout::create(5,0,0,5.));
	sdc::Button::Ptr btn =
		createBtn(&SetupWindow::onBtnAddDirPressed, "add directory");
	dirListBtnPane->add(btn);
	
	btn = 
		createBtn(&SetupWindow::onBtnChangeDirPressed, "change directory");
	dirListBtnPane->add(btn);
	
	btn = 
		createBtn(&SetupWindow::onBtnRemoveDirPressed, "remove directory");
	dirListBtnPane->add(btn);

	rescanBtn = btn =
		createBtn(&SetupWindow::onBtnRescanPressed, "rescan");
	dirListBtnPane->add(btn);
    
    chkbxFS = sdc::CheckBox::create();
	chkbxFS->setText("Fastscan");
	chkbxFS->setOpaque(false);
	chkbxFS->sce::EventSender<sdc::events::ActionEvent>::addEventListener(
		boost::bind(&SetupWindow::onFastScanSelected, this, _1, _2)
	);
	dirListBtnPane->add(chkbxFS);

	return dirListBtnPane;
}
//-----------------------------------------------------------------------------
sdc::AContainerPtr SetupWindow::createMainBtnPane() {
	mainBtnPane = sdc::Panel::create();
    mainBtnPane->setBackground(getContentPane()->getBackground());
	sdc::Button::Ptr btn = createBtn(&SetupWindow::onBtnOkPressed, "OK");
	mainBtnPane->add(btn);
	
	btn = createBtn(&SetupWindow::onBtnCancelPressed, "Cancel");
	mainBtnPane->add(btn);
	return mainBtnPane;
}
//-----------------------------------------------------------------------------
sdc::StringList::Ptr SetupWindow::createDirList() {
	dirList = sdc::StringList::create();
	return dirList;
}
//-----------------------------------------------------------------------------
sdc::ScrollPanePtr SetupWindow::createDirListScrollPane() {
	createDirList();
	dirListScrollPane = sdc::ScrollPane::create(dirList);
	dirListScrollPane->setPreferredSize(sd::Dimension(450., 300.));
	return dirListScrollPane;
}
//-----------------------------------------------------------------------------
sdc::ButtonPtr SetupWindow::createBtn(BtnActionFunc f, const std::string &txt) {
	sdc::Button::Ptr res = sdc::Button::create();
	res->setText(txt);
	res->sce::EventSender<sdc::events::ActionEvent>::addEventListener(
		boost::bind(f, this, _1, _2)
	);
	return res;
}
//-----------------------------------------------------------------------------
namespace {
	ScanningDialog::Ptr __scanningDlg;
	void onScanningDlgClose(void *, const sdc::OnCloseEvent &ev) {
		__scanningDlg.reset();
    }
    void resetBtn(void *, const sdc::OnCloseEvent &ev, sdc::Button::WPtr _btn)
    {
        sdc::Button::Ptr btn = _btn.lock();
        if (btn) {
            btn->setEnabled(true);
        }
	}
}
//-----------------------------------------------------------------------------
void SetupWindow::openScanningDialog() {
	if (!__scanningDlg) {
        __scanningDlg = ScanningDialog::create(getPtr());
        __scanningDlg->validate();
        __scanningDlg->pack();
        __scanningDlg->positionWindow();
        __scanningDlg->addOnCloseEventListener(&onScanningDlgClose);
        __scanningDlg->open();
        __scanningDlg->startScan(ctrl);
    }
    rescanBtn->setEnabled(false);
    sdc::Button::WPtr btn = rescanBtn;
     __scanningDlg->addTrackedOnCloseEventListener(
        boost::bind(&resetBtn,_1,_2,btn),
        getPtr()
    );
}
//-----------------------------------------------------------------------------
void SetupWindow::onBtnOkPressed(void *, const sdc::events::ActionEvent &ev) {
	try {
		saveSettings();
	} catch (const std::exception &ex) {
		::com::osMessageBox ( 
			"Error", std::string("saving setting failed: ") + ex.what(), ::com::MSG_ALERT 
		);
		return;
	} catch(...) {
		::com::osMessageBox ( 
			"Error", "saving setting failed: unknown reason.", ::com::MSG_ALERT 
		);
		return;
	}
	if (!ctrl)
		return;
	close();
	if (ctrl->isAllScanned()) {
		return;
	}
	openScanningDialog();
}
//-----------------------------------------------------------------------------
void SetupWindow::onBtnRescanPressed(void *, const sdc::events::ActionEvent &ev) 
{
	try {
		saveSettings();
	} catch (const std::exception &ex) {
		::com::osMessageBox ( 
			"Error", std::string("saving setting failed: ") + ex.what(), ::com::MSG_ALERT 
		);
		//return;
	} catch(...) {
		::com::osMessageBox ( 
			"Error", "saving setting failed: unknown reason.", ::com::MSG_ALERT
		);
		return;
	}
	if (!ctrl)
		return;
	openScanningDialog();
	__scanningDlg->startScan(ctrl);
}	
//-----------------------------------------------------------------------------
void SetupWindow::onBtnCancelPressed(void *, const sdc::events::ActionEvent &ev) 
{
	cancelSettings();
	close();
}
//-----------------------------------------------------------------------------
void SetupWindow::onBtnAddDirPressed(void *, const sdc::events::ActionEvent &ev) 
{
	if (!ctrl)
		return;
	
	setEnabled(false);
	std::string dir = ctrl->selectDirectory("", getPtr());
	setEnabled(true);

	if (dir=="") {
		return;
	}
	if (!ctrl->addPluginFolder(dir)) {
		return;
	}
	dirList->addElement(dir);
	dirList->redraw();
}
//-----------------------------------------------------------------------------
void SetupWindow::onBtnChangeDirPressed(void *, const sdc::events::ActionEvent &ev)
{
	if (!ctrl)
		return;
	int index = dirList->getSelectedIndex();
	if (index<0)
		return;
	const std::string &old = dirList->get(index);

	setEnabled(false);
	std::string dir = ctrl->selectDirectory(old, getPtr());
	setEnabled(true);
	
	if (dir=="") {
		return;
	}
	if (!ctrl->removePluginFolder(old))
		return;
	if (!ctrl->addPluginFolder(dir)) {
		// restore old value
		ctrl->addPluginFolder(old);
		return;
	}
	dirList->set(index, dir);
	dirList->redraw();
}
//-----------------------------------------------------------------------------
void SetupWindow::onBtnRemoveDirPressed(void *, const sdc::events::ActionEvent &ev) 
{
	int index = dirList->getSelectedIndex();
	if (index<0)
		return;
	if (!ctrl)
		return;
	const std::string &str = dirList->get(index);
	if (!ctrl->removePluginFolder(str))
		return;
	dirList->removeElementAt(index);
	dirList->clearSelection();
	dirList->redraw();
}
}}} // namespace(s)
