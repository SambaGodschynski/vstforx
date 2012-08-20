/*
 * FrxConnection.hpp
 *
 *  Created on: Mon Aug 20 10:44:56 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXCONNECTION_H
#define SAMBAG_FRXCONNECTION_H

#include <boost/shared_ptr.hpp>
#include "FrxComponent.hpp"

namespace frx { namespace gui { namespace components {
namespace sdc = sambag::disco::components;
namespace sdcu = sdc::ui;
//=============================================================================
/** 
  * @class FrxConnection.
  */
class FrxConnection : public FrxComponent {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef FrxComponent Super;
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxConnection> Ptr;
	//-------------------------------------------------------------------------
	virtual sdcu::AComponentUIPtr getComponentUI(sdcu::ALookAndFeelPtr laf) const;
protected:
	//-------------------------------------------------------------------------
	FrxConnection();
private:
public:
}; // FrxConnection
}}} // namespace(s)

#endif /* SAMBAG_FRXCONNECTION_H */
