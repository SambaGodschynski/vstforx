/*
 * FrxHover.cpp
 *
 *  Created on: Thu Sep 27 14:56:25 2012
 *      Author: Johannes Unger
 */

#include "FrxHover.hpp"
#include "VerticalFormatter.hpp"
namespace frx { namespace gui { namespace components {
//=============================================================================
//  Class FrxHover
//=============================================================================
//-----------------------------------------------------------------------------
void FrxHover::postConstructor() {
	Super::postConstructor();
	formatter = VerticalFormatter::create();
}
//-----------------------------------------------------------------------------
void FrxHover::setFormatter(IFormatter::Ptr fomatter) {
	this->formatter = formatter;
}
//-----------------------------------------------------------------------------
void FrxHover::addElement(sdc::AComponent::Ptr c) {
	if (formatter)
		formatter->addElement(c);
	Super::addElement(c);
}
//-----------------------------------------------------------------------------
void FrxHover::clearContent() {
	Super::clearContent();
	formatter->reset();
}
}}} // namespace(s)
