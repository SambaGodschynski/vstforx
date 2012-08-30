/*
 * FrxParameter.hpp
 *
 *  Created on: Mon Aug 20 10:45:54 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXPARAMETER_H
#define SAMBAG_FRXPARAMETER_H

#include <boost/shared_ptr.hpp>
#include <sambag/com/Common.hpp>
#include "FrxNode.hpp"
#include <string>

namespace frx { namespace gui { namespace components {
namespace sd = sambag::disco;
namespace sdc = sd::components;
namespace sdcu = sdc::ui;


//=============================================================================
/** 
  * @class FrxParameter.
  * Encapsulates a parameter control element.
  */
class FrxParameter : public FrxNode {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	static const std::string PROPERTY_ENC_CTRL;
	//-------------------------------------------------------------------------
	typedef FrxNode Super;
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxParameter> Ptr;
	//-------------------------------------------------------------------------
	virtual sdcu::AComponentUIPtr createComponentUI(sdcu::ALookAndFeelPtr laf) const;
protected:
	//-------------------------------------------------------------------------
	sdc::AComponent::Ptr ctrl;
	//-------------------------------------------------------------------------
	FrxParameter();
	//-------------------------------------------------------------------------
	virtual void postConstructor();
private:
public:
	//-------------------------------------------------------------------------
	void setEncapsulatedCtrl(sdc::AComponent::Ptr ctrl);
	//-------------------------------------------------------------------------
	sdc::AComponent::Ptr getEncapsulatedCtrl() const { return ctrl; }
	//-------------------------------------------------------------------------
	Ptr getPtr() const {
		return boost::shared_dynamic_cast<FrxParameter>(Super::getPtr());
	}

}; // FrxParameter
}}} // namespace(s)

#endif /* SAMBAG_FRXPARAMETER_H */
