/*
 * INodeConnection.hpp
 *
 *  Created on: Fri Dec  7 15:47:57 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_INODECONNECTION_H
#define SAMBAG_INODECONNECTION_H

#include <boost/shared_ptr.hpp>
#include "IConnection.hpp"
namespace frx { namespace processing {
//=============================================================================
/** 
  * @class INodeConnection.
  */
class INodeConnection : public IConnection {
//=============================================================================
private:
	///////////////////////////////////////////////////////////////////////////
	// Archive:
	//-------------------------------------------------------------------------
	friend class boost::serialization::access;
	//-------------------------------------------------------------------------
	template <typename Archive> 
	void serialize(Archive &ar, const unsigned int version) {
		ar & boost::serialization::base_object<IConnection> ( *this );
	}
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<INodeConnection> Ptr;
}; // INodeConnection
}} // namespace(s)

#endif /* SAMBAG_INODECONNECTION_H */
