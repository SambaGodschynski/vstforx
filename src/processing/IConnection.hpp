/*
 * IConnection.hpp
 *
 *  Created on: Sat Oct  6 22:26:50 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_ICONNECTION_H
#define SAMBAG_ICONNECTION_H

#include <boost/shared_ptr.hpp>

#include "ModelObject.hpp"

namespace frx { namespace processing {
//=============================================================================
/** 
  * @class IConnection.
  */
class IConnection : public ModelObject {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<IConnection> Ptr;
public:
	//-------------------------------------------------------------------------
	virtual ModelObject::Ptr getSource() const = 0;
	//-------------------------------------------------------------------------
	virtual ModelObject::Ptr getDestination() const = 0;
}; // IConnection
}} // namespace(s)

#endif /* SAMBAG_ICONNECTION_H */
