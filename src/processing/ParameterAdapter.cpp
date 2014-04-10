/*
 * ParameterAdapter.cpp
 *
 *  Created on: Thu Oct 18 13:31:50 2012
 *      Author: Johannes Unger
 */

#include "ParameterAdapter.hpp"
#include "IModelController.hpp"

namespace frx { namespace processing {
//=============================================================================
//  Class ParameterAdapter
//=============================================================================
//-----------------------------------------------------------------------------
bool ParameterAdapter::removeImpl(IModelControllerPtr ctrl) {
	return ctrl->removeParameter(getPtr());
}
}} // namespace(s)
