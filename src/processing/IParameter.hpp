/*
 * IParameter.hpp
 *
 *  Created on: Sat Oct  6 13:52:58 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_IPARAMETER_H
#define SAMBAG_IPARAMETER_H

#include <boost/shared_ptr.hpp>
#include "ModelObject.hpp"

namespace frx { namespace processing {
//=============================================================================
/** 
  * @class IParameter.
  */
class IParameter : public ModelObject {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<IParameter> Ptr;
}; // IParameter
}} // namespace(s)

#endif /* SAMBAG_IPARAMETER_H */
