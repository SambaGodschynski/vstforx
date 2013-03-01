/*
 * ModelObject.cpp
 *
 *  Created on: Fri Oct  5 13:39:30 2012
 *      Author: Johannes Unger
 */

#include "ModelObject.hpp"
#include <sambag/com/exceptions/IllegalStateException.hpp>
namespace frx { namespace processing {
//=============================================================================
//  Class ModelObject
//=============================================================================
//-----------------------------------------------------------------------------
bool ModelObject::requestRemove() {
	Ptr _this = self.lock();
	if (!_this) {
		SAMBAG_THROW(
			sambag::com::exceptions::IllegalStateException,
			"Null Pointer Exception."
		);
	}
	if (signal.num_slots()==0) {
		return true;
	}
	return signal(_this);
}
//-----------------------------------------------------------------------------
bool ModelObject::remove(IModelControllerPtr ctrl) {
	return requestRemove() && removeImpl(ctrl);
}
}} // namespace(s)
