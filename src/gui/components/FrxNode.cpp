/*
 * FrxNode.cpp
 *
 *  Created on: Mon Aug 20 10:43:58 2012
 *      Author: Johannes Unger
 */

#include "FrxNode.hpp"
#include <sambag/disco/components/ui/ALookAndFeel.hpp>
#include <gui/components/ui/FrxNodeUI.hpp>
#include <gui/components/ui/FrxDualUI.hpp>
#include <sambag/disco/components/SvgComponent.hpp>

namespace frx { namespace gui { namespace components {
//=============================================================================
//  Class FrxNode
//=============================================================================
//-----------------------------------------------------------------------------
FrxNode::FrxNode() {
	setName("FrxNode");
}
//-----------------------------------------------------------------------------
void FrxNode::redraw() {
	AContainer::Ptr c = getParent();
	if (!c) {
		return;
	}
	c->redraw(getBounds());
}
//-----------------------------------------------------------------------------
sdcu::AComponentUIPtr FrxNode::createComponentUI(sdcu::ALookAndFeelPtr laf) const {
	return laf->getUI<FrxNode>();
}
//-----------------------------------------------------------------------------
sambag::com::Number FrxNode::getRadius() const {
	ui::FrxNodeUI::Ptr ui = std::dynamic_pointer_cast<ui::FrxNodeUI>(getUI());
	if (!ui) {
        ui::AFrxDualUI::Ptr dualUI =
            std::dynamic_pointer_cast<ui::AFrxDualUI>(getUI());
        if (dualUI) {
            ui =  std::dynamic_pointer_cast<ui::FrxNodeUI>(dualUI->getUI());
            if (ui) {
                return ui->getCoreRadius(getPtr());
            }
        }
    
        // TODO: handle clipping via related path
        sdc::AComponent::WPtr _imgc;
        getClientProperty("imageComponent", _imgc);
        sdc::SvgComponent::Ptr svg =
            std::dynamic_pointer_cast<sdc::SvgComponent>(_imgc.lock());
        if (!svg) {
            return 0;
        }
        sdc::AComponent::Ptr corona = svg->getDummyById("#core");
        return corona->getWidth()/2.;
	}
	return ui->getCoreRadius(getPtr());
}
//-----------------------------------------------------------------------------
void FrxNode::updateChildLocation(sdc::AComponent::Ptr c) {
	if (!c)
		return;
	// translate ctrl to center
	sd::Point2D loc ( 
		getWidth()/2. - c->getWidth()/2.,
		getHeight()/2. - c->getHeight()/2.
	);
	c->setLocation(loc);
}
//-----------------------------------------------------------------------------
void FrxNode::setBounds(const sd::Rectangle &r) {
	Super::setBounds(r);
	BOOST_FOREACH(AComponent::Ptr c, getComponents()) {
		updateChildLocation(c);
	}
}
//-----------------------------------------------------------------------------
void FrxNode::onChildChanged(void *src, 
	const sce::PropertyChanged &ev, sdc::AComponent::WPtr c) 
{
	if (ev.getPropertyName() == sdc::AComponent::PROPERTY_BOUNDS)
		updateChildLocation(c.lock());
}
//-----------------------------------------------------------------------------
void FrxNode::installChildListener(sdc::AComponent::Ptr c) {
	ChildSenderMap::iterator it = childSenderMap.find(c);
	ChildSender con = c->sce::EventSender<sce::PropertyChanged>::addEventListener (
		boost::bind(&FrxNode::onChildChanged, this, _1, _2, sdc::AComponent::WPtr(c))
	);
	if (it!=childSenderMap.end()) {
		it->second.disconnect();
		it->second = con;
	} else {
		childSenderMap[c] = con;
	}

}
//-----------------------------------------------------------------------------
void FrxNode::uninstallChildListener(sdc::AComponent::Ptr c) {
	ChildSenderMap::iterator it = childSenderMap.find(c);
	if (it==childSenderMap.end()) {
		return;
	}
	it->second.disconnect();
	childSenderMap.erase(it);
}
//-----------------------------------------------------------------------------
void FrxNode::add(sdc::AComponent::Ptr c) {
	Super::add(c);
	updateChildLocation(c);
	installChildListener(c);
}
//-----------------------------------------------------------------------------
void FrxNode::remove(sdc::AComponent::Ptr c) {
	uninstallChildListener(c);
	Super::remove(c);
}
}}} // namespace(s)
