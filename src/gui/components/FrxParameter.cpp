/*
 * FrxParameter.cpp
 *
 *  Created on: Mon Aug 20 10:45:54 2012
 *      Author: Johannes Unger
 */

#include "FrxParameter.hpp"
#include <sambag/disco/components/ui/ALookAndFeel.hpp>

namespace frx { namespace gui { namespace components {
//=============================================================================
//  Class FrxParameter
//=============================================================================
//-----------------------------------------------------------------------------
FrxParameter::FrxParameter() {
	setName("FrxParameter");
}
//-----------------------------------------------------------------------------
sdcu::AComponentUIPtr FrxParameter::createComponentUI(sdcu::ALookAndFeelPtr laf) const {
	return laf->getUI<FrxParameter>();
}
}}} // namespace(s)
