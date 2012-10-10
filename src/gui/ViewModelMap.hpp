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
	virtual bool isClosed() const;
	//-------------------------------------------------------------------------
	/**
	 * @return number of registered relations
	 */
	size_t getSize() const;
}; // ModelMap
}} // namespace(s)

#endif /* SAMBAG_MODELMAP_H */
