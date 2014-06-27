/*
 * FrxPacket.cpp
 *
 *  Created on: Tue Jun 24 10:10:15 2014
 *      Author: Johannes Unger
 */

#include "FrxPacket.hpp"
#include "FrxCircuidView.hpp"
#include "FrxSelection.hpp"

namespace frx { namespace gui { namespace components {
namespace {
    template <class Container>
    sd::Rectangle __getBounds(const Container &c) {
        sd::Point2D min(FLT_MAX, FLT_MAX),
                    max(-FLT_MAX, -FLT_MAX);
        BOOST_FOREACH(typename Container::value_type x, c) {
            min = sd::minimize(min, x.first->getBounds().x0());
            max = sd::maximize(max, x.first->getBounds().x1());
        }
        return sd::Rectangle(min, max);
    }
} // namespace
//=============================================================================
//  Class FrxPacket
//=============================================================================
//-----------------------------------------------------------------------------
void FrxPacket::postConstructor() {
    sce::EventSender<sce::PropertyChanged>::addEventListener(
        boost::bind(&FrxPacket::onPropertyChanged, this, _2)
    );
    setName("Packet");
}
//-----------------------------------------------------------------------------
void FrxPacket::onPropertyChanged(const sce::PropertyChanged &ev) {
    if (ev.getPropertyName() == PROPERTY_BOUNDS) {
        onBoundChanged();
    }
}
//-----------------------------------------------------------------------------
FrxPacket::Ptr FrxPacket::create() {
    Ptr res( new FrxPacket() );
    res->self = res;
    res->postConstructor();
    return res;
}
//-----------------------------------------------------------------------------
void FrxPacket::onBoundChanged() {
    BOOST_FOREACH(Container::value_type x, container) {
        updateComponentLocation(x.first);
    }
}
//-----------------------------------------------------------------------------
sdcu::AComponentUIPtr
FrxPacket::createComponentUI(sdcu::ALookAndFeelPtr laf) const
{
    return laf->getUI<FrxPacket>();
}
//-----------------------------------------------------------------------------
void FrxPacket::updateComponentLocation(FrxComponentPtr c) {
    sd::Point2D p =  getPivot();
    boost::geometry::add_point(p, getLocation());
    boost::geometry::subtract_point(p, c->getPivot());
    c->setLocation(p);
}
//-----------------------------------------------------------------------------
void FrxPacket::add(FrxComponentPtr x) {
    if (!x || !x->isVisible()) {
        return;
    }
    container.push_back(Content(x, sd::Point2D()));
}
//-----------------------------------------------------------------------------
void FrxPacket::pack() {
    sd::Point2D min = __getBounds(container).x0();
    BOOST_FOREACH(Container::value_type &x, container) {
        sd::Point2D p =  x.first->getLocation();
        boost::geometry::subtract_point(p, min);
        updateComponentLocation(x.first);
        x.second = p;
        x.first->setVisible(false);
    }
}
//-----------------------------------------------------------------------------
void FrxPacket::unpack() {
    if (container.empty()) {
        return;
    }
    FrxCircuidViewPtr view = getFirstContainer<FrxCircuidView>();
    FrxSelection::Ptr sel;
    if (view) {
        sel = view->getSelection();
    }
    sd::Point2D min = getLocation();
    BOOST_FOREACH(Container::value_type &x, container) {
        sd::Point2D p = min;
        boost::geometry::add_point(p, x.second);
        x.first->setLocation(p);
        x.first->setVisible(true);
        if (sel) {
            sel->getContent().push_back(x.first);
            sel->updateBounds();
            sel->setVisible(true);
        }
    }
}
//-----------------------------------------------------------------------------
FrxPacket::~FrxPacket() {
    unpack();
}
}}} // namespace(s)
