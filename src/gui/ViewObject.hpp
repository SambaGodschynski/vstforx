/*
 * ViewObject.hpp
 *
 *  Created on: Fri Oct  5 13:39:18 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_VIEWOBJECT_H
#define SAMBAG_VIEWOBJECT_H

#include <boost/shared_ptr.hpp>
#include <boost/serialization/access.hpp>

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
	///////////////////////////////////////////////////////////////////////////
	// Archive:
	//-------------------------------------------------------------------------
	friend class boost::serialization::access;
	//-------------------------------------------------------------------------
	template <typename Archive> 
	void serialize(Archive &ar, const unsigned int version) {
	}
public:
	//-------------------------------------------------------------------------
	virtual std::string getObjectName() const = 0;
	//-------------------------------------------------------------------------
	virtual ~ViewObject() {}
}; // ViewObject
}} // namespace(s)

#endif /* SAMBAG_VIEWOBJECT_H */
