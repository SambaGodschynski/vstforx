/*
 * FrxCircuidView.cpp
 *
 *  Created on: Mon Aug 20 12:12:59 2012
 *      Author: Johannes Unger
 */

#include "FrxCircuidView.hpp"
#include <sambag/disco/components/ui/ALookAndFeel.hpp>
#include "FrxControl.hpp"
#include <list>
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
	setComponentPopupMenu(getFrxControl(getPtr()).getCircuidViewPopup(getPtr()));
}
//-----------------------------------------------------------------------------
sdc::AComponentPtr FrxCircuidView::findComponentOnPoint(const sd::Point2D &p,
		FrxCircuidView::ZOrder _start, 
		FrxCircuidView::ZOrder _end
	)
{
	struct Filter {
		sd::Point2D loc;
		ZOrder start, end;
		Filter(const sd::Point2D &loc, ZOrder start, ZOrder end) :
		loc(loc), start(start), end(end) {}
		int operator()( sdc::AComponent::Ptr p ) {
			if (!p)
				return 0;
			if (loc==NULL_POINT2D) {
				return -1;
			}
			ZOrder z = FLT_MIN;
			p->getClientProperty(PROPERTY_ZORDER, z);
			if (z > end)
				return -1;
			if (p->contains(p->getLocationOnComponent(loc))) {
				loc=NULL_POINT2D;
				return 1;
			}
			return 0;
		}
	};
	ZOrder start = std::min(_start, _end);
	ZOrder end = std::max(_start, _end);
	std::list<sdc::AComponentPtr> res;
	findComponents(res, Filter(p, start, end));
	if (res.empty()){
		return sdc::AComponentPtr();
	}
	return res.back();
}
}}} // namespace(s)
