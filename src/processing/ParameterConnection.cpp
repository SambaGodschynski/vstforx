/*
 * ParameterConnection.cpp
 *
 *  Created on: Wed Oct 24 17:17:56 2012
 *      Author: Johannes Unger
 */

#include "ParameterConnection.hpp"

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
}} // namespace(s)
