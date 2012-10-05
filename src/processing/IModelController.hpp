/*
 * IModelController.hpp
 *
 *  Created on: Fri Oct  5 13:40:06 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_IMODELCONTROLLER_H
#define SAMBAG_IMODELCONTROLLER_H

#include <boost/shared_ptr.hpp>
#include <gui/components/Forward.hpp>
namespace frx { namespace processing {
//=============================================================================
/** 
  * @class IModelController.
  */
class IModelController {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<IModelController> Ptr;
	//-------------------------------------------------------------------------
}; // IModelController
extern IModelController *
getModelController(frx::gui::components::FrxCircuidViewPtr view);
}} // namespace(s)

#endif /* SAMBAG_IMODELCONTROLLER_H */
