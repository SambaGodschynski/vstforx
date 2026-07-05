/*
 * IViewModelMap.hpp
 *
 *  Created on: Fri Oct  5 13:38:18 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_IVIEWMODELMAP_H
#define SAMBAG_IVIEWMODELMAP_H

#include <memory>
#include <processing/ModelObject.hpp>
#include "components/Forward.hpp"
#include "ViewObject.hpp"

namespace frx { namespace gui {
//=============================================================================
/** 
  * @class IViewModelMap.
  */
class IViewModelMap {
//=============================================================================
private:
	///////////////////////////////////////////////////////////////////////////
	// Archive:
	//-------------------------------------------------------------------------
	friend class boost::serialization::access;
	//-------------------------------------------------------------------------
	template <typename Archive> 
	void serialize(Archive &ar, const unsigned int version) {}
public:
	//-------------------------------------------------------------------------
	typedef std::shared_ptr<IViewModelMap> Ptr;
	//-------------------------------------------------------------------------
	/**
	 * @param viewobject
	 * @return related model object
	 * @throw when map is closed
	 */
	virtual processing::ModelObject::Ptr 
	getModelObject(ViewObject::Ptr obj) = 0;
	//-------------------------------------------------------------------------
	/**
	 * @param modelobject
	 * @return related view object
	 * @throw when map is closed
	 */
	virtual ViewObject::Ptr 
	getViewObject(frx::processing::ModelObject::Ptr obj) = 0;
	//-------------------------------------------------------------------------
	/**
	 * @throw when map is closed or one of the arguments == NULL
	 */
	virtual bool registerObjects(ViewObject::Ptr vobj,
		frx::processing::ModelObject::Ptr mobj) = 0;
	//-------------------------------------------------------------------------
	/**
	 * @throw when map is closed
	 */
	virtual void remove(ViewObject::Ptr vobj,
		frx::processing::ModelObject::Ptr mobj) = 0;
	//-------------------------------------------------------------------------
	/**
	 * If the map closed no object releation can be read or written.
	 * This is the case e.g. when the viewobjects
	 * are persisted and the editor is closed. 
	 * @return true if map is closed.
	 */
	virtual bool isLocked() const = 0;
	//-------------------------------------------------------------------------
	/**
	 * @return number of registered relations
	 */
	virtual size_t getSize() const = 0;
	//-------------------------------------------------------------------------
	virtual std::string toString() const = 0;
}; // IViewModelMap
///////////////////////////////////////////////////////////////////////////////
extern IViewModelMap::Ptr 
getViewModelMap(components::FrxCircuidViewPtr view);
}} // namespace(s)

#endif /* SAMBAG_IVIEWMODELMAP_H */
