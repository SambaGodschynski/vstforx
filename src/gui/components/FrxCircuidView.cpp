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
const float FrxCircuidView::Z_InteractiveStuff = Z_OnTop;
//-----------------------------------------------------------------------------
void FrxCircuidView::add(sdc::AComponentPtr comp, ZOrder zord) {
	// order on insert:
	for (size_t i = 0; i<content->getComponentCount(); ++i) {
		AComponent::Ptr c = content->getComponent(i);
		ZOrder z = FLT_MAX;
		c->getClientProperty(PROPERTY_ZORDER, z);
		if (zord < z ) {
			comp->putClientProperty(PROPERTY_ZORDER, zord);
			content->add(comp, i);
			return;
		}
	}
	comp->putClientProperty(PROPERTY_ZORDER, zord);
	content->add(comp);
}
//-----------------------------------------------------------------------------
FrxCircuidView::FrxCircuidView() {
	setName("FrxCircuidView");
	//selection = FrxSelection::create();
	//add(selection, Z_InteractiveStuff); // !parent <= !!
}
//-----------------------------------------------------------------------------
void FrxCircuidView::remove(sdc::AComponentPtr comp) {
	content->remove(comp);
}
//-----------------------------------------------------------------------------
sdcu::AComponentUIPtr FrxCircuidView::createComponentUI(sdcu::ALookAndFeelPtr laf) const
{
	return laf->getUI<FrxCircuidView>();
}
//-----------------------------------------------------------------------------
void FrxCircuidView::postConstructor() {
	content = sdc::Panel::create();
	content->setSize(sd::Dimension(10000, 10000));
	Super::add(content);
	content->setLayout(sdc::ALayoutManagerPtr());
	selection = FrxSelection::create();
	add(selection, Z_InteractiveStuff);
}
}}} // namespace(s)
