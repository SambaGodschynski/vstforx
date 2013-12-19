/*
 * ModelFactory.hpp
 *
 *  Created on: Wed Dec 18 10:05:14 2013
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_MODELFACTORY_H
#define SAMBAG_MODELFACTORY_H

#include <loki/Singleton.h>
#include "IHostInfo.h"
#include "processing.h"
#include <boost/function.hpp>
#include <boost/unordered_map.hpp>
#include <boost/shared_ptr.hpp>
#include <com/Serialization.h>
#include <list>
#include <boost/foreach.hpp>

#define FRX_MODELFACTORY_REGISTER(type,prod)                                   \
    namespace { const bool type ## prod =                                      \
            frx::processing::ModelFactory::instance().register_<prod>(         \
                std::string(#type) + "." + #prod, &prod::create                \
        );}

#define FRX_MODELFACTORY_REGISTER_IO(type,prod)                                \
    namespace { const bool type ## prod =                                      \
            frx::processing::ModelFactory::instance().registerWithIO<prod>(    \
                std::string(#type) + "." + #prod, &prod::create                \
        );}

#define FRX_MODELFACTORY_REGISTER_DETAILS(type,prod)                              \
    namespace { const bool type ## prod =                                         \
            frx::processing::ModelFactory::instance().registerWithDetail<prod>(   \
                std::string(#type) + "." + #prod, &prod::create                   \
        );}


namespace frx { namespace processing {
//=============================================================================
/** 
  * @class ModelFactory.
  */
class ModelFactory {
//=============================================================================
friend struct Loki::CreateUsingNew<ModelFactory>;
public:
    //-------------------------------------------------------------------------
    typedef ::processing::ProcessAdapter::Ptr Product;
    typedef boost::function<Product(IHostInfo::Ptr)> Creator;
    typedef boost::function<Product(IHostInfo::Ptr,int, int)> CreatorWithIO;
    typedef boost::function<Product(IHostInfo::Ptr, std::string)> CreatorWithDetail;
    typedef std::string Id;
    typedef boost::unordered_map<Id, Creator> CreatorMap;
    typedef boost::unordered_map<Id, CreatorWithIO> CreatorWithIOMap;
    typedef boost::unordered_map<Id, CreatorWithDetail> CreatorWithDetailMap;
    typedef boost::function<void(com::oArchive*)> OArchiveRegisterF;
    typedef boost::function<void(com::iArchive*)> IArchiveRegisterF;
    typedef std::list<OArchiveRegisterF> OARegList;
    typedef std::list<IArchiveRegisterF> IARegList;
protected:
private:
    //-------------------------------------------------------------------------
    CreatorMap           creators;
    CreatorWithIOMap     creatorsIO;
    CreatorWithDetailMap creatorsDetail;
    OARegList oaregs;
    IARegList iaregs;
    //-------------------------------------------------------------------------
    template <class Archive, class ConcreteProd>
    static void toArchive(Archive *ar) {
        ar->template register_type<ConcreteProd>();
    }
    //-------------------------------------------------------------------------
    template <class ConcreteProd>
    bool registerArchives() {
        oaregs.push_back( &toArchive<com::oArchive, ConcreteProd> );
        iaregs.push_back( &toArchive<com::iArchive, ConcreteProd> );
        return true;
    }
public:
	//-------------------------------------------------------------------------
	static ModelFactory & instance();
    //-------------------------------------------------------------------------
    /**
     * @brief creates object using uri.
     * @param plugin description string see com::Descriptor
     * examples:
     * frx.processing.vst2x.FrxTestplugin(2,2)
     * frx.processing.internal.FrxADSR
     * frx.processing.unknown-plugin.location('/home/plugins/plugin.vst')
     * frx.processing.briged-plugin.location('/home/plugins/plugin.vst')
     * frx.processing.vst2x.location('/home/plugins/plugin.vst')
     * frx.processing.au.location('/home/plugins/plugin.au')
     */
    Product create(const std::string &pdStr, IHostInfo::Ptr hI);
    //-------------------------------------------------------------------------
    template <class ConcreteProd>
    bool register_(const Id &id, const Creator &creator) {
        return
            creators.insert(
                CreatorMap::value_type(id, creator)
            ).second && registerArchives<ConcreteProd>();
    }
    //-------------------------------------------------------------------------
    template <class ConcreteProd>
    bool registerWithIO(const Id &id, const CreatorWithIO &creator) {
        return 
            creatorsIO.insert(
                CreatorWithIOMap::value_type(id, creator)
            ).second && registerArchives<ConcreteProd>();
    }
    //-------------------------------------------------------------------------
    template <class ConcreteProd>
    bool registerWithDetail(const Id &id, const CreatorWithDetail &creator) {
        return
            creatorsDetail.
            insert(
                CreatorWithDetailMap::value_type(id, creator)
            ).second && registerArchives<ConcreteProd>();
    }
    //-------------------------------------------------------------------------
    void registerToArchive(com::iArchive &ar) const;
    //-------------------------------------------------------------------------
    void registerToArchive(com::oArchive &ar) const;
    //-------------------------------------------------------------------------
    size_t getNumRegisteredIds() const {
        return creators.size() +
               creatorsIO.size() +
               creatorsDetail.size();
    }
    //-------------------------------------------------------------------------
    template <class Container>
    void getRegisteredIds(Container &out) const {
        BOOST_FOREACH(const CreatorMap::value_type &v, creators)
        {
            out.push_back(v.first);
        }
        BOOST_FOREACH(const CreatorWithIOMap::value_type &v, creatorsIO)
        {
            out.push_back(v.first);
        }
        BOOST_FOREACH(const CreatorWithDetailMap::value_type &v, creatorsDetail)
        {
            out.push_back(v.first);
        }
    }

}; // ModelFactory
}} // namespace(s)

#endif /* SAMBAG_MODELFACTORY_H */




