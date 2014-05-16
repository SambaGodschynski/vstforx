/*
 * ModelFactory.cpp
 *
 *  Created on: Wed Dec 18 10:05:14 2013
 *      Author: Johannes Unger
 */

#include "ModelFactory.hpp"
#include <com/one4All.h>
#include <sambag/com/exceptions/IllegalArgumentException.hpp>
#include <boost/foreach.hpp>


namespace frx { namespace processing {
typedef Loki::SingletonHolder<ModelFactory> ModelFactoryHolder;
//=============================================================================
//  Class ModelFactory
//=============================================================================
//-----------------------------------------------------------------------------
ModelFactory & ModelFactory::instance() {
	return ModelFactoryHolder::Instance();
}
//-----------------------------------------------------------------------------
ModelFactory::Product ModelFactory::create(const std::string &pdStr,
    IHostInfo::Ptr hI)
{
    using sambag::com::exceptions::IllegalArgumentException;
    SAMBAG_LOG_INFO<<"ModelFactory create: " << pdStr;
    com::IdParser descr(pdStr);
    if (descr==com::FRX_NULL_ID || descr.namespace_()!="processing" ) {
        SAMBAG_THROW(IllegalArgumentException, pdStr + " not found");
    }
    std::string id = descr.type() + "." + descr.name();
    CreatorMap::const_iterator it = creators.find(id);
    if (it==creators.end()) {
        SAMBAG_THROW(IllegalArgumentException, pdStr + " not found");
    }
    
    if (it->second.detailF && descr.details().length() > 0 ) {
        return it->second.detailF(hI, descr.details());
    }
    if (it->second.withIOF && descr.numInputs() >= 0 || descr.numOutputs() >= 0) {
        return it->second.withIOF(hI, descr.numInputs(), descr.numOutputs());
    }
    if (!it->second._defaultF) {
        SAMBAG_THROW(IllegalArgumentException, pdStr + " no creator found");
    }
    return it->second._defaultF(hI);
}
//-----------------------------------------------------------------------------
void ModelFactory::registerToArchive(com::iArchive &ar) const {
    BOOST_FOREACH(const IARegList::value_type &x, iaregs) {
        x.second(&ar);
    }
}
//-----------------------------------------------------------------------------
void ModelFactory::registerToArchive(com::oArchive &ar) const {
    BOOST_FOREACH(const OARegList::value_type &x, oaregs) {
        x.second(&ar);
    }
}
//-----------------------------------------------------------------------------
void ModelFactory::registerToArchive(com::iArchive &ar, const Id &id) const {
	IARegList::const_iterator it = iaregs.find(id);
	if (it==iaregs.end()) {
		SAMBAG_LOG_WARN<<"registerToArchive: "<<id<<" not found";
		return;
	}
	it->second(&ar);
}
//-----------------------------------------------------------------------------
void ModelFactory::registerToArchive(com::oArchive &ar, const Id &id) const {
  	OARegList::const_iterator it = oaregs.find(id);
	if (it==oaregs.end()) {
		SAMBAG_LOG_WARN<<"registerToArchive: "<<id<<" not found";
		return;
	}
	it->second(&ar);
}
}} // namespace(s)
