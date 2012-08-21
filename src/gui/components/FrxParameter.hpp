/*
 * FrxParameter.hpp
 *
 *  Created on: Mon Aug 20 10:45:54 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXPARAMETER_H
#define SAMBAG_FRXPARAMETER_H

#include <boost/shared_ptr.hpp>
#include "FrxNode.hpp"

namespace frx { namespace gui { namespace components {
namespace sdc = sambag::disco::components;
namespace sdcu = sdc::ui;
//=============================================================================
/** 
  * @class FrxParameter.
  * TODO: subclass FrxKnob
  */
class FrxParameter : public FrxNode {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef FrxNode Super;
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxParameter> Ptr;
	//-------------------------------------------------------------------------
	virtual sdcu::AComponentUIPtr getComponentUI(sdcu::ALookAndFeelPtr laf) const;
protected:
	//-------------------------------------------------------------------------
	FrxParameter();
private:
public:
}; // FrxParameter
}}} // namespace(s)

#endif /* SAMBAG_FRXPARAMETER_H */
