/*
 * FrxCircuidViewUI.cpp
 *
 *  Created on: Mon Aug 20 12:13:07 2012
 *      Author: Johannes Unger
 */

#include "FrxCircuidViewUI.hpp"

namespace frx { namespace gui {
namespace components { namespace ui { 
//=============================================================================
//  Class FrxCircuidViewUI
//=============================================================================
//-----------------------------------------------------------------------------
FrxCircuidViewUI::FrxCircuidViewUI() {
}
//-----------------------------------------------------------------------------
FrxCircuidViewUI::Ptr FrxCircuidViewUI::create() {
	return Ptr(new FrxCircuidViewUI());
}
}}}} // namespace(s)
