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

struct lua_State;

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
    /**
     * @brief clears all data
     */
    void clear();
    //-------------------------------------------------------------------------
    /**
     * @brief removes values of a key.
     */
    void remove(const std::string &key);
    //-------------------------------------------------------------------------
    /**
     * @brief adds a value to a key
     */
    void add(const std::string &key, const std::string &value);
    //-------------------------------------------------------------------------
    /**
     * @brief pops and migrates key,value data from lua stack.
     */
    void add(lua_State *lua);
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
		if (container.count(key) == 0) {
			return;
		}
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
