#include "FrxSvgKnobUI.hpp"
#include <sambag/disco/components/SvgComponent.hpp>
#include <gui/components/FrxParameter.hpp>
#include <gui/components/FrxCircuidView.hpp>
#include <sambag/disco/components/DefaultBoundedRangeModel.hpp>

namespace frx { namespace gui {
namespace components { namespace ui {
namespace {
    void _onModel(sdc::AComponentWPtr _c) {
        sdc::AComponentPtr c = _c.lock();
        FrxCircuidView::Ptr view = c->getFirstContainer<FrxCircuidView>();
        if (!view) {
            return;
        }
        view->redraw();
    }
}
//=============================================================================
// class FrxSvgKnobUI 
//=============================================================================
//-----------------------------------------------------------------------------
void FrxSvgKnobUI::installKnobListener() {
    sdc::AComponentPtr myn = main.lock(),
                       hnd = handle.lock(),
                       coronaKnob = _coronaKnob.lock();
    SAMBAG_ASSERT(myn&&hnd&&coronaKnob);
    myn->sdc::EventSender<sdc::events::MouseEvent>::addTrackedEventListener(
		boost::bind(&FrxSvgKnobUI::onMouse, this, _1, _2),
		getPtr()
	);
    hnd->sdc::EventSender<sdc::events::MouseEvent>::addTrackedEventListener(
		boost::bind(&FrxSvgKnobUI::onMouse, this, _1, _2),
		getPtr()
	);
    coronaKnob->sdc::EventSender<sdc::events::MouseEvent>::addTrackedEventListener(
		boost::bind(&FrxSvgKnobUI::onMouse, this, _1, _2),
		getPtr()
	);
}
//-----------------------------------------------------------------------------
void FrxSvgKnobUI::setCoronaAlpha(double alpha) {
    sdc::SvgComponent::Dummy::Ptr component = _coronaKnob.lock();
    if (!component) {
        return;
    }
    sdsg::Style style = component->getStyle();
    style.fillOpacity(alpha);
    style.strokeOpacity(alpha);
    component->setStyle(style);
    if (alpha<0.1) {
        if (component->isVisible()) {
            component->setVisible(false);
        }
    } else {
        if (!component->isVisible()) {
            component->setVisible(true);
        }
    }
    FrxComponent::Ptr c = component->getFirstContainer<FrxComponent>();
    c->redraw();
    Super::setCoronaAlpha(alpha);
}
//-----------------------------------------------------------------------------
void FrxSvgKnobUI::installUI(sdc::AComponentPtr c) {
    Super::installUI(c);
    sdc::SvgComponent::Ptr svg = getSvgComponent();
    if (!svg) {
        throw std::runtime_error("FrxSvgKnobUI svg component not found");
    }
    // get main element
    std::vector<sdc::SvgComponent::Dummy::Ptr> l;
    svg->getDummiesByClass(".disco-knob",  l);
    if (l.empty()) {
        throw std::runtime_error("FrxSvgKnobUI no disco-knob defined");
    }
    if (l.size()>1) {
        throw std::runtime_error("FrxSvgKnobUI more than one disco-knob defined");
    }
    sdc::SvgComponent::Dummy::Ptr main = l.front();
    SAMBAG_ASSERT(main);
    this->main = main;
    // get handle element
    l.clear();
    svg->getDummiesByClass(".disco-knob-handle",  l);
    if (l.empty()) {
        throw std::runtime_error("FrxSvgKnobUI no disco-handle defined");
    }
    if (l.size()>1) {
        throw std::runtime_error("FrxSvgKnobUI more than one disco-handle defined");
    }
    sdc::SvgComponent::Dummy::Ptr handle = l.front();
    SAMBAG_ASSERT(handle);
    this->handle = handle;
    // remove encapsulated ctrl from view
    FrxParameter::Ptr par = boost::dynamic_pointer_cast<FrxParameter>(c);
    SAMBAG_ASSERT(par);
    sdc::AComponent::Ptr enc = par->getEncapsulatedCtrl();
    SAMBAG_ASSERT(enc);
    par->remove(enc);
    // set encapsulated ctrl as svg knob model
    sdc::DefaultBoundedRangeModel::Ptr model =
        boost::dynamic_pointer_cast<sdc::DefaultBoundedRangeModel>(enc);
    SAMBAG_ASSERT(model);
    main->setModel(model);
    // corona-knob
    sdc::SvgComponent::Dummy::Ptr coronaKnob =
        getSvgSubComponent("#corona-knob");
    if (!coronaKnob) {
        throw std::runtime_error("no #corona-knob element found");
    }
    _coronaKnob = coronaKnob;
    sdsg::Style style = coronaKnob->getStyle();
    style.fillOpacity(0);
    style.strokeOpacity(0);
    coronaKnob->setStyle(style);
    // install listener
    installKnobListener();

}
}}}} // namespace(s)
