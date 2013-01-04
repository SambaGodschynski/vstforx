/*
 * FrxIO.cpp
 *
 *  Created on: Mon Aug 27 10:35:21 2012
 *      Author: Johannes Unger
 */

#include "FrxIO.hpp"

namespace frx { namespace gui { namespace components {
//=============================================================================
//  Class FrxIO
//=============================================================================
//-----------------------------------------------------------------------------
const std::string FrxIO::PROPERTY_STATE = "property state";
//-----------------------------------------------------------------------------
bool FrxIO::getState(State state) const {
	unsigned int mask = (1 << state);
	return ((states & mask) == mask);
}
//-----------------------------------------------------------------------------
void FrxIO::setState(State state, bool val) {
	unsigned int old = states;
	if (val) {
		states |= (1 << state);
	} else {
		states &= ~(1 << state);
	}
	firePropertyChanged(PROPERTY_STATE, old, states);
	sdc::AComponentPtr parent = getParent();
	if (parent) {
		parent->redraw();
	}
}
}}} // namespace(s)
