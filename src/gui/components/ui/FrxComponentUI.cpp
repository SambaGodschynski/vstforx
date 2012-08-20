#include "FrxComponentUI.hpp"
#include <sambag/disco/components/AComponent.hpp>

namespace frx { namespace gui {
namespace components { namespace ui { 
//=============================================================================
// class FrxComponentUI
//=============================================================================
//-----------------------------------------------------------------------------
FrxComponentUI::FrxComponentUI() {

}
//-----------------------------------------------------------------------------
void FrxComponentUI::draw(sd::IDrawContext::Ptr cn, sdc::AComponentPtr c) {
	sd::Rectangle r(0,0,c->getWidth(), c->getHeight());
	cn->rect(r);
	cn->setFillColor(sd::ColorRGBA(0));
	cn->stroke();
}
//-----------------------------------------------------------------------------
void FrxComponentUI::installUI(sdc::AComponentPtr c) {

}
//-----------------------------------------------------------------------------
bool FrxComponentUI::contains(sdc::AComponent::Ptr c,
	const sd::Point2D &p)
{
	return Super::contains(c, p);
}
}}}}