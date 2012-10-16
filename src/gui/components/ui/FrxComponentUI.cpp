#include "FrxComponentUI.hpp"
#include <sambag/disco/components/AComponent.hpp>
#include <sambag/disco/components/PopupMenu.hpp>
#include <gui/components/FrxComponent.hpp>
#include <gui/components/FrxCircuidView.hpp>
#include <gui/IFrxControl.hpp>

namespace frx { namespace gui {
namespace components { namespace ui { 
//=============================================================================
// class FrxComponentUI
//=============================================================================
//-----------------------------------------------------------------------------
FrxComponentUI::FrxComponentUI() {

}
//----------------------------------------------------------------------------
void FrxComponentUI::postConstructor(FrxComponentUI::Ptr self) {
	this->self = self;
}
//-----------------------------------------------------------------------------
void FrxComponentUI::draw(sd::IDrawContext::Ptr cn, sdc::AComponentPtr c) {
	sd::Rectangle r(0,0,c->getWidth(), c->getHeight());
	cn->rect(r);
	cn->setFillColor(sd::ColorRGBA(0));
	cn->stroke();
}
//-----------------------------------------------------------------------------
void FrxComponentUI::installUI(sdc::AComponentPtr c) {
	installListener(c);
	installDefaults(c);
}
//-----------------------------------------------------------------------------
bool FrxComponentUI::contains(sdc::AComponent::Ptr c,
	const sd::Point2D &p)
{
	return Super::contains(c, p);
}
void ee(FrxCircuidViewPtr, FrxComponentPtr);
//-----------------------------------------------------------------------------
void FrxComponentUI::createPopupmenuEntries(sdc::PopupMenuPtr menu, 
	FrxCircuidViewPtr view, 
	FrxComponentPtr c) 
{
	sdc::MenuItem::Ptr item = sdc::MenuItem::create();
	item->setText("remove " + c->getName());
	item->EventSender<sdc::events::ActionEvent>::addTrackedEventListener (
		getFrxControl(view).createCtrlCommandFunction(
			view, 
			c,
			boost::bind(&IFrxControl::removeComponent, &getFrxControl(view), _1, _2)
		),
		c
	);
	menu->add(item);
}
//-----------------------------------------------------------------------------
sdc::PopupMenuPtr FrxComponentUI::createPopupmenu(FrxComponentPtr c, 
		FrxCircuidViewPtr view)
{
	sdc::PopupMenuPtr menu = sdc::PopupMenu::create();
	createPopupmenuEntries(menu, view, c);
	if (menu->getComponentCount() == 0) {
		return sdc::PopupMenuPtr();
	}
	return menu;
}
//-----------------------------------------------------------------------------
void FrxComponentUI::installDefaults(sdc::AComponentPtr c) {
	FrxComponent::Ptr frxC = boost::shared_dynamic_cast<FrxComponent>(c);
	FrxCircuidView::Ptr view = c->getFirstContainer<FrxCircuidView>();
	SAMBAG_ASSERT(frxC && view);
	// add popupmenu
	sdc::PopupMenuPtr menu = createPopupmenu(frxC, view);
	if (menu) {
		frxC->setComponentPopupMenu(menu);
	}
}
//-----------------------------------------------------------------------------
void FrxComponentUI::installListener(sdc::AComponentPtr c) {
}
}}}}