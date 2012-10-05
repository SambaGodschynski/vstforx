/*
 * ModelObject.hpp
 *
 *  Created on: Fri Oct  5 13:39:30 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_MODELOBJECT_H
#define SAMBAG_MODELOBJECT_H

#include <boost/shared_ptr.hpp>

namespace frx { namespace processing {
//=============================================================================
/** 
  * @class ModelObject.
  */
class ModelObject {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<ModelObject> Ptr;
protected:
private:
public:
}; // ModelObject
}} // namespace(s)

#endif /* SAMBAG_MODELOBJECT_H */
