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
#include <boost/regex.hpp>


/**
 * helper macros for default processors
 */
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
    typedef boost::function<Product(IHostInfo::Ptr)> CreatorDefault;
    typedef boost::function<Product(IHostInfo::Ptr,int, int)> CreatorWithIO;
    typedef boost::function<Product(IHostInfo::Ptr, std::string)> CreatorWithDetail;
    typedef std::string Id;
    struct CreatorFunctions {
        CreatorDefault _defaultF;
        CreatorWithIO withIOF;
        CreatorWithDetail detailF;
        CreatorFunctions(const CreatorDefault &f) : _defaultF(f) {}
        CreatorFunctions(const CreatorWithIO &f) : withIOF(f) {}
        CreatorFunctions(const CreatorWithDetail &f) : detailF(f) {}
        bool set(const CreatorDefault &f) {
            if (_defaultF) {
                return false;
            }
            _defaultF = f;
            return true;
        }
        bool set(const CreatorWithIO &f) {
            if (_defaultF) {
                return false;
            }
            withIOF = f;
            return true;
        }
        bool set(const CreatorWithDetail &f) {
            if (_defaultF) {
                return false;
            }
            detailF = f;
            return true;
        }
    };
    typedef boost::unordered_map<Id, CreatorFunctions> CreatorMap;
    typedef boost::function<void(com::oArchive*)> OArchiveRegisterF;
    typedef boost::function<void(com::iArchive*)> IArchiveRegisterF;
    typedef std::list<OArchiveRegisterF> OARegList;
    typedef std::list<IArchiveRegisterF> IARegList;
protected:
private:
    //-------------------------------------------------------------------------
    CreatorMap creators;
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
    //-------------------------------------------------------------------------
    template <class Creator>
    bool _register(const Id &id, const Creator &creator) {
        CreatorMap::iterator it = creators.find(id);
        if (it==creators.end()) {
            return creators.insert(
                CreatorMap::value_type(id, CreatorFunctions(creator))
            ).second;
        }
        return it->second.set(creator);
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
     * frx.processing.unknown-plugin.Plugin('/home/plugins/plugin.vst')
     * frx.processing.briged-plugin.Plugin('/home/plugins/plugin.vst')
     * frx.processing.vst2x.Plugin('/home/plugins/plugin.vst')
     * frx.processing.au.Plugin('/home/plugins/plugin.au')
     */
    Product create(const std::string &pdStr, IHostInfo::Ptr hI);
    //-------------------------------------------------------------------------
    template <class T>
    boost::shared_ptr<T> create(const std::string &pdStr, IHostInfo::Ptr hI) {
        return boost::dynamic_pointer_cast<T>( create(pdStr, hI) );
    }
    //-------------------------------------------------------------------------
    /**
     * @brief registeres creator for id and ConcreteProd for archive
     */
    template <class ConcreteProd>
    bool register_(const Id &id, const CreatorDefault &creator) {
        return _register(id, creator) && registerArchives<ConcreteProd>();
    }
    //-------------------------------------------------------------------------
    /**
     * @brief registeres creator for id and ConcreteProd for archive
     */
    template <class ConcreteProd>
    bool registerWithIO(const Id &id, const CreatorWithIO &creator) {
       return _register(id, creator) && registerArchives<ConcreteProd>();
    }
    //-------------------------------------------------------------------------
    /**
     * @brief registeres creator for id and ConcreteProd for archive
     */
    template <class ConcreteProd>
    bool registerWithDetail(const Id &id, const CreatorWithDetail &creator) {
        return _register(id, creator) && registerArchives<ConcreteProd>();
    }
    //-------------------------------------------------------------------------
    /**
     * @brief registeres creator for id
     */
    bool register_(const Id &id, const CreatorDefault &creator) {
        return _register(id, creator);
    }
    //-------------------------------------------------------------------------
    /**
     * @brief registeres creator for id
     */
    bool registerWithIO(const Id &id, const CreatorWithIO &creator) {
       return _register(id, creator);
    }
    //-------------------------------------------------------------------------
    /**
     * @brief registeres creator for id
     */
    bool registerWithDetail(const Id &id, const CreatorWithDetail &creator) {
        return _register(id, creator);
    }
    //-------------------------------------------------------------------------
    void registerToArchive(com::iArchive &ar) const;
    //-------------------------------------------------------------------------
    void registerToArchive(com::oArchive &ar) const;
    //-------------------------------------------------------------------------
    size_t getNumRegisteredIds() const {
        return creators.size();
    }
    //-------------------------------------------------------------------------
    template <class Container>
    void getRegisteredIds(Container &out, const std::string &filter="") const {
        BOOST_FOREACH(const CreatorMap::value_type &v, creators)
        {
            if (filter.length()==0 ||
                boost::regex_match(v.first, boost::regex(filter)))
            {
                out.push_back(v.first);
            }
        }
    }

}; // ModelFactory
}} // namespace(s)

#endif /* SAMBAG_MODELFACTORY_H */




