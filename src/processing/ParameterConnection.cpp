/*
 * ParameterConnection.cpp
 *
 *  Created on: Wed Oct 24 17:17:56 2012
 *      Author: Johannes Unger
 */

#include <tuple>
#include "ParameterConnection.hpp"
#include "ParameterAdapter.hpp"
#include <processing/parameter/ConnectionOperators.h>
#include <loki/Typelist.h>
#include "IModelController.hpp"
#include <sambag/com/exceptions/IllegalArgumentException.hpp>

namespace frx { namespace processing {
//=============================================================================
//  Class ParameterConnection
//=============================================================================
//-----------------------------------------------------------------------------
ParameterConnection::Ptr 
ParameterConnection::createConnection(IParameter::Ptr _a, 
	IParameter::Ptr _b)
{
	ParameterAdapter::Ptr a = std::dynamic_pointer_cast<ParameterAdapter>(_a);
	ParameterAdapter::Ptr b = std::dynamic_pointer_cast<ParameterAdapter>(_b);
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
	typedef LOKI_TYPELIST_6(
		pp::InverseConnection, 
		pp::LogConnection, 
		pp::ExpConnection,
		pp::OffsetConnection,
        pp::MultiplierConnection,
        pp::MinMaxConnection
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
	HasParameter::Ptr hp = std::dynamic_pointer_cast<HasParameter>(cn);
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
    if (!op) {
        SAMBAG_THROW(sambag::com::exceptions::IllegalArgumentException,"invalid operator: "+opId);
    }
	cn->addOperator(op);
    operators.push_back(op);
	HasParameter::Ptr hp = 
		std::dynamic_pointer_cast<HasParameter>(op);
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
size_t ParameterConnection::getNumConnectionOps() {
    return operators.size();
}
//-----------------------------------------------------------------------------
void ParameterConnection::removeOpParameter(::processing::parameter::Parameter::Ptr p) {
    // inefficiency! but for only a couple of parameter
    // its the easyiest way to perform
    ParameterGroupMap::iterator it = parameters.begin();
    for (; it!=parameters.end(); ++it) {
        SAMBAG_ASSERT(it->second);
        if (it->second->getAdaptee() == p) {
            parameters.erase(it);
            break;
        }
    }
}
//-----------------------------------------------------------------------------
void ParameterConnection::removeConnectionOp(size_t index) {
    if (index>=operators.size()) {
        SAMBAG_LOG_WARN<<"ParameterConnection::removeConnectionOp() out of bounds";
        return;
    }
    ConnectionOperator::Ptr op = operators[index];
    cn->removeOperator(op);
    operators.erase(operators.begin()+index);
    
    // remove op parameter
    using namespace ::processing::parameter;
	HasParameter::Ptr hp = std::dynamic_pointer_cast<HasParameter>(op);
	if (!hp) {
		return;
	}
	size_t num = hp->getNumParameter();
	for (size_t i=0; i<num; ++i) {
		removeOpParameter(hp->getParameter(i));
		
	}
}
//-----------------------------------------------------------------------------
std::string ParameterConnection::getConnectionOpName(size_t index) {
    if (index>operators.size()) {
        SAMBAG_LOG_WARN<<"ParameterConnection::getConnectionOpName() out of bounds";
        return "";
    }
    return operators[index]->getName();
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
	std::tie(it, end) = parameters.equal_range(key);
	out.reserve(parameters.count(key));
	for (; it!=end; ++it) {
		out.push_back(it->second);
	}
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
