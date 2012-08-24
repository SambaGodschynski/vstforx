/*
 * FrxCircuidView.cpp
 *
 *  Created on: Mon Aug 20 12:12:59 2012
 *      Author: Johannes Unger
 */

#include "FrxCircuidView.hpp"
#include <sambag/disco/components/ui/ALookAndFeel.hpp>

namespace frx { namespace gui { namespace components {
//=============================================================================
//  Class FrxCircuidView
//=============================================================================
//-----------------------------------------------------------------------------
const std::string FrxCircuidView::PROPERTY_ZORDER = "z_order";
//-----------------------------------------------------------------------------
const float FrxCircuidView::Z_Wires = 5.f;
//-----------------------------------------------------------------------------
const float FrxCircuidView::Z_ProcessorNodes = 4.f;
//-----------------------------------------------------------------------------
const float FrxCircuidView::Z_IO = 4.f;
//-----------------------------------------------------------------------------
const float FrxCircuidView::Z_Knobs = 3.f;
//-----------------------------------------------------------------------------
const float FrxCircuidView::Z_OnTop = 1.f;
//-----------------------------------------------------------------------------
const float FrxCircuidView::Z_Default = FrxCircuidView::Z_OnTop;
//-----------------------------------------------------------------------------
void FrxCircuidView::add(sdc::AComponentPtr comp, ZOrder zord) {
	// order on insert:
	for (size_t i = 0; i<getComponentCount(); ++i) {
		AComponent::Ptr c = getComponent(i);
		ZOrder z = FLT_MAX;
		c->getClientProperty(PROPERTY_ZORDER, z);
		if (zord < z ) {
			comp->putClientProperty(PROPERTY_ZORDER, zord);
			Super::add(comp, i);
			return;
		}
	}
	comp->putClientProperty(PROPERTY_ZORDER, zord);
	Super::add(comp);
}
//-----------------------------------------------------------------------------
FrxCircuidView::FrxCircuidView() {
	setName("FrxCircuidView");
}
//-----------------------------------------------------------------------------
void FrxCircuidView::remove(sdc::AComponentPtr comp) {
	Super::remove(comp);
}
//-----------------------------------------------------------------------------
sdcu::AComponentUIPtr FrxCircuidView::createComponentUI(sdcu::ALookAndFeelPtr laf) const
{
	return laf->getUI<FrxCircuidView>();
}
//-----------------------------------------------------------------------------
void FrxCircuidView::constructorAlt() {
	setLayout(sdc::ALayoutManagerPtr());
}
}}} // namespace(s)
