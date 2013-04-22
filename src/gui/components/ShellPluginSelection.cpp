/*
 * ShellPluginSelection.cpp
 *
 *  Created on: Wed Dec  5 12:00:05 2012
 *      Author: Johannes Unger
 */

#include "ShellPluginSelection.hpp"
#include <gui/components/ui/FrxLookAndFeel.hpp>
#include <sambag/disco/components/ui/ALookAndFeel.hpp>
#include <sambag/disco/components/ui/UIManager.hpp>
#include <sambag/disco/components/Button.hpp>
#include <sambag/disco/components/Panel.hpp>
#include <sambag/disco/components/ScrollPane.hpp>
#include <sambag/disco/components/BorderLayout.hpp>
#include <com/one4All.h>

namespace frx { namespace gui { namespace components {
//=============================================================================
//  Class ShellPluginSelection
//=============================================================================
//-----------------------------------------------------------------------------
void ShellPluginSelection::postConstructor() {
	Super::postConstructor();
	sambag::disco::components::ui::UIManager::instance().
		installLookAndFeel(getRootPane(),
			ui::FrxLookAndFeel::create()
	);
	createContent();
}
//-----------------------------------------------------------------------------
void ShellPluginSelection::onBtnOk(void *, const ActionEvent &ev) {
	sce::EventSender<ActionEvent>::notifyListeners(
		this, ActionEvent(getPtr())
	);
	close();
}
//-----------------------------------------------------------------------------
void ShellPluginSelection::onBtnCancel(void *, const ActionEvent &ev) {
	close();
}
//-----------------------------------------------------------------------------
void ShellPluginSelection::createContent() {
	namespace sd = sambag::disco;
	namespace sdc = sd::components;
	// list
	list = sdc::StringList::create();
	sdc::ScrollPane::Ptr dirListScrollPane = sdc::ScrollPane::create(list);
	dirListScrollPane->setPreferredSize(sd::Dimension(450., 300.));
	getContentPane()->add(dirListScrollPane, sdc::BorderLayout::CENTER, APPEND);
	// mainbtn
	sdc::Panel::Ptr btns = sdc::Panel::create();
	sdc::Button::Ptr btnOk = sdc::Button::create();
	btnOk->setText("Ok");
	btnOk->sce::EventSender<ActionEvent>::addEventListener(
		boost::bind(&ShellPluginSelection::onBtnOk, this, _1, _2)
	);
	btns->add(btnOk);

	sdc::Button::Ptr btnCancel = sdc::Button::create();
	btnCancel->setText("Cancel");
	btnCancel->sce::EventSender<ActionEvent>::addEventListener(
		boost::bind(&ShellPluginSelection::onBtnCancel, this, _1, _2)
	);
	btns->add(btnCancel);
	getContentPane()->add(btns, sdc::BorderLayout::SOUTH, APPEND);
}
//-----------------------------------------------------------------------------
void ShellPluginSelection::setShellPlugin(const ::processing::PluginInfo &pI) {
	setTitle(pI.name + " is a shell plugin, select a plugin to open:");
	plugInf = pI;
}
//-----------------------------------------------------------------------------
const ::processing::PluginInfo &
ShellPluginSelection::getCurrentSelection() {
	int index = list->getSelectedIndex();
	if (index < 0 || index > (int)plugids.size()) {
		plugInf.location = "";
		return plugInf;
	}
	plugInf.location = 
		::com::createVSTPluginFilename(plugInf.location, plugids[index]);
	return plugInf;
}
//-----------------------------------------------------------------------------
void ShellPluginSelection::addShellInfo(const ::processing::ShellPluginInfo &inf) 
{
	list->addElement(inf.name);
	plugids.push_back(inf.id);
	list->redraw();
}
}}} // namespace(s)
