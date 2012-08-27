/*
 * FrxIO.hpp
 *
 *  Created on: Mon Aug 27 10:35:21 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXIO_H
#define SAMBAG_FRXIO_H

#include <boost/shared_ptr.hpp>
#include "FrxNode.hpp"

namespace frx { namespace gui { namespace components {
namespace sdc = sambag::disco::components;
namespace sdcu = sdc::ui;
//=============================================================================
/** 
  * @class FrxIO.
  */
class FrxIO : public FrxNode {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef FrxNode Super;
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxIO> Ptr;
protected:
	//-------------------------------------------------------------------------
	FrxIO(){}
private:
public:
}; // FrxIO
}}} // namespace(s)

#endif /* SAMBAG_FRXIO_H */
