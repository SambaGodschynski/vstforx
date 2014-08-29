/*
 * LuaUserData.hpp
 *
 *  Created on: Fri Aug 29 10:43:36 2014
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_LUAUSERDATA_H
#define SAMBAG_LUAUSERDATA_H

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ptree_serialization.hpp>
#include <boost/foreach.hpp>


namespace frx { namespace scripts {
//=============================================================================
/** 
  * @class LuaUserData.
  * @brief Set and get userdata for a luastate.
  */
class LuaUserData {
//=============================================================================
public:
    typedef boost::property_tree::ptree Container;
protected:
private:
    typedef boost::property_tree::ptree_error ContainerEx;
    Container container;
    ///////////////////////////////////////////////////////////////////////////
	// Archive:
	//-------------------------------------------------------------------------
	friend class boost::serialization::access;
	template <typename Archive> 
	void serialize(Archive &ar, const unsigned int version) { 
		ar & container; 
	} 
public:
    //-------------------------------------------------------------------------
    const Container & getContainer() const {
        return container;
    }
    //-------------------------------------------------------------------------
    void add(const std::string &key, const std::string &value);
    //-------------------------------------------------------------------------
    template <class STL>
    void get(const std::string &key, STL &out) const;
    //-------------------------------------------------------------------------
    /**
     * @brief migrates data from map to user data. 
     */
    template <class Map>
    void migrate(const Map &map);
}; // LuaUserData

///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
template <class STL>
void LuaUserData::get(const std::string &key, STL &out) const {
    try {
        BOOST_FOREACH(const Container::value_type &v, container.get_child(key))
        {
            if (!v.second.data().empty()) {
                out.insert(out.end(), v.second.data());
            }
        }
    } catch (const ContainerEx &) {}
}
//-----------------------------------------------------------------------------
template <class Map>
void LuaUserData::migrate(const Map &map) {
    BOOST_FOREACH(const typename Map::value_type &v, map) {
        add(v.first, v.second);
    }
}
}} // namespace(s)
#endif /* SAMBAG_LUAUSERDATA_H */
