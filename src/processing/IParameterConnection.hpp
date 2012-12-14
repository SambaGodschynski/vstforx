/*
 * IParameterConnection.hpp
 *
 *  Created on: Fri Dec  7 15:48:02 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_IPARAMETERCONNECTION_H
#define SAMBAG_IPARAMETERCONNECTION_H

#include <boost/shared_ptr.hpp>
#include "IConnection.hpp"
#include "IParameter.hpp"

namespace frx { namespace processing {
//=============================================================================
/** 
  * @class IParameterConnection.
  */
class IParameterConnection : public IConnection {
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
	typedef boost::shared_ptr<IParameterConnection> Ptr;
	//-------------------------------------------------------------------------
	/**
	 * @return parameter connection operator type ids on connection.
	 */
	virtual void getConnectionOps(ParameterCnOpTypeIds &out) = 0;
	//-------------------------------------------------------------------------
	// TODO: 
	/**
	 * removes connection operator on index i which compares to the index
	 * in the ParameterCnOpTypeIds container getten by getConnectionOps().
	 */
	//virtual void removeConnectionOp(size_t index) = 0;
}; // IParameterConnection
}} // namespace(s)

#endif /* SAMBAG_IPARAMETERCONNECTION_H */
