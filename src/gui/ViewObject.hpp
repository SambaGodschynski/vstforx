/*
 * ViewObject.hpp
 *
 *  Created on: Fri Oct  5 13:39:18 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_VIEWOBJECT_H
#define SAMBAG_VIEWOBJECT_H

#include <boost/shared_ptr.hpp>

namespace frx { namespace gui {
//=============================================================================
/** 
  * @class ViewObject.
  */
class ViewObject {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<ViewObject> Ptr;
protected:
private:
public:
	//-------------------------------------------------------------------------
	virtual ~ViewObject() {}
}; // ViewObject
}} // namespace(s)

#endif /* SAMBAG_VIEWOBJECT_H */
