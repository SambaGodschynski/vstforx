/*
 * FrxPacket.cpp
 *
 *  Created on: Tue Jun 24 10:10:15 2014
 *      Author: Johannes Unger
 */

#include "FrxPacket.hpp"
#include "FrxFlag.hpp"
#include "FrxCircuidView.hpp"

namespace frx { namespace gui { namespace components {
//=============================================================================
//  Class FrxPacket
//=============================================================================
//-----------------------------------------------------------------------------
void FrxPacket::postConstructor() {
    sce::EventSender<sce::PropertyChanged>::addEventListener(
        boost::bind(&FrxPacket::onPropertyChanged, this, _2)
    );
    setName("Packet");
    FrxCircuidView::Ptr view = getFirstContainer<FrxCircuidView>();
    if (!view) {
        return;
    }
    FrxFlag::Ptr flag = FrxFlag::create();
    flag->setTarget(getPtr());
    flag->setUpperFlagText(getName());
    view->add(flag, FrxCircuidView::Z_Flags, true);
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
void FrxPacket::updateComponentLocation(sdc::AComponentPtr c) {
    c->setLocation(getLocation());
}
//-----------------------------------------------------------------------------
void FrxPacket::add(FrxComponentPtr x) {
    if (!x || !x->isVisible()) {
        return;
    }
    container.push_back(Content(x, x->getLocation()));
    updateComponentLocation(x);
    x->setVisible(false);
}
//-----------------------------------------------------------------------------
void FrxPacket::unpack() {
    BOOST_FOREACH(Container::value_type x, container) {
        x.first->setLocation(x.second);
        x.first->setVisible(true);
    }
}
//-----------------------------------------------------------------------------
FrxPacket::~FrxPacket() {
    unpack();
}
}}} // namespace(s)
