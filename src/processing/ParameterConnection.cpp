/*
 * ParameterConnection.cpp
 *
 *  Created on: Wed Oct 24 17:17:56 2012
 *      Author: Johannes Unger
 */

#include "ParameterConnection.hpp"
#include "ParameterAdapter.hpp"
#include <processing/parameter/ConnectionOperators.h>
#include <loki/TypeList.h>

namespace frx { namespace processing {
//=============================================================================
//  Class ParameterConnection
//=============================================================================
//-----------------------------------------------------------------------------
ParameterConnection::Ptr 
ParameterConnection::createConnection(IParameter::Ptr _a, 
	IParameter::Ptr _b)
{
	ParameterAdapter::Ptr a = boost::shared_dynamic_cast<ParameterAdapter>(_a);
	ParameterAdapter::Ptr b = boost::shared_dynamic_cast<ParameterAdapter>(_b);
	Ptr res = create();
	res->src = a;
	res->dst = b;
	res->cn = ::processing::parameter::ParameterConnection::create(
		a->getAdaptee(), b->getAdaptee());
	b->setValue(a->getValue());
	return res;
}
namespace {
	namespace pp = ::processing::parameter;
	typedef LOKI_TYPELIST_4(
		pp::InverseConnection, 
		pp::LogConnection, 
		pp::ExpConnection,
		pp::OffsetConnection
	) ConnectionOps;
	//#########################################################################
	template <class COps>
	void getCopIds(ParameterCnOpTypeIds &out) {
		out.push_back(COps::Head::name());
		getCopIds<COps::Tail>(out);
	}
	template <>
	void getCopIds<Loki::NullType>(ParameterCnOpTypeIds &out) {
	}
	//#########################################################################
	template <class COps>
	pp::ConnectionOperator::Ptr createCOp(const ParameterCnOpTypeId &id) {
		typedef COps::Head COp;
		if (id == COp::name()) {
			return COp::create();
		}
		return createCOp<COps::Tail>(id);
	}
	template <>
	pp::ConnectionOperator::Ptr createCOp<Loki::NullType>(const ParameterCnOpTypeId &id)
	{
		return pp::ConnectionOperator::Ptr();
	}

}// namespace(s)
//-----------------------------------------------------------------------------
void ParameterConnection::getParameterCnOpTypeIds(ParameterCnOpTypeIds &out) {
	out.reserve( Loki::TL::Length<ConnectionOps>::value );
	getCopIds<ConnectionOps>(out);
}
//-----------------------------------------------------------------------------
void ParameterConnection::
addParameterCnOp(const ParameterCnOpTypeId &opId)
{
	using namespace ::processing::parameter;
	ConnectionOperator::Ptr op = createCOp<ConnectionOps>(opId);
	cn->addOperator(op);
	HasParameter::Ptr hp = 
		boost::shared_dynamic_cast<HasParameter>(op);
	if (!hp) {
		return;
	}
	size_t num = hp->getNumParameter();
	for (size_t i=0; i<num; ++i) {
		Parameter::Ptr p;
		p = hp->getParameter(i);
		parameters.insert(std::make_pair(op->getName(), ParameterAdapter::create(p)));
	}
}
//-----------------------------------------------------------------------------
void ParameterConnection::getParameterGroupKeys(ParameterGroupKeys &out) const {
	ParameterGroupMap::const_iterator it = parameters.begin();
	while (it!=parameters.end()) {
		const ParameterGroupKey &key = it->first;
		out.insert(key);
		it = parameters.upper_bound(key); // next key
	}
}
//-----------------------------------------------------------------------------
void ParameterConnection::
getParameters(const ParameterGroupKey &key, Parameters &out) const 
{
	if (key=="*") { // all parameter
		out.reserve(parameters.size());
		BOOST_FOREACH(const ParameterGroupMap::value_type &v, parameters) {
			out.push_back(v.second);
		}
		return;
	}
	ParameterGroupMap::const_iterator it, end;
	boost::tie(it, end) = parameters.equal_range(key);
	out.reserve(parameters.count(key));
	for (; it!=end; ++it) {
		out.push_back(it->second);
	}
}
//-----------------------------------------------------------------------------
void ParameterConnection::getConnectionOps(ParameterCnOpTypeIds &out) {
}
//-----------------------------------------------------------------------------
bool ParameterConnection::requestRemove(ModelObject::Ptr obj) {
	bool res = true;
	BOOST_FOREACH(const ParameterGroupMap::value_type &v, parameters) {
		res &= v.second->requestRemove(v.second);
	}
	return res && Super::requestRemove(obj);
}
}} // namespace(s)
