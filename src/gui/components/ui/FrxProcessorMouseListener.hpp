/*
 * FrxProcessorMouseListener.hpp
 *
 *  Created on: Mon Aug 27 20:10:03 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXPROCESSORMOUSELISTENER_H
#define SAMBAG_FRXPROCESSORMOUSELISTENER_H

#include <boost/shared_ptr.hpp>
#include "FrxNodeMouseListener.hpp"
namespace frx { namespace gui {
namespace components { namespace ui { 
//=============================================================================
/** 
  * @class FrxProcessorMouseListener.
  */
class FrxProcessorMouseListener : public FrxNodeMouseListener{
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxProcessorMouseListener> Ptr;
protected:
	//-------------------------------------------------------------------------
	virtual void beginConnecting(const sdc::events::MouseEvent &ev) {}
	//-------------------------------------------------------------------------
	virtual void connecting(const sdc::events::MouseEvent &ev) {}
	//-------------------------------------------------------------------------
	virtual void endConnecting(const sdc::events::MouseEvent &ev) {}
private:
public:
}; // FrxProcessorMouseListener
}}}} // namespace(s)

#endif /* SAMBAG_FRXPROCESSORMOUSELISTENER_H */
