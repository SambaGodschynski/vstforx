/*
 * ViewFactory.hpp
 *
 *  Created on: Wed Dec 18 10:05:20 2013
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_VIEWFACTORY_H
#define SAMBAG_VIEWFACTORY_H

#include <loki/Singleton.h>
#include <boost/function.hpp>
#include <boost/unordered_map.hpp>
#include <boost/shared_ptr.hpp>
#include <com/Serialization.h>
#include <list>
#include <gui/components/FrxProcessorNode.hpp>
#include <boost/foreach.hpp>

namespace frx { namespace gui { namespace components {
//=============================================================================
/** 
  * @class ViewFactory.
  */
class ViewFactory {
//=============================================================================
friend struct Loki::CreateUsingNew<ViewFactory>;
public:
    //-------------------------------------------------------------------------
    typedef FrxProcessorNode::Ptr Product;
    typedef boost::function<Product()> Creator;
    typedef std::string Id;
    typedef boost::unordered_map<Id, Creator> CreatorMap;
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
public:
	//-------------------------------------------------------------------------
	static ViewFactory & instance();
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
    Product create(const std::string &pdStr);
    //-------------------------------------------------------------------------
    template <class ConcreteProd>
    bool register_(const Id &id, const Creator &creator) {
        return
            creators.insert(
                CreatorMap::value_type(id, creator)
            ).second && registerArchives<ConcreteProd>();
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
    /**
     * @return registered ids without namespace: 
     * frx.gui.internal.XY => internal.XY
     */
    template <class Container>
    void getRegisteredIds(Container &out) const {
        BOOST_FOREACH(const CreatorMap::value_type &v, creators) {
            out.push_back(v.first);
        }
    }
}; // ViewFactory
}}} // namespace(s)

#endif /* SAMBAG_VIEWFACTORY_H */
