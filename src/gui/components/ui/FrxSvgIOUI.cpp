#include "FrxSvgIOUI.hpp"
#include <sambag/disco/svg/SvgRoot.hpp>
#include <gui/components/FrxIO.hpp>
#include <sambag/disco/svg/graphicElements/Text.hpp>

namespace frx { namespace gui {
namespace components { namespace ui {
//=============================================================================
// class FrxSvgIOUI 
//=============================================================================
//-----------------------------------------------------------------------------
void FrxSvgIOUI::installUI(sdc::AComponentPtr c) {
    Super::installUI(c);
    FrxIO::Ptr io = boost::dynamic_pointer_cast<FrxIO>(c);
    SAMBAG_ASSERT(io);
    sdc::SvgComponent::Ptr svg = getSvgComponent();
    SAMBAG_ASSERT(svg);
    sdsg::ISceneGraph::Ptr g = getSceneGraph(svg);
    _display = g->getElementById("#frx-io-display");
    setDisplay(io->getDisplayText());
    sd::IDrawable::Ptr state = g->getElementById("#frx-io-active");
    if (!state) {
        return;
    }
    this->_state = state;
    c->sce::EventSender<sce::PropertyChanged>::addTrackedEventListener(
        boost::bind(&FrxSvgIOUI::onProperty, this, _2),
        shared_from_this()
    );
    g->setVisible(state, false);
    g->invalidate();
    svg->redraw();
}
//-----------------------------------------------------------------------------
void FrxSvgIOUI::onProperty(const sce::PropertyChanged &ev) {
    if (ev.getPropertyName()==FrxIO::PROPERTY_STATE) {
        sdc::SvgComponent::Ptr svg = getSvgComponent();
        sdsg::ISceneGraph::Ptr g = getSceneGraph(svg);
        sd::IDrawable::Ptr x = _state.lock();
        if (!x) {
            return;
        }
        unsigned int states=0;
        ev.getNewValue(states);
        bool state = FrxIO::getState(FrxIO::Activated, states);
        SAMBAG_BEGIN_SYNCHRONIZED(svg->getTreeLock())
            g->setVisible(x, state);
            g->invalidate();
        SAMBAG_END_SYNCHRONIZED
        svg->redraw();
    }
    if (ev.getPropertyName()==FrxIO::PROPERTY_DISPLAY_TXT) {
        std::string txt;
        ev.getNewValue(txt);
        setDisplay(txt);
    }
}
//-----------------------------------------------------------------------------
void FrxSvgIOUI::setDisplay(const std::string &txt) {
    sdsg::Text::Ptr displ = boost::dynamic_pointer_cast<sdsg::Text>(_display.lock());
    if (!displ) {
        return;
    }
    sdc::SvgComponent::Ptr svg = getSvgComponent();
    displ->setText(txt);
    svg->redraw();
}
}}}} // namespace(s)
