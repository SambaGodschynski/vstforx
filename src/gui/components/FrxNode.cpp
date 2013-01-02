/*
 * FrxNode.cpp
 *
 *  Created on: Mon Aug 20 10:43:58 2012
 *      Author: Johannes Unger
 */

#include "FrxNode.hpp"
#include <sambag/disco/components/ui/ALookAndFeel.hpp>
#include <gui/components/ui/FrxNodeUI.hpp>

namespace frx { namespace gui { namespace components {
//=============================================================================
//  Class FrxNode
//=============================================================================
//-----------------------------------------------------------------------------
FrxNode::FrxNode() {
	setName("FrxNode");
}
//-----------------------------------------------------------------------------
sdcu::AComponentUIPtr FrxNode::createComponentUI(sdcu::ALookAndFeelPtr laf) const {
	return laf->getUI<FrxNode>();
}
//-----------------------------------------------------------------------------
sambag::com::Number FrxNode::getRadius() const {
	ui::FrxNodeUI::Ptr ui = boost::shared_dynamic_cast<ui::FrxNodeUI>(getUI());
	if (!ui) {
		return 0.;
	}
	return ui->getCoreRadius(getPtr());
}
}}} // namespace(s)
