/*
 * FrxProcessorNodeUI.cpp
 *
 *  Created on: Mon Aug 20 10:46:06 2012
 *      Author: Johannes Unger
 */

#include "FrxProcessorNodeUI.hpp"
#include <sambag/disco/svg/HtmlColors.hpp>
#include <vector>

namespace frx { namespace gui {
namespace components { namespace ui { 
namespace {
	std::vector<sd::ColorRGBA> colors;
	void initColors() {
		using namespace sambag::disco::svg;
		colors.reserve(100);
		colors.push_back( HtmlColors::getColor("red") );
		colors.push_back( HtmlColors::getColor("CornflowerBlue") );
		colors.push_back( HtmlColors::getColor("DeepPink") );
		colors.push_back( HtmlColors::getColor("Gold") );
		colors.push_back( HtmlColors::getColor("HotPink") );
		colors.push_back( HtmlColors::getColor("Khaki") );
		colors.push_back( HtmlColors::getColor("Orchid") );
		colors.push_back( HtmlColors::getColor("SpringGreen") );
		colors.push_back( HtmlColors::getColor("MediumVioletRed") );
		colors.push_back( HtmlColors::getColor("LightSteelBlue") );
		colors.push_back( HtmlColors::getColor("SlateBlue") );
		colors.push_back( HtmlColors::getColor("YellowGreen") );
		colors.push_back( HtmlColors::getColor("Fuchsia") );
		colors.push_back( HtmlColors::getColor("DeepPink") );
		colors.push_back( HtmlColors::getColor("MintCream") );
	}
}
//=============================================================================
//  Class FrxProcessorNodeUI
//=============================================================================
//-----------------------------------------------------------------------------
sd::ColorRGBA getProcessorFillColor(sdc::AComponent::Ptr c) {
	if (colors.empty()) {
		initColors();
	}
	static int i = 0; //rand() % colors.size();
	return colors[i++%colors.size()].setA(0.4);
}
}}}} // namespace(s)
