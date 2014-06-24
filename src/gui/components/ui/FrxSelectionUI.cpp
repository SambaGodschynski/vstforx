/*
 * FrxSelectionUI.cpp
 *
 *  Created on: Tue Aug 28 10:47:37 2012
 *      Author: Johannes Unger
 */

#include "FrxSelectionUI.hpp"
#include <sambag/disco/components/AComponent.hpp>
#include <sambag/disco/components/ui/UIManager.hpp>
#include <sambag/disco/components/PopupMenu.hpp>
#include <gui/IFrxControl.hpp>
#include <gui/components/FrxCircuidView.hpp>
#include <gui/components/FrxComponent.hpp>
#include <gui/components/VerticalFormatter.hpp>
#include <gui/components/HorizontalFormatter.hpp>
#include <gui/components/FrxPacket.hpp>

namespace frx { namespace gui {
namespace components { namespace ui { 
//=============================================================================
//  Class FrxSelectionUI
//=============================================================================
//-----------------------------------------------------------------------------
void FrxSelectionUI::installListeners(sdc::AComponentPtr c) {
	mouseListener = FrxSelectionMouseListener::create();
	c->sdc::EventSender<sdc::events::MouseEvent>::addTrackedEventListener (
		boost::bind(&FrxSelectionMouseListener::onMouse, mouseListener.get(), _1, _2),
		getPtr()
	);
}
//-----------------------------------------------------------------------------
void FrxSelectionUI::installDefaults(sdc::AComponentPtr c) {
	sdc::ui::UIManager &m = sdc::ui::getUIManager();
	m.getProperty("FrxSelection.selectedStyle", selectedStyle);
    vFormatter = VerticalFormatter::create();
    hFormatter = HorizontalFormatter::create();
    FrxSelection::Ptr sel = boost::dynamic_pointer_cast<FrxSelection> (c);
    sel->setFormatter(vFormatter);
}
//-----------------------------------------------------------------------------
void FrxSelectionUI::installUI(sdc::AComponentPtr c) {
	installDefaults(c);
	installListeners(c);
	installContextMenu(c);
}
//-----------------------------------------------------------------------------
void FrxSelectionUI::draw(sd::IDrawContext::Ptr cn, sdc::AComponentPtr c) {
	sd::Rectangle r = sd::Rectangle(0,0, c->getWidth(), c->getHeight()); 
	selectedStyle.intoContext(cn);
	cn->rect(r);
	cn->fill();
	cn->rect(r);
	cn->stroke();
}
//-----------------------------------------------------------------------------
namespace {
	void clearSelection(void *src, const sdce::ActionEvent &ev, sdc::AComponentWPtr c)
	{
		FrxSelection::Ptr sel = boost::dynamic_pointer_cast<FrxSelection> (c.lock());
		if (!sel) {
			return;
		}
		FrxCircuidViewPtr view = sel->getFirstContainer<FrxCircuidView>();
		if (!view) {
			return;
		}
		IFrxControl &ctrl = getFrxControl(view);
		typedef FrxSelection::ContentContainer C;
		BOOST_FOREACH(C::value_type v, sel->getContent()) {
			FrxComponentPtr cmp = boost::dynamic_pointer_cast<FrxComponent>(v.lock());
			if (!cmp) {
				continue;
			}
			ctrl.removeComponent(view, cmp);
		}
		sel->clearContent();

	}
    //-------------------------------------------------------------------------
	void packItems(const sdce::ActionEvent &ev, sdc::AComponentWPtr c)
	{
		FrxSelection::Ptr sel = boost::dynamic_pointer_cast<FrxSelection> (c.lock());
		if (!sel) {
			return;
		}
		FrxCircuidViewPtr view = sel->getFirstContainer<FrxCircuidView>();
		if (!view) {
			return;
		}
        FrxPacket::Ptr packet = FrxPacket::create();
        packet->setLocation(sel->getLocation());
        // add elements to packet
        typedef FrxSelection::ContentContainer SelContainer;
        BOOST_FOREACH(SelContainer::value_type x, sel->getContent()) {
            FrxComponent::Ptr comp =
                boost::dynamic_pointer_cast<FrxComponent>(x.lock());
            if (!comp) {
                continue;
            }
            packet->add(comp);
        }
        // add packet to view
        view->add(packet);
	}
} // namespace(s)
//-----------------------------------------------------------------------------
void FrxSelectionUI::rotate(const sdce::ActionEvent &ev, sdc::AComponentWPtr c)
{
    FrxSelection::Ptr sel = boost::dynamic_pointer_cast<FrxSelection> (c.lock());
    if (!sel) {
        return;
    }
    typedef FrxSelection::ContentContainer C;
    C components = sel->getContent();
    sd::Point2D p = sel->getLocation();
    sel->clearContent();
    sel->setFormatter(
        sel->getFormatter()==vFormatter ? hFormatter : vFormatter
    );
    BOOST_FOREACH(C::value_type v, components) {
        sel->addElement(v.lock(), p);
    }
}
//-----------------------------------------------------------------------------
void FrxSelectionUI::installContextMenu(sdc::AComponentPtr c) {
	sdc::PopupMenuPtr menu = sdc::PopupMenu::create();

	sdc::MenuItem::Ptr item = sdc::MenuItem::create();
	item->setText("flip orientation");
	item->sdc::EventSender<sdce::ActionEvent>::addEventListener (
		boost::bind(&FrxSelectionUI::rotate, this, _2, sdc::AComponentWPtr(c))
	);
	menu->add(item);
    
    item = sdc::MenuItem::create();
	item->setText("pack items");
	item->sdc::EventSender<sdce::ActionEvent>::addEventListener (
		boost::bind(&packItems, _2, sdc::AComponentWPtr(c))
	);
	menu->add(item);
    
    item = sdc::MenuItem::create();
	item->setText("remove items");
	item->sdc::EventSender<sdce::ActionEvent>::addEventListener (
		boost::bind(&clearSelection, _1, _2, sdc::AComponentWPtr(c))
	);
	menu->add(item);
    
	c->setComponentPopupMenu(menu);
}

}}}} // namespace(s)
