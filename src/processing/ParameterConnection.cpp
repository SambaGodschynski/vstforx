/*
 * ParameterConnection.cpp
 *
 *  Created on: Wed Oct 24 17:17:56 2012
 *      Author: Johannes Unger
 */

#include "ParameterConnection.hpp"
#include "ParameterAdapter.hpp"
#include <processing/parameter/ConnectionOperators.h>
#include <loki/Typelist.h>
#include "IModelController.hpp"

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
    res->initConnectionParameter();
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
		getCopIds<typename COps::Tail>(out);
	}
	template <>
	void getCopIds<Loki::NullType>(ParameterCnOpTypeIds &out) {
	}
	//#########################################################################
	template <class COps>
	pp::ConnectionOperator::Ptr createCOp(const ParameterCnOpTypeId &id) {
		typedef typename COps::Head COp;
		if (id == COp::name()) {
			return COp::create();
		}
		return createCOp<typename COps::Tail>(id);
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
void ParameterConnection::initConnectionParameter() {
    using namespace ::processing::parameter;
	HasParameter::Ptr hp = boost::shared_dynamic_cast<HasParameter>(cn);
	if (!hp) {
		return;
	}
	size_t num = hp->getNumParameter();
	for (size_t i=0; i<num; ++i) {
		Parameter::Ptr p;
		p = hp->getParameter(i);
		parameters.insert(std::make_pair(".", ParameterAdapter::create(p)));
	}
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
bool ParameterConnection::requestRemove() {
	bool res = true;
	BOOST_FOREACH(const ParameterGroupMap::value_type &v, parameters) {
		res &= v.second->requestRemove();
	}
	return res && Super::requestRemove();
}
//-----------------------------------------------------------------------------
bool ParameterConnection::removeImpl(IModelControllerPtr ctrl) {
	/*
		ParameterConnection disconnects when ModelObject deleted.
	*/
	return true;
}
}} // namespace(s)
