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
#include <boost/bimap/unordered_set_of.hpp>
#include <com/Serialization.h>
#include <sambag/com/Thread.hpp>

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
	//-------------------------------------------------------------------------
	ViewModelMap(const ViewModelMap&) {}
private:
	///////////////////////////////////////////////////////////////////////////
	// Archive:
	//-------------------------------------------------------------------------
	friend class boost::serialization::access;
	//-------------------------------------------------------------------------
	void serialize(com::iArchive &ar, const unsigned int version);
	//-------------------------------------------------------------------------
	void serialize(com::oArchive &ar, const unsigned int version);
	///////////////////////////////////////////////////////////////////////////
	//-------------------------------------------------------------------------
	/**
	 * place where model objects are when map is locked.
	 */ 
	typedef std::list<frx::processing::ModelObject::Ptr> ModelBedroom;
	//-------------------------------------------------------------------------
	ModelBedroom bedroom;
    //-------------------------------------------------------------------------
    mutable sambag::com::RecursiveMutex mutex;
	//-------------------------------------------------------------------------
	typedef std::list<ViewObject::Ptr> ViewObjects;
	//-------------------------------------------------------------------------
	/**
     * throws if closed.
	 */
	void checkState();
	//-------------------------------------------------------------------------
	bool closed;
	//-------------------------------------------------------------------------
	typedef boost::bimap< 
		boost::bimaps::unordered_set_of<ViewObject::Ptr>,
		boost::bimaps::unordered_set_of<frx::processing::ModelObject::Ptr> 
	> Map;
	//-------------------------------------------------------------------------
	Map map;
public:
	//-------------------------------------------------------------------------
	virtual Ptr clone() const;
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
	virtual bool registerObjects(ViewObject::Ptr vobj,
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
	virtual std::string toString() const;
	//-------------------------------------------------------------------------
	/**
	 * serializes ViewModels intro archive and locks map.
	 */
	void lock(::com::oArchive &ar);
	//-------------------------------------------------------------------------
	/**
	 * deserializes ViewModels from archive and unlocks map.
	 */
	void unlock(::com::iArchive &ar);
	//-------------------------------------------------------------------------
    typedef std::vector<frx::processing::ModelObject::Ptr> ModelObjects;
    void getModelObjects(ModelObjects &out) const;
}; // ModelMap
}} // namespace(s)

#endif /* SAMBAG_MODELMAP_H */
