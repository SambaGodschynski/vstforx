#include "FrxSvgIOUI.hpp"
#include <sambag/disco/svg/SvgRoot.hpp>
#include <gui/components/FrxIO.hpp>

namespace frx { namespace gui {
namespace components { namespace ui {
//=============================================================================
// class FrxSvgIOUI 
//=============================================================================
//-----------------------------------------------------------------------------
void FrxSvgIOUI::installUI(sdc::AComponentPtr c) {
    Super::installUI(c);
    sdc::SvgComponent::Ptr svg = getSvgComponent();
    SAMBAG_ASSERT(svg);
    sdsg::SceneGraph::Ptr g = getSceneGraph(svg);
    sd::IDrawable::Ptr state = g->getElementById("#frx-io-active");
    if (!state) {
        return;
    }
    this->_state = state;
    c->sce::EventSender<sce::PropertyChanged>::addTrackedEventListener(
        boost::bind(&FrxSvgIOUI::onProperty, this, _2),
        shared_from_this()
    );
    g->setFlag(state, sdsg::SceneGraph::Invisible, true);
    g->invalidate();
    svg->redraw();
}
//-----------------------------------------------------------------------------
void FrxSvgIOUI::onProperty(const sce::PropertyChanged &ev) {
    if (ev.getPropertyName()==FrxIO::PROPERTY_STATE) {
        sdc::SvgComponent::Ptr svg = getSvgComponent();
        sdsg::SceneGraph::Ptr g = getSceneGraph(svg);
        sd::IDrawable::Ptr x = _state.lock();
        if (!x) {
            return;
        }
        unsigned int states=0;
        ev.getNewValue(states);
        bool state = FrxIO::getState(FrxIO::Activated, states);
        g->setFlag(x, sdsg::SceneGraph::Invisible, !state);
        g->invalidate();
        svg->redraw();
    }
}
}}}} // namespace(s)
