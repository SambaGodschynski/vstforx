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
const std::string BrowserConstants::FRX_BROWSER_PLUGIN = "plugin";
const std::string BrowserConstants::FRX_BROWSER_PRESET = "preset";
const std::string BrowserConstants::FRX_BROWSER_PROCESSOR = "processor";
const std::string BrowserConstants::FRX_BROWSER_PARAMETER = "parameter"; 
const std::string BrowserConstants::FRX_BROWSER_ADD_CONTENT_FOLDER = "addContent"; 
const std::string BrowserConstants::FRX_BROWSER_PLUGIN_INSTRUMENT = "pluginInstrument";
//-----------------------------------------------------------------------------
sd::ISurface::Ptr BrowserConstants::getIcon(const std::string &type) {
	sd::IResourceManager &m = sd::getResourceManager();
	return m.getImage("FrxBrowser." + type + ".image");
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
