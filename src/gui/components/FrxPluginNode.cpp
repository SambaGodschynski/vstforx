/*
 * FrxPluginNode.cpp
 *
 *  Created on: Mon Aug 20 12:12:39 2012
 *      Author: Johannes Unger
 */

#include "FrxPluginNode.hpp"
#include <sambag/disco/components/ui/ALookAndFeel.hpp>

namespace frx { namespace gui { namespace components {
//=============================================================================
//  Class FrxPluginNode
//=============================================================================
//-----------------------------------------------------------------------------
FrxPluginNode::FrxPluginNode() {
	setName("FrxPluginNode");
}
//-----------------------------------------------------------------------------
sdcu::AComponentUIPtr FrxPluginNode::getComponentUI(sdcu::ALookAndFeelPtr laf) const
{
	return laf->getUI<FrxPluginNode>();
}
}}} // namespace(s)
