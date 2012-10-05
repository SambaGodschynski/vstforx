/*
 * IViewModelMap.hpp
 *
 *  Created on: Fri Oct  5 13:38:18 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_IVIEWMODELMAP_H
#define SAMBAG_IVIEWMODELMAP_H

#include <boost/shared_ptr.hpp>
#include <processing/ModelObject.hpp>
#include "ViewObject.hpp"

namespace frx { namespace gui {
//=============================================================================
/** 
  * @class IViewModelMap.
  */
class IViewModelMap {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<IViewModelMap> Ptr;
	//-------------------------------------------------------------------------
	virtual processing::ModelObject::Ptr getModelObject(ViewObject::Ptr obj) = 0;
	//-------------------------------------------------------------------------
	virtual ViewObject::Ptr getViewObject(frx::processing::ModelObject::Ptr obj) = 0;
}; // IViewModelMap
}} // namespace(s)

#endif /* SAMBAG_IVIEWMODELMAP_H */
