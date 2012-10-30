/*
 * FrxColumnBrowser.cpp
 *
 *  Created on: Mon Oct 22 11:19:25 2012
 *      Author: Johannes Unger
 */

#include "FrxColumnBrowser.hpp"

namespace frx { namespace gui { namespace components { 
//=============================================================================
//  Class FrxColumnBrowser
//=============================================================================
//-----------------------------------------------------------------------------
sdcu::AComponentUIPtr 
FrxColumnBrowser::createComponentUI(sdcu::ALookAndFeelPtr laf) const
{
	return laf->getUI<FrxColumnBrowser>();
}
//-----------------------------------------------------------------------------
void FrxColumnBrowser::postConstructor() {
	Super::postConstructor();
	sdc::ui::UIManager::instance().installLookAndFeel(getRootPane(),
		ui::FrxLookAndFeel::create()
	);
	browser = BrowserImpl::create();
	getContentPane()->add(browser);
	buttonPane = sdc::Panel::create();
	getContentPane()->add(buttonPane, sdc::BorderLayout::SOUTH, -1);
}
//-----------------------------------------------------------------------------
void FrxColumnBrowser::setCtrl(IFrxColumnBrowserCtrl::Ptr ctrl) {
	this->ctrl = ctrl;
}
//-----------------------------------------------------------------------------
IFrxColumnBrowserCtrl::Ptr FrxColumnBrowser::getCtrl() const {
	return ctrl;
}
//-----------------------------------------------------------------------------
void FrxColumnBrowser::initTree(FrxCircuidViewPtr view) {
	if (!ctrl)
		return;
	ctrl->initTree(view, getPtr());
}
}}} // namespace(s)
