/*
 * FrxSvgPacketUI.cpp
 *
 *  Created on: Tue Jun 24 10:11:04 2014
 *      Author: Johannes Unger
 */

#include "FrxSvgPacketUI.hpp"
#include <sambag/disco/components/PopupMenu.hpp>
#include <OS_Specific/OS_com.h>
#include <sambag/disco/components/Window.hpp>
#include <gui/IFrxControl.hpp>
#include <gui/components/FrxPacket.hpp>
#include <gui/components/FrxMenuLabel.hpp>

namespace frx { namespace gui {
namespace components { namespace ui {
//=============================================================================
//  Class FrxSvgPacketUI
//=============================================================================
//-----------------------------------------------------------------------------
FrxSvgPacketUI::FrxSvgPacketUI() {
}
//-----------------------------------------------------------------------------
namespace {
    //-------------------------------------------------------------------------
    void onRename(sdc::AComponentWPtr _c)
    {
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
    //-------------------------------------------------------------------------
    void onUnpack(FrxCircuidView::WPtr _view, sdc::AComponentWPtr _c)
    {
        FrxCircuidView::Ptr view = _view.lock();
        sdc::AComponentPtr c = _c.lock();
        if (!view || !c) {
            return;
        }
        IFrxControl &ctrl = getFrxControl(view);
        FrxPacket::Ptr packet = std::dynamic_pointer_cast<FrxPacket>(c);
        if (!packet) {
            return;
        }
        packet->unpack();
        ctrl.removeComponent(view, packet);
    }
} // namespace
void FrxSvgPacketUI::createPopupmenuEntries(sdc::PopupMenuPtr menu,
    FrxCircuidViewPtr view, FrxComponentPtr c)
{
    sdc::MenuItem::Ptr item = sdc::MenuItem::create();
	item->setText("rename...");
	item->sdc::EventSender<sdce::ActionEvent>::addEventListener (
		boost::bind(&onRename, sdc::AComponentWPtr(c))
	);
	menu->add(item);
	item = sdc::MenuItem::create();
	item->setText("unpack");
	item->sdc::EventSender<sdc::events::ActionEvent>::addTrackedEventListener
    (
		boost::bind(&onUnpack, FrxCircuidView::WPtr(view), sdc::AComponentWPtr(c)),
		c
	);
	menu->add(item);
}
//-----------------------------------------------------------------------------
void FrxSvgPacketUI::installDefaults(sdc::AComponentPtr c) {
    Super::installDefaults(c);
}
//-----------------------------------------------------------------------------
void FrxSvgPacketUI::installListeners(sdc::AComponentPtr c) {
    Super::installListeners(c);
}
//-----------------------------------------------------------------------------
FrxSvgPacketUI::Ptr FrxSvgPacketUI::create() {
    Ptr res(new FrxSvgPacketUI());
    res->postConstructor(res);
    return res;
}
//-----------------------------------------------------------------------------
void FrxSvgPacketUI::installUI(sdc::AComponentPtr c) {
    Super::installUI(c);
    FrxComponent::Ptr frxC = std::dynamic_pointer_cast<FrxComponent>(c);
	FrxCircuidView::Ptr view = c->getFirstContainer<FrxCircuidView>();
	SAMBAG_ASSERT(frxC && view);
    // add popupmenu
	sdc::PopupMenuPtr menu = createPopupmenu(frxC, view);
	if (menu) {
		frxC->setComponentPopupMenu(menu);
	}
}
//-----------------------------------------------------------------------------
sdc::PopupMenuPtr FrxSvgPacketUI::createPopupmenu(FrxComponentPtr c, 
		FrxCircuidViewPtr view)
{
	sdc::PopupMenuPtr menu = sdc::PopupMenu::create();
	FrxMenuLabel::Ptr label = FrxMenuLabel::create();
	label->setText(c->getName());
	menu->add(label);
	createPopupmenuEntries(menu, view, c);
	if (menu->getComponentCount() == 0) {
		return sdc::PopupMenuPtr();
	}
	return menu;
}

}}}} // namespace(s)
