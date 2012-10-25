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

namespace frx { namespace gui { namespace components {
//=============================================================================
//  Class SetupWindow
//=============================================================================
//-----------------------------------------------------------------------------
void SetupWindow::setCtrl(SetupCtrl::Ptr ctrl) {
	this->ctrl = ctrl;
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
void SetupWindow::onBtnOkPressed(void *, const sdc::events::ActionEvent &ev) {
	close();
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
	std::string old = dirList->get(index);
	std::string dir = ctrl->selectDirectory(old);
	dirList->set(index, dir);
	dirList->redraw();
}
//-----------------------------------------------------------------------------
void SetupWindow::onBtnRemoveDirPressed(void *, const sdc::events::ActionEvent &ev) 
{
	int index = dirList->getSelectedIndex();
	if (index<0)
		return;
	dirList->removeElementAt(index);
	dirList->redraw();
}
}}} // namespace(s)
