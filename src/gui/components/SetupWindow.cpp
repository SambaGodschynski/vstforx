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
#include <boost/foreach.hpp>
#include <sambag/com/Thread.hpp>
#include <sambag/disco/components/Timer.hpp>
#include <queue>
#include <boost/unordered_map.hpp>
#include <sambag/com/Thread.hpp>
namespace frx { namespace gui { namespace components {
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
	void onBtnOk(void *, const sdc::events::ActionEvent& ev);
	//-------------------------------------------------------------------------
	void onBtnCancel(void *, const sdc::events::ActionEvent& ev);
	//-------------------------------------------------------------------------
	void onFileEvent(const std::string &file, SetupCtrl::FileStatus fst);
	//-------------------------------------------------------------------------
	void onScanCompleted();
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
}
//-----------------------------------------------------------------------------
sdc::AContainerPtr ScanningDialog::createListPane() {
	list = sdc::StringList::create();
	dirListScrollPane = sdc::ScrollPane::create(list);
	dirListScrollPane->setPreferredSize(sd::Dimension(450., 300.));
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
	res->EventSender<sdc::events::ActionEvent>::addEventListener(
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
	if (ctrl) {
		ctrl->stopScanning();
		ctrl->joinScan();
	}
	close();
}
//-----------------------------------------------------------------------------
void ScanningDialog::startScan(SetupCtrl::Ptr ctrl) {
	this->ctrl = ctrl;
	if (!ctrl)
		return;
	btnOk->setEnabled(false);
	ctrl->startScan(
		boost::bind(&ScanningDialog::onFileEvent, this, _1, _2),
		boost::bind(&ScanningDialog::onScanCompleted, this)
	);
	timer = sdc::Timer::create(100);
	timer->EventSender<sdc::TimerEvent>::addTrackedEventListener(
		boost::bind(&ScanningDialog::onRefresh, this, _1, _2),
		getPtr()
	);
	timer->setNumRepetitions(-1);
	timer->start();
}
//-----------------------------------------------------------------------------
void ScanningDialog::onFileEvent(const std::string &file, SetupCtrl::FileStatus fst) 
{
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
void ScanningDialog::onScanCompleted() {
	timer->stop();
	btnOk->setEnabled(true);
}
//-----------------------------------------------------------------------------
void ScanningDialog::onRefresh(void *, const sdc::TimerEvent &ev) {
	if (tmpEntries.empty()) {
		return;
	}
	SAMBAG_BEGIN_SYNCHRONIZED(mutex)
		while(!tmpEntries.empty()) {
			const std::string &file = tmpEntries.front().first;
			const std::string &status = tmpEntries.front().second;
			File2Listindex::const_iterator it =
				file2listindex.find(file);
			if (it == file2listindex.end()) {
				list->addElement(file);
				file2listindex[file] = list->DefaultListModel::getSize() - 1;
			} else {
				list->set(it->second, file + "->" + status);
			}
			tmpEntries.pop();
		}
	SAMBAG_END_SYNCHRONIZED
	dirListScrollPane->revalidate();
	int i = list->DefaultListModel::getSize() - 1;
	list->ensureIndexIsVisible(i);
	list->revalidate();
	list->redraw();
}
//=============================================================================
//  Class SetupWindow
//=============================================================================
//-----------------------------------------------------------------------------
void SetupWindow::setCtrl(SetupCtrl::Ptr ctrl) {
	this->ctrl = ctrl;
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
	revalidate();
	redraw();
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
	getContentPane()->add(createSetupPane(), sdc::BorderLayout::CENTER, APPEND);
	getContentPane()->add(createMainBtnPane(), sdc::BorderLayout::SOUTH, APPEND);
}
//-----------------------------------------------------------------------------
SetupWindow::~SetupWindow() {
}
//-----------------------------------------------------------------------------
sdc::AContainerPtr SetupWindow::createSetupPane() {
	sdc::Panel::Ptr pane = sdc::Panel::create();
	//pane->setLayout(sdc::GridLayout::create(0, 2));
	pane->add(createDirListPane());
	pane->add(createDirListBtnPane());
	return pane;
}
//-----------------------------------------------------------------------------
sdc::AContainerPtr SetupWindow::createDirListPane() {
	dirListPane = createDirListScrollPane();
	return dirListPane;
}
//-----------------------------------------------------------------------------
sdc::AContainerPtr SetupWindow::createDirListBtnPane() {
	dirListBtnPane = sdc::Panel::create();
	dirListBtnPane->setLayout(sdc::GridLayout::create(3,0));
	sdc::Button::Ptr btn =
		createBtn(&SetupWindow::onBtnAddDirPressed, "add directory");
	dirListBtnPane->add(btn);
	
	btn = 
		createBtn(&SetupWindow::onBtnChangeDirPressed, "change directory");
	dirListBtnPane->add(btn);
	
	btn = 
		createBtn(&SetupWindow::onBtnRemoveDirPressed, "remove directory");
	dirListBtnPane->add(btn);

	return dirListBtnPane;
}
//-----------------------------------------------------------------------------
sdc::AContainerPtr SetupWindow::createMainBtnPane() {
	mainBtnPane = sdc::Panel::create();
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
	res->EventSender<sdc::events::ActionEvent>::addEventListener(
		boost::bind(f, this, _1, _2)
	);
	return res;
}
//-----------------------------------------------------------------------------
namespace {
	ScanningDialog::Ptr scanningDlg;
	void onScanningDlgClose(void *, const sdc::OnCloseEvent &ev) {
		scanningDlg.reset();
	}
}
//-----------------------------------------------------------------------------
void SetupWindow::openScanningDialog() {
	if (scanningDlg) // already open
		return;
	scanningDlg = ScanningDialog::create();
	scanningDlg->validate();
	scanningDlg->pack();
	scanningDlg->addOnCloseEventListener(&onScanningDlgClose);
	scanningDlg->open();
}
//-----------------------------------------------------------------------------
void SetupWindow::onBtnOkPressed(void *, const sdc::events::ActionEvent &ev) {
	try {
		if (ctrl) {
			ctrl->saveSettings();
		}
	} catch (const std::exception &ex) {
		// TODO: handle
		throw;
	} catch(...) {
		//TODO: handle
		throw;
	}
	close();
	openScanningDialog();
	scanningDlg->startScan(ctrl);
}	
//-----------------------------------------------------------------------------
void SetupWindow::onBtnCancelPressed(void *, const sdc::events::ActionEvent &ev) 
{
	close();
}
//-----------------------------------------------------------------------------
void SetupWindow::onBtnAddDirPressed(void *, const sdc::events::ActionEvent &ev) 
{
	if (!ctrl)
		return;
	std::string dir = ctrl->selectDirectory();
	if (!ctrl->addPluginFolder(dir))
		return;
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
	std::string dir = ctrl->selectDirectory(old);
	if (!ctrl->removePluginFolder(old))
		return;
	if (!ctrl->addPluginFolder(dir))
		return;
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
