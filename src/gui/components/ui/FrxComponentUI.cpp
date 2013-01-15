#include "FrxComponentUI.hpp"
#include <sambag/disco/components/AComponent.hpp>
#include <sambag/disco/components/PopupMenu.hpp>
#include <gui/components/FrxComponent.hpp>
#include <gui/components/FrxCircuidView.hpp>
#include <gui/IFrxControl.hpp>
#include <map>
#include "TooltipTexts.hpp"
namespace frx { namespace gui {
namespace components { namespace ui {
namespace {

}
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
	installListeners(c);
	installDefaults(c);
}
//-----------------------------------------------------------------------------
void FrxComponentUI::setTooltipText(sdc::AComponentPtr c) {
	if (c->getTooltipText().length() > 0) {
		return;
	}
	c->setTooltipText(
		TooltipTexts::getText(*(c.get()))
	);
}
//-----------------------------------------------------------------------------
bool FrxComponentUI::contains(sdc::AComponent::Ptr c,
	const sd::Point2D &p)
{
	return Super::contains(c, p);
}
//-----------------------------------------------------------------------------
void FrxComponentUI::createPopupmenuEntries(sdc::PopupMenuPtr menu, 
	FrxCircuidViewPtr view, 
	FrxComponentPtr c) 
{
	IFrxControl &ctrl = getFrxControl(view);
	sdc::MenuItem::Ptr item = sdc::MenuItem::create();
	item->setText("remove " + c->getName());
	item->sdc::EventSender<sdc::events::ActionEvent>::addTrackedEventListener (
		SAMBAG_CREATE_FRXCONTROL_CMD(ctrl,view,c,&IFrxControl::removeComponent),
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
	// tooltip
	setTooltipText(c);
}
//-----------------------------------------------------------------------------
void FrxComponentUI::installListeners(sdc::AComponentPtr c) {
}
//-----------------------------------------------------------------------------
void FrxComponentUI::setImage(sd::ISurface::Ptr img) {
	image = img;
}
//-----------------------------------------------------------------------------
void FrxComponentUI::drawImage(sd::IDrawContext::Ptr cn, sdc::AComponentPtr c) {
	if (!image || !c)
		return;
	// calc. offset
	// TODO: handle context transf.
	sd::Rectangle r = image->getSize(); 
	sd::Point2D imageOffset(
		(c->getWidth() - r.width()) / 2.,
		(c->getHeight() - r.height()) / 2.
	); 
	cn->translate(imageOffset);
	cn->drawSurface(image);
}
}}}}
