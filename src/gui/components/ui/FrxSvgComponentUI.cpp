#include "FrxSvgComponentUI.hpp"
#include <sambag/disco/components/AComponent.hpp>
#include <sambag/disco/components/PopupMenu.hpp>
#include <sambag/disco/components/Label.hpp>
#include <gui/components/FrxComponent.hpp>
#include <gui/components/FrxCircuidView.hpp>
#include <gui/IFrxControl.hpp>
#include <map>
#include "TooltipTexts.hpp"
#include <sambag/disco/components/ui/UIManager.hpp>
#include <gui/components/FrxMenuLabel.hpp>
#include <sambag/disco/svg/SvgRoot.hpp>
#include <com/Settings.h>

namespace frx { namespace gui {
namespace components { namespace ui {
//=============================================================================
// class FrxSvgComponentUI
//=============================================================================
//-----------------------------------------------------------------------------
FrxSvgComponentUI::FrxSvgComponentUI() {

}
//----------------------------------------------------------------------------
FrxSvgComponentUI::~FrxSvgComponentUI() {
}
//-----------------------------------------------------------------------------
sdc::SvgComponent::Dummy::Ptr
FrxSvgComponentUI::getSvgSubComponent(const std::string &id) const
{
    sdc::SvgComponent::Ptr svg = getSvgComponent();
    if (!svg) {
        throw std::runtime_error("FrxSvgNodeUI: missing image component");
    }
    sdc::SvgComponent::Dummy::Ptr res = svg->getDummyById(id);
    if (!res) {
        std::string path;
        getComponent()->getClientProperty("svgPath", path);
        throw std::runtime_error(path+": missing "+id+" element");
    }
    return res;
}
//-----------------------------------------------------------------------------
sdc::AContainerPtr FrxSvgComponentUI::getComponent() const {
    sdc::AContainer::Ptr res = component.lock();
    SAMBAG_ASSERT(res);
    return res;
}
//----------------------------------------------------------------------------
void FrxSvgComponentUI::postConstructor(FrxSvgComponentUI::Ptr self) {
	this->self = self;
}
//-----------------------------------------------------------------------------
void FrxSvgComponentUI::installUI(sdc::AComponentPtr c) {
    component = boost::dynamic_pointer_cast<sdc::AContainer>(c);
    installImageComponent(c);
	installListeners(c);
	installDefaults(c);
}
//-----------------------------------------------------------------------------
void FrxSvgComponentUI::setTooltipText(sdc::AComponentPtr c) {
	if (c->getTooltipText().length() > 0) {
		return;
	}
	c->setTooltipText(
		TooltipTexts::getText(*(c.get()))
	);
}
//-----------------------------------------------------------------------------
void FrxSvgComponentUI::createPopupmenuEntries(sdc::PopupMenuPtr menu, 
	FrxCircuidViewPtr view, 
	FrxComponentPtr c) 
{
	IFrxControl &ctrl = getFrxControl(view);
	sdc::MenuItem::Ptr item = sdc::MenuItem::create();
	item->setText("remove");
	item->sdc::EventSender<sdc::events::ActionEvent>::addTrackedEventListener (
		SAMBAG_CREATE_FRXCONTROL_CMD(ctrl,view,c,&IFrxControl::removeComponent),
		c
	);
	menu->add(item);
}
//-----------------------------------------------------------------------------
sdc::PopupMenuPtr FrxSvgComponentUI::createPopupmenu(FrxComponentPtr c, 
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
//-----------------------------------------------------------------------------
void FrxSvgComponentUI::installImageComponent(sdc::AComponentPtr c) {
    FrxComponent::Ptr fc = boost::dynamic_pointer_cast<FrxComponent>(c);
    if (!fc) {
        return;
    }
    try {
        std::string path =
            com::getSettings().getStylePath()+ "/" + fc->getTypeId() + ".svg";
        c->putClientProperty("svgPath", path);
        sdc::SvgComponent::Ptr svg = sdc::SvgComponent::create();
        svg->setSvgFilename(path);
        setImageComponent(svg);
    } catch (const std::exception &ex) {
        SAMBAG_LOG_ERR<<ex.what();
        throw;
    }
}
//-----------------------------------------------------------------------------
void FrxSvgComponentUI::installDefaults(sdc::AComponentPtr c) {
	FrxComponent::Ptr frxC = boost::dynamic_pointer_cast<FrxComponent>(c);
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
void FrxSvgComponentUI::installListeners(sdc::AComponentPtr c) {
}
//-----------------------------------------------------------------------------
void FrxSvgComponentUI::setImageComponent(sdc::SvgComponent::Ptr img) {
	imageComponent = img;
    sdc::AContainerPtr c = getComponent();
    if (!c) {
        return;
    }
    sdc::AComponent::WPtr _imgc;
    c->getClientProperty("imageComponent", _imgc);
    sdc::AComponent::Ptr imgc = _imgc.lock();
    if (imgc) {
        c->remove(imgc);
    }
    sd::Dimension size = imageComponent->getSvgObject()->getSize().size();
    imageComponent->setSize(size);
    c->setSize(size);
    c->add(imageComponent);
    _imgc = imageComponent;
    c->putClientProperty("imageComponent", _imgc);
}
//-----------------------------------------------------------------------------
void FrxSvgComponentUI::uninstallUI(sdc::AComponentPtr _c) {
    sdc::AContainerPtr c = boost::dynamic_pointer_cast<sdc::AContainer>(_c);
    if (!c) {
        return;
    }
    sdc::AComponent::WPtr _imgc;
    c->getClientProperty("imageComponent", _imgc);
    sdc::AComponent::Ptr imgc = _imgc.lock();
    if (imgc) {
        c->remove(imgc);
    }
    Super::uninstallUI(c);
}
}}}}
