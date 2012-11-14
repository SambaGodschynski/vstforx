/*
 * FrxColumnBrowser.cpp
 *
 *  Created on: Mon Oct 22 11:19:25 2012
 *      Author: Johannes Unger
 */

#include "FrxColumnBrowser.hpp"
#include <sambag/disco/IResourceManager.hpp>

namespace frx { namespace gui { namespace components { 
//=============================================================================
// class BrowserConstants
//=============================================================================
//-----------------------------------------------------------------------------
const std::string BrowserConstants::FRX_BROWSER_FOLDER = "folder";
const std::string BrowserConstants::FRX_BROWSER_DEFAULT = "default";
//-----------------------------------------------------------------------------
sd::ISurface::Ptr BrowserConstants::getIcon(const std::string &type) {
	sd::IResourceManager &m = sd::getResourceManager();
	if (type==BrowserConstants::FRX_BROWSER_FOLDER) {
		return m.getImage("FrxBrowser.folder.image");
	}
	if (type==BrowserConstants::FRX_BROWSER_DEFAULT) {
		return m.getImage("FrxBrowser.default.image");
	}
	return sd::ISurface::Ptr();
}
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
	if (!ctrl)
		return;
	ctrl->initListeners(getPtr());
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
