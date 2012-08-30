/*
 * FrxParameterMouseListener.hpp
 *
 *  Created on: Wed Aug 29 14:03:47 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXPARAMETERMOUSELISTENER_H
#define SAMBAG_FRXPARAMETERMOUSELISTENER_H

#include <boost/shared_ptr.hpp>
#include "FrxNodeMouseListener.hpp"

namespace frx { namespace gui {
namespace components { namespace ui { 
//=============================================================================
/** 
  * @class FrxParameterMouseListener.
  */
class FrxParameterMouseListener : public FrxNodeMouseListener {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxParameterMouseListener> Ptr;
protected:
	//-------------------------------------------------------------------------
	virtual void use(const sdc::events::MouseEvent &ev);
private:
public:
}; // FrxParameterMouseListener
}}}} // namespace(s)

#endif /* SAMBAG_FRXPARAMETERMOUSELISTENER_H */
