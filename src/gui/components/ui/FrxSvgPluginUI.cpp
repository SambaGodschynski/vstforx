#include "FrxSvgPluginUI.hpp"
#include <gui/IFrxControl.hpp>
#include <gui/HandyNamespaces.hpp>
#include <gui/components/FrxCircuidView.hpp>
#include <sambag/disco/components/DefaultButtonModell.hpp>
namespace frx { namespace gui {
namespace components { namespace ui {
//=============================================================================
// class FrxSvgPluginUI 
//=============================================================================
//-----------------------------------------------------------------------------
void FrxSvgPluginUI::installListeners(sdc::AComponent::Ptr c) {
    Super::installListeners(c);
    
    sdc::SvgComponent::Ptr svg = getSvgComponent();
    // get main element
    std::vector<sdc::SvgComponent::Dummy::Ptr> l;
    svg->getDummiesByClass(".disco-button",  l);
    if (l.empty()) {
        throw std::runtime_error("FrxSvgKnobUI no disco-button defined");
    }
    if (l.size()>1) {
        throw std::runtime_error("FrxSvgKnobUI more than one disco-knob defined");
    }
    sdc::SvgComponent::Dummy::Ptr main = l.front();

    sdc::SvgComponent::Dummy::Ptr handle = getFirstChildOfClass(".disco-button-handle", main);
    sdc::SvgComponent::Dummy::Ptr idle = getFirstChildOfClass(".disco-button-handle-idle", handle);
    sdc::SvgComponent::Dummy::Ptr rollover = getFirstChildOfClass(".disco-button-handle-rollover", handle);
    sdc::SvgComponent::Dummy::Ptr pressed = getFirstChildOfClass(".disco-button-handle-pressed", handle);
    
    main->sdc::EventSender<sdc::events::MouseEvent>::addTrackedEventListener(
		boost::bind(&FrxSvgPluginUI::onMouse, this, _1, _2),
		getPtr()
	);
    handle->sdc::EventSender<sdc::events::MouseEvent>::addTrackedEventListener(
		boost::bind(&FrxSvgPluginUI::onMouse, this, _1, _2),
		getPtr()
	);
    idle->sdc::EventSender<sdc::events::MouseEvent>::addTrackedEventListener(
		boost::bind(&FrxSvgPluginUI::onMouse, this, _1, _2),
		getPtr()
	);
    rollover->sdc::EventSender<sdc::events::MouseEvent>::addTrackedEventListener(
		boost::bind(&FrxSvgPluginUI::onMouse, this, _1, _2),
		getPtr()
	);
    pressed->sdc::EventSender<sdc::events::MouseEvent>::addTrackedEventListener(
		boost::bind(&FrxSvgPluginUI::onMouse, this, _1, _2),
		getPtr()
	);
    FrxComponent::Ptr fc = c->getFirstContainer<FrxComponent>();
    if (!fc) {
        return;
    }
    // add editor open/close listener
    FrxCircuidView::Ptr view = c->getFirstContainer<FrxCircuidView>();
    if (!view) {
        return;
    }
	typedef sce::EventSender<sdc::events::ActionEvent> AcSender;
    std::shared_ptr<sdc::DefaultButtonModell> model =
        main->getModel<sdc::DefaultButtonModell>();
    if (!model) {
        return;
    }
	AcSender *sender = dynamic_cast<AcSender*>( model.get() );
	if (!sender) {
		return;
	}
	IFrxControl &ctrl = getFrxControl(view); 
	sender->addTrackedEventListener (
		SAMBAG_CREATE_FRXCONTROL_CMD(ctrl, view, fc,
		&IFrxControl::openClosePluginEditor),
		c
	);
}
}}}} // namespace(s)
