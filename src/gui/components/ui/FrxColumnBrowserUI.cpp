/*
 * FrxColumnBrowserUI.cpp
 *
 *  Created on: Mon Oct 22 11:50:10 2012
 *      Author: Johannes Unger
 */

#include "FrxColumnBrowserUI.hpp"
#include <sambag/disco/components/ui/UIManager.hpp>

namespace frx { namespace gui {
namespace components { namespace ui { 
//=============================================================================
//  Class FrxColumnBrowserUI
//=============================================================================
//-----------------------------------------------------------------------------
void FrxColumnBrowserUI::installListeners(sdc::AComponentPtr c) {
}
//-----------------------------------------------------------------------------
void FrxColumnBrowserUI::installDefaults(sdc::AComponentPtr c) {
	sdc::ui::UIManager &m = sdc::ui::getUIManager();
	sd::Dimension size(300, 230);
	m.getProperty("Browser.size", size);
	c->setSize(size);
}
//-----------------------------------------------------------------------------
void FrxColumnBrowserUI::installUI(sdc::AComponentPtr c) {
	installDefaults(c);
	installListeners(c);
}
//-----------------------------------------------------------------------------
void FrxColumnBrowserUI::draw(sd::IDrawContext::Ptr cn, sdc::AComponentPtr c) {
}

}}}} // namespace(s)
