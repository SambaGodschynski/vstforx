/*
 * ViewObject.hpp
 *
 *  Created on: Fri Oct  5 13:39:18 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_VIEWOBJECT_H
#define SAMBAG_VIEWOBJECT_H

#include <memory>
#include <boost/serialization/access.hpp>
#include <com/SerializationFwd.h>
namespace frx { namespace gui {
//=============================================================================
/** 
  * @class ViewObject.
  */
class ViewObject {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef std::shared_ptr<ViewObject> Ptr;
protected:
private:
	///////////////////////////////////////////////////////////////////////////
	// Archive:
	//-------------------------------------------------------------------------
	friend class boost::serialization::access;
	//-------------------------------------------------------------------------
	void serialize(::com::iArchive &ar, const unsigned int version) {}
	void serialize(::com::oArchive &ar, const unsigned int version) {}
public:
	//-------------------------------------------------------------------------
	virtual std::string getObjectName() const = 0;
	//-------------------------------------------------------------------------
	virtual ~ViewObject() {}
}; // ViewObject
}} // namespace(s)

#endif /* SAMBAG_VIEWOBJECT_H */
