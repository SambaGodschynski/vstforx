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
	 * @return the number of ops on connection.
	 */
	virtual size_t getNumConnectionOps() = 0;
	//-------------------------------------------------------------------------
	/**
	 * @brief removes connection operator on index i
	 */
    virtual void removeConnectionOp(size_t index) = 0;
    //-------------------------------------------------------------------------
    /**
     * @name of op on index
     */
    virtual std::string getConnectionOpName(size_t index) = 0;
}; // IParameterConnection
}} // namespace(s)

#endif /* SAMBAG_IPARAMETERCONNECTION_H */
