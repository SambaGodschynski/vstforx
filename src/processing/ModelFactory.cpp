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
namespace {
    void _throw(const std::string &str) {
        SAMBAG_THROW(
            sambag::com::exceptions::IllegalArgumentException,
            str
        );
    }
} // namespace
ModelFactory::Product ModelFactory::create(const std::string &pdStr,
    IHostInfo::Ptr hI)
{
    SAMBAG_LOG_INFO<<"ModelFactory create: " << pdStr;
    com::IdParser descr(pdStr);
    if (descr==com::FRX_NULL_ID || descr.namespace_()!="processing" ) {
        _throw(pdStr + " not found");
    }
    std::string id = descr.type() + "." + descr.name();
    if (descr.details().length() > 0 ) {
        CreatorWithDetailMap::const_iterator it =
            creatorsDetail.find(id);
        if (it==creatorsDetail.end()) {
            _throw(pdStr + " not found");
        }
        return it->second(hI, descr.details());
    }
    if (descr.numInputs() >= 0 || descr.numOutputs() >= 0) {
        CreatorWithIOMap::const_iterator it =
            creatorsIO.find(id);
        if (it==creatorsIO.end()) {
            _throw(pdStr + " not found");
        }
        return it->second(hI, descr.numInputs(), descr.numOutputs());
    }
    CreatorMap::const_iterator it =
        creators.find(id);
    if (it==creators.end()) {
        _throw(pdStr + " not found");
    }
    return it->second(hI);
}
//-----------------------------------------------------------------------------
void ModelFactory::registerToArchive(com::iArchive &ar) const {
    BOOST_FOREACH(const IArchiveRegisterF &f, iaregs) {
        f(&ar);
    }
}
//-----------------------------------------------------------------------------
void ModelFactory::registerToArchive(com::oArchive &ar) const {
    BOOST_FOREACH(const OArchiveRegisterF &f, oaregs) {
        f(&ar);
    }
}
}} // namespace(s)
