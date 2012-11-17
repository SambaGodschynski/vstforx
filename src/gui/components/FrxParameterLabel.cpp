/*
 * FrxParameterLabel.cpp
 *
 *  Created on: Thu Nov 15 08:53:01 2012
 *      Author: Johannes Unger
 */

#include "FrxParameterLabel.hpp"
#include <sambag/disco/components/ui/ALookAndFeel.hpp>

namespace frx { namespace gui { namespace components {
//=============================================================================
//  Class FrxParameterLabel
//=============================================================================
//-----------------------------------------------------------------------------
FrxParameterLabel::FrxParameterLabel() {
	setName("FrxParameterLabel");
	setOpaque(true);
}
//-----------------------------------------------------------------------------
void FrxParameterLabel::postConstructor() {
}
//-----------------------------------------------------------------------------
sdc::ui::AComponentUIPtr 
FrxParameterLabel::createComponentUI(sdc::ui::ALookAndFeelPtr laf) const {
	return laf->getUI<FrxParameterLabel>();
}
}}} // namespace(s)
