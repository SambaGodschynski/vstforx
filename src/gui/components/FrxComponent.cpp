/*
 * FrxComponent.cpp
 *
 *  Created on: 17. Aug 2012
 *      Author: sambag
 */

#include "FrxComponent.hpp"
#include <sambag/disco/components/ui/ALookAndFeel.hpp>

namespace frx { namespace gui { namespace components {
//=============================================================================
//  Class FrxComponent
//=============================================================================
//-----------------------------------------------------------------------------
FrxComponent::FrxComponent() {
	setName("FrxComponent");
}
//-----------------------------------------------------------------------------
sdc::ui::AComponentUIPtr 
FrxComponent::getComponentUI(sdc::ui::ALookAndFeelPtr laf) const 
{
	return laf->getUI<FrxComponent>();
}
//-----------------------------------------------------------------------------
sd::Point2D FrxComponent::getPivot() const {
	sd::Coordinate w = getWidth(), h = getHeight();
	return sd::Point2D(w/2., h/2.);
}
}}} // namespace(s)
