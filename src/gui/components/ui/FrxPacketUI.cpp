/*
 * FrxPacketUI.cpp
 *
 *  Created on: Tue Jun 24 10:11:04 2014
 *      Author: Johannes Unger
 */

#include "FrxPacketUI.hpp"
#include <sambag/disco/components/PopupMenu.hpp>
#include <OS_Specific/OS_com.h>
#include <sambag/disco/components/Window.hpp>

namespace frx { namespace gui {
namespace components { namespace ui {
//=============================================================================
//  Class FrxPacketUI
//=============================================================================
//-----------------------------------------------------------------------------
FrxPacketUI::FrxPacketUI() {
}
//-----------------------------------------------------------------------------
namespace {
    void onRename(sdc::AComponentWPtr _c) {
        sdc::AComponentPtr c = _c.lock();
        if (!c) {
            return;
        }
        sdc::Window::Ptr win = c->getFirstContainer<sdc::Window>();
        if (!win) {
            return;
        }
        std::string name = c->getName();
        ::com::osShowInputTextDlg("Name", name, win->getWindowImpl()->getSystemHandle());
        c->setName(name);
    }
} // namespace
void FrxPacketUI::createPopupmenuEntries(sdc::PopupMenuPtr menu,
    FrxCircuidViewPtr view, FrxComponentPtr c)
{
    sdc::MenuItem::Ptr item = sdc::MenuItem::create();
	item->setText("rename...");
	item->sdc::EventSender<sdce::ActionEvent>::addEventListener (
		boost::bind(&onRename, sdc::AComponentWPtr(c))
	);
	menu->add(item);
    Super::createPopupmenuEntries(menu, view, c);
}
//-----------------------------------------------------------------------------
void FrxPacketUI::installDefaults(sdc::AComponentPtr c) {
	setImage(
		sambag::disco::getResourceManager().getImage("FrxPacket.image")
	);
    Super::installDefaults(c);
}
//-----------------------------------------------------------------------------
void FrxPacketUI::installListeners(sdc::AComponentPtr c) {
    Super::installListeners(c);
}
//-----------------------------------------------------------------------------
sambag::com::Number FrxPacketUI::getCoreRadius(sdc::AComponentPtr c) const {
    if (!hasImage()) {
        return 0.;
    }
    sd::Rectangle r = getImage()->getSize();
    return r.width() / 2.;
}
//-----------------------------------------------------------------------------
FrxPacketUI::Ptr FrxPacketUI::create() {
    Ptr res(new FrxPacketUI());
    res->postConstructor(res);
    return res;
}
//-----------------------------------------------------------------------------
void FrxPacketUI::installUI(sdc::AComponentPtr c) {
    Super::installUI(c);
}
//-----------------------------------------------------------------------------
void FrxPacketUI::draw(sd::IDrawContext::Ptr cn, sdc::AComponentPtr c) {
 	Super::draw(cn, c);
	if (hasImage()) {
		drawImage(cn, c);
		return;
	}
}

}}}} // namespace(s)
