/*
 * FrxTooltipManager.cpp
 *
 *  Created on: Sun Nov 25 15:29:13 2012
 *      Author: Johannes Unger
 */

#include "FrxTooltipManager.hpp"
#include <gui/components/FrxComponent.hpp>
#include <gui/components/FrxCircuidView.hpp>

namespace frx { namespace gui { namespace components {
typedef Loki::SingletonHolder<FrxTooltipManager> FrxTooltipManagerHolder;
//=============================================================================
//  Class FrxTooltipManager
//=============================================================================
//-----------------------------------------------------------------------------
FrxTooltipManager & FrxTooltipManager::instance() {
	return FrxTooltipManagerHolder::Instance();
}
//-----------------------------------------------------------------------------
void FrxTooltipManager::showTooltip(sambag::disco::components::AComponentPtr c)
{
	Super::showTooltip(c);
}
//-----------------------------------------------------------------------------
void FrxTooltipManager::
registerComponent(sambag::disco::components::AComponentPtr component) 
{
	Super::registerComponent(component);
}
//-----------------------------------------------------------------------------
void FrxTooltipManager::mouseEntered(const sambag::disco::components::events::MouseEvent &ev) 
{
	sambag::disco::components::AComponent::Ptr c = ev.getSource();
	FrxCircuidView::Ptr view = c->getFirstContainer<FrxCircuidView>();
	if (!view) {
		Super::mouseEntered(ev);
		return;
	}
	view->hintMessage(c->getTooltipText());
}
//-----------------------------------------------------------------------------
void FrxTooltipManager::mouseExited(const sambag::disco::components::events::MouseEvent &ev)
{
}
}}} // namespace(s)
