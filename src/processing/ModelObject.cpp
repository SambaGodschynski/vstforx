/*
 * ModelObject.cpp
 *
 *  Created on: Fri Oct  5 13:39:30 2012
 *      Author: Johannes Unger
 */

#include "ModelObject.hpp"
namespace frx { namespace processing {
//=============================================================================
//  Class ModelObject
//=============================================================================
//-----------------------------------------------------------------------------
bool ModelObject::requestRemove(Ptr self) {
	if (signal.num_slots()==0) {
		return true;
	}
	return signal(self);
}
}} // namespace(s)
