/*
 * FrxParameter.hpp
 *
 *  Created on: Mon Aug 20 10:45:54 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXPARAMETER_H
#define SAMBAG_FRXPARAMETER_H

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <sambag/com/Common.hpp>
#include <sambag/disco/components/DefaultBoundedRangeModel.hpp>
#include "FrxNode.hpp"
#include <string>
#include <gui/HandyNamespaces.hpp>

namespace frx { namespace gui { namespace components {
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
	static const std::string PROPERTY_RANGE_MODEL;
	//-------------------------------------------------------------------------
	typedef FrxNode Super;
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxParameter> Ptr;
	//-------------------------------------------------------------------------
	typedef boost::weak_ptr<FrxParameter> WPtr;
	//-------------------------------------------------------------------------
	virtual sdcu::AComponentUIPtr createComponentUI(sdcu::ALookAndFeelPtr laf) const;
protected:
	//-------------------------------------------------------------------------
	sdc::DefaultBoundedRangeModel::Ptr rangeModel;
	//-------------------------------------------------------------------------
	sdc::AComponent::Ptr ctrl;
	//-------------------------------------------------------------------------
	FrxParameter();
	//-------------------------------------------------------------------------
	virtual void postConstructor();
private:
	///////////////////////////////////////////////////////////////////////////
	// Archive:
	//-------------------------------------------------------------------------
	friend class boost::serialization::access;
	//-------------------------------------------------------------------------
	template <typename Archive> 
	void serialize(Archive &ar, const unsigned int version) { 
		ar & boost::serialization::base_object<Super>(*this); 
	} 
public:
	//-------------------------------------------------------------------------
	void setEncapsulatedCtrl(sdc::AComponent::Ptr ctrl);
	//-------------------------------------------------------------------------
	void setRangeModel(sdc::DefaultBoundedRangeModel::Ptr model);
	//-------------------------------------------------------------------------
	sdc::DefaultBoundedRangeModel::Ptr getRangeModel() const {
		return rangeModel;
	}
	//-------------------------------------------------------------------------
	sdc::AComponent::Ptr getEncapsulatedCtrl() const { return ctrl; }
	//-------------------------------------------------------------------------
	Ptr getPtr() const {
		return boost::dynamic_pointer_cast<FrxParameter>(Super::getPtr());
	}

}; // FrxParameter
}}} // namespace(s)

#endif /* SAMBAG_FRXPARAMETER_H */
