/*
 * FrxProcessorNode.cpp
 *
 *  Created on: Mon Aug 20 10:45:09 2012
 *      Author: Johannes Unger
 */

#include "FrxProcessorNode.hpp"
#include <sambag/disco/components/ui/ALookAndFeel.hpp>

namespace frx { namespace gui { namespace components {
//=============================================================================
//  Class FrxProcessorNode
//=============================================================================
//-----------------------------------------------------------------------------
FrxProcessorNode::FrxProcessorNode() {
	setName("FrxProcessorNode");
}
//-----------------------------------------------------------------------------
sdcu::AComponentUIPtr FrxProcessorNode::createComponentUI(sdcu::ALookAndFeelPtr laf) const {
	return laf->getUI<FrxProcessorNode>();
}
}}} // namespace(s)
