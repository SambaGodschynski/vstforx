/*
 * IExit.hpp
 *
 *  Created on: Wed Oct 10 14:11:08 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_IEXIT_H
#define SAMBAG_IEXIT_H

#include <boost/shared_ptr.hpp>
#include "ModelObject.hpp"

namespace frx { namespace processing {
//=============================================================================
/** 
  * @class IExit.
  */
class IExit : public ModelObject {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<IExit> Ptr;
}; // IExit
}} // namespace(s)

#endif /* SAMBAG_IEXIT_H */
