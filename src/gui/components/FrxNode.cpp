/*
 * FrxNode.cpp
 *
 *  Created on: Mon Aug 20 10:43:58 2012
 *      Author: Johannes Unger
 */

#include "FrxNode.hpp"
#include <sambag/disco/components/ui/ALookAndFeel.hpp>

namespace frx { namespace gui { namespace components {
//=============================================================================
//  Class FrxNode
//=============================================================================
//-----------------------------------------------------------------------------
FrxNode::FrxNode() {
	setName("FrxNode");
}
//-----------------------------------------------------------------------------
sdcu::AComponentUIPtr FrxNode::getComponentUI(sdcu::ALookAndFeelPtr laf) const {
	return laf->getUI<FrxNode>();
}
}}} // namespace(s)
