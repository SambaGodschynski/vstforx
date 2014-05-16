/*
 * ViewFactory.cpp
 *
 *  Created on: Wed Dec 18 10:05:20 2013
 *      Author: Johannes Unger
 */

#include "ViewFactory.hpp"
#include <com/one4All.h>
#include <sambag/com/exceptions/IllegalArgumentException.hpp>
#include <boost/foreach.hpp>

namespace frx { namespace gui { namespace components {
typedef Loki::SingletonHolder<ViewFactory> ViewFactoryHolder;
//=============================================================================
//  Class ViewFactory
//=============================================================================
//-----------------------------------------------------------------------------
ViewFactory & ViewFactory::instance() {
	return ViewFactoryHolder::Instance();
}
//-----------------------------------------------------------------------------
ViewFactory::Product ViewFactory::create(const std::string &pdStr)
{
    SAMBAG_LOG_INFO<<"ViewFactory create: " << pdStr;
    com::IdParser descr(pdStr);
    if (descr==com::FRX_NULL_ID || descr.namespace_()!="gui" ) {
        SAMBAG_THROW(
            sambag::com::exceptions::IllegalArgumentException,
            pdStr + " not found"
        );
    }
    std::string id = descr.type() + "." + descr.name();
    CreatorMap::const_iterator it =
        creators.find(id);
    if (it==creators.end()) {
        SAMBAG_THROW(
            sambag::com::exceptions::IllegalArgumentException,
            pdStr + " not found"
        );
    }
    return it->second();
}
//-----------------------------------------------------------------------------
void ViewFactory::registerToArchive(com::iArchive &ar) const {
    BOOST_FOREACH(const IArchiveRegisterF &f, iaregs) {
        f(&ar);
    }
}
//-----------------------------------------------------------------------------
void ViewFactory::registerToArchive(com::oArchive &ar) const {
    BOOST_FOREACH(const OArchiveRegisterF &f, oaregs) {
        f(&ar);
    }
}

}}} // namespace(s)
