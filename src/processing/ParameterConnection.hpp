/*
 * ParameterConnection.hpp
 *
 *  Created on: Wed Oct 24 17:17:56 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_PARAMETERCONNECTION_H
#define SAMBAG_PARAMETERCONNECTION_H

#include <boost/shared_ptr.hpp>
#include "IConnection.hpp"
#include "ParameterAdapter.hpp"
#include "parameter/parameter.h"
namespace frx { namespace processing {
//=============================================================================
/** 
  * @class ParameterConnection.
  */
class ParameterConnection : public IConnection {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<ParameterConnection> Ptr;
	//-------------------------------------------------------------------------
protected:
	ParameterConnection() {}
private:
	//-------------------------------------------------------------------------
	ParameterAdapter::Ptr src;
	//-------------------------------------------------------------------------
	ParameterAdapter::Ptr dst;
	//-------------------------------------------------------------------------
	::processing::parameter::ParameterConnection::Ptr cn;
public:
	//-------------------------------------------------------------------------
	virtual ~ParameterConnection() {
	
	}
	//-------------------------------------------------------------------------
	static Ptr create() {
		return Ptr(new ParameterConnection());
	}
	//-------------------------------------------------------------------------
	virtual ModelObject::Ptr getSource() const {
		return src;
	}
	//-------------------------------------------------------------------------
	virtual ModelObject::Ptr getDestination() const {
		return dst;
	}
	//-------------------------------------------------------------------------
	::processing::parameter::Parameter::Ptr getSourceParameter() const {
		return cn->getParameterA();
	}
	//-------------------------------------------------------------------------
	::processing::parameter::Parameter::Ptr getDestinationParameter() const {
		return cn->getParameterB();
	}
	//-------------------------------------------------------------------------
	::processing::parameter::ParameterConnection::Ptr 
	getParameterConnection() const 
	{
		return cn;
	}
	//-------------------------------------------------------------------------
	static Ptr createConnection(IParameter::Ptr a, IParameter::Ptr b);
}; // ParameterConnection
}} // namespace(s)

#endif /* SAMBAG_PARAMETERCONNECTION_H */
