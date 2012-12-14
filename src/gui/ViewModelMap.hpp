/*
 * ModelMap.hpp
 *
 *  Created on: Wed Oct 10 12:06:10 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_MODELMAP_H
#define SAMBAG_MODELMAP_H

#include <boost/shared_ptr.hpp>
#include "IViewModelMap.hpp"
#include <boost/bimap.hpp> 
#include <iostream>
#include <list>
#include <boost/serialization/list.hpp> 
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/weak_ptr.hpp>
#include <boost/foreach.hpp>
#include <sambag/com/exceptions/IllegalStateException.hpp>
#include <boost/serialization/access.hpp>

namespace frx { namespace gui {
//=============================================================================
/** 
  * @class ModelMap.
  */
class ViewModelMap : public IViewModelMap {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<ViewModelMap> Ptr;
protected:
	//-------------------------------------------------------------------------
	ViewModelMap();
private:
	///////////////////////////////////////////////////////////////////////////
	// Archive:
	//-------------------------------------------------------------------------
	friend class boost::serialization::access;
	//-------------------------------------------------------------------------
	template <typename Archive> 
	void serialize(Archive &ar, const unsigned int version) {
		ar & boost::serialization::base_object<IViewModelMap> ( *this );
		ar & bedroom;
		ar & closed;
	}
	///////////////////////////////////////////////////////////////////////////
	//-------------------------------------------------------------------------
	/**
	 * place where model objects are when map is locked.
	 */ 
	typedef std::list<frx::processing::ModelObject::Ptr> ModelBedroom;
	//-------------------------------------------------------------------------
	ModelBedroom bedroom;
	//-------------------------------------------------------------------------
	/**
     * throws if closed.
	 */
	void checkState();
	//-------------------------------------------------------------------------
	bool closed;
	//-------------------------------------------------------------------------
	typedef boost::bimap<ViewObject::Ptr,
		frx::processing::ModelObject::Ptr> Map;
	//-------------------------------------------------------------------------
	Map map;
public:
	//-------------------------------------------------------------------------
	static Ptr create();
	//-------------------------------------------------------------------------
	/**
	 * @param viewobject
	 * @return related model object
	 */
	virtual processing::ModelObject::Ptr getModelObject(ViewObject::Ptr obj);
	//-------------------------------------------------------------------------
	/**
	 * @param modelobject
	 * @return related view object
	 */
	virtual ViewObject::Ptr getViewObject(frx::processing::ModelObject::Ptr obj);
	//-------------------------------------------------------------------------
	virtual void registerObjects(ViewObject::Ptr vobj,
		frx::processing::ModelObject::Ptr mobj);
	//-------------------------------------------------------------------------
	virtual void remove(ViewObject::Ptr vobj,
		frx::processing::ModelObject::Ptr mobj);
	//-------------------------------------------------------------------------
	/**
	 * If the map closed no object releation can be read or written.
	 * This is the case e.g. when the viewobjects
	 * are persisted and the editor is closed. 
	 * @return true if map is closed.
	 */
	virtual bool isLocked() const;
	//-------------------------------------------------------------------------
	/**
	 * @return number of registered relations
	 */
	size_t getSize() const;
	//-------------------------------------------------------------------------
	std::string toString() const;
	//-------------------------------------------------------------------------
	/**
	 * serializes ViewModels intro archive and locks map.
	 */
	template <class Archive>
	void lock(Archive &ar) {
		std::list<ViewObject::Ptr> l;
		BOOST_FOREACH(const Map::left_map::value_type &v, map.left) {
			l.push_back(v.first);
			bedroom.push_back(v.second);
		}
		ar & l;
		map.clear();
		closed = true;
	}
	//-------------------------------------------------------------------------
	/**
	 * deserializes ViewModels from archive and unlocks map.
	 */
	template <class Archive>
	void unlock(Archive &ar) {
		typedef std::list<ViewObject::Ptr> ViewList;
		ViewList l;
		ar>>l;
		if (l.size() != bedroom.size()) {
			SAMBAG_THROW(
				sambag::com::exceptions::IllegalStateException,
				"map unlock failed."
			);
		}
		ViewList::const_iterator vit = l.begin();
		ModelBedroom::const_iterator mit = bedroom.begin();
		while(vit!=l.end()) {
			map.insert(Map::value_type(*vit, *mit));
			++vit;
			++mit;
		}
		closed = false;
		bedroom.clear();
	}
	//-------------------------------------------------------------------------

}; // ModelMap
}} // namespace(s)

#endif /* SAMBAG_MODELMAP_H */
