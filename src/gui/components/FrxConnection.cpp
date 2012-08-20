/*
 * FrxConnection.cpp
 *
 *  Created on: Mon Aug 20 10:44:56 2012
 *      Author: Johannes Unger
 */

#include "FrxConnection.hpp"
#include <sambag/disco/components/ui/ALookAndFeel.hpp>

namespace frx { namespace gui { namespace components {
//=============================================================================
//  Class FrxConnection
//=============================================================================
//-----------------------------------------------------------------------------
FrxConnection::FrxConnection() {
	setName("FrxConnection");
}
//-----------------------------------------------------------------------------
sdcu::AComponentUIPtr FrxConnection::getComponentUI(sdcu::ALookAndFeelPtr laf) const {
	return laf->getUI<FrxConnection>();
}
}}} // namespace(s)
