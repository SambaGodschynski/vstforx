/*
 * FrxNode.hpp
 *
 *  Created on: Mon Aug 20 10:43:58 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXNODE_H
#define SAMBAG_FRXNODE_H

#include <boost/shared_ptr.hpp>
#include "FrxComponent.hpp"

namespace frx { namespace gui { namespace components {
namespace sdc = sambag::disco::components;
namespace sdcu = sdc::ui;
//=============================================================================
/** 
  * @class FrxNode.
  */
class FrxNode : public FrxComponent {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef FrxComponent Super;
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxNode> Ptr;
	//-------------------------------------------------------------------------
	virtual sdcu::AComponentUIPtr createComponentUI(sdcu::ALookAndFeelPtr laf) const;
protected:
	//-------------------------------------------------------------------------
	FrxNode();
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
	SAMBAG_STD_STATIC_COMPONENT_CREATOR(FrxNode)
}; // FrxNode
}}} // namespace(s)

#endif /* SAMBAG_FRXNODE_H */
