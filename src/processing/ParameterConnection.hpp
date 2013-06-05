/*
 * ParameterConnection.hpp
 *
 *  Created on: Wed Oct 24 17:17:56 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_PARAMETERCONNECTION_H
#define SAMBAG_PARAMETERCONNECTION_H

#include <boost/shared_ptr.hpp>
#include "IParameterConnection.hpp"
#include "ParameterAdapter.hpp"
#include "parameter/parameter.h"

namespace frx { namespace processing {
//=============================================================================
/** 
  * @class ParameterConnection.
  */
class ParameterConnection : public IParameterConnection {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef IParameterConnection Super;
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<ParameterConnection> Ptr;
	//-------------------------------------------------------------------------
	typedef ::processing::parameter::ConnectionOperator ConnectionOperator;
protected:
	//-------------------------------------------------------------------------
	ParameterConnection() {}
	//-------------------------------------------------------------------------
	virtual bool removeImpl(IModelControllerPtr ctrl);
private:
	//-------------------------------------------------------------------------
	ParameterAdapter::Ptr src;
	//-------------------------------------------------------------------------
	ParameterAdapter::Ptr dst;
	//-------------------------------------------------------------------------
	::processing::parameter::ParameterConnection::Ptr cn;
	//-------------------------------------------------------------------------
	typedef std::multimap<ParameterGroupKey, ParameterAdapter::Ptr> ParameterGroupMap;
	ParameterGroupMap parameters;
	///////////////////////////////////////////////////////////////////////////
	// Archive:
	//-------------------------------------------------------------------------
	friend class boost::serialization::access;
	//-------------------------------------------------------------------------
	template <typename Archive> 
	void serialize(Archive &ar, const unsigned int version) {
		ar & boost::serialization::base_object<IParameterConnection> ( *this );
		ar & src;
		ar & dst;
		ar & parameters;
		ar & cn;
	}
public:
    //-------------------------------------------------------------------------
    void initConnectionParameter();
	//-------------------------------------------------------------------------
	Ptr getPtr() const {
		return boost::shared_dynamic_cast<ParameterConnection>( self.lock() );
	}
	//-------------------------------------------------------------------------
	virtual ~ParameterConnection() {
	
	}
	//-------------------------------------------------------------------------
	static Ptr create() {
		Ptr res(new ParameterConnection());
		res->self = res;
		return res;
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
	//-------------------------------------------------------------------------
	static void getParameterCnOpTypeIds(ParameterCnOpTypeIds &out);
	//-------------------------------------------------------------------------
	virtual void addParameterCnOp(const ParameterCnOpTypeId &opId);
	//-------------------------------------------------------------------------
	virtual void getParameterGroupKeys(ParameterGroupKeys &out) const;
	//-------------------------------------------------------------------------
	/**
	 * @return parameter by group key. if key == "*" all parameter
	 * will be returned.
	 */
	virtual void getParameters(const ParameterGroupKey &key, Parameters &out) const;
	//-------------------------------------------------------------------------
	virtual void getConnectionOps(ParameterCnOpTypeIds &out);
	//-------------------------------------------------------------------------
	virtual bool requestRemove();
}; // ParameterConnection
}} // namespace(s)

#endif /* SAMBAG_PARAMETERCONNECTION_H */
