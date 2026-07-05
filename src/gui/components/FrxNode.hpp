/*
 * FrxNode.hpp
 *
 *  Created on: Mon Aug 20 10:43:58 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXNODE_H
#define SAMBAG_FRXNODE_H

#include <memory>
#include "FrxComponent.hpp"
#include <gui/HandyNamespaces.hpp>
#include <unordered_map>
#include <sambag/com/events/PropertyChanged.hpp>

namespace frx { namespace gui { namespace components {
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
	typedef std::shared_ptr<FrxNode> Ptr;
	//-------------------------------------------------------------------------
	virtual sdcu::AComponentUIPtr createComponentUI(sdcu::ALookAndFeelPtr laf) const;
protected:
	//-------------------------------------------------------------------------
	typedef sce::EventSender<sce::PropertyChanged>::Connection ChildSender;
	//-------------------------------------------------------------------------
	typedef std::unordered_map<sdc::AComponent::Ptr, ChildSender>
		ChildSenderMap;
	ChildSenderMap childSenderMap;
	//-------------------------------------------------------------------------
	FrxNode();
	//-------------------------------------------------------------------------
	virtual void updateChildLocation(sdc::AComponent::Ptr c);
	//-------------------------------------------------------------------------
	virtual void onChildChanged(void *src, 
		const sce::PropertyChanged &ev, sdc::AComponentWPtr c);
	//-------------------------------------------------------------------------
	void installChildListener(sdc::AComponent::Ptr c);
	//-------------------------------------------------------------------------
	void uninstallChildListener(sdc::AComponent::Ptr c);
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
	/**
	 * @override
	 * redraws parent containers area which fits to the
	 * the bounding rect of this object.
	 */
	virtual void redraw();
	//-------------------------------------------------------------------------
	/**
	 * @override
	 */
	virtual void setBounds(const sd::Rectangle &r);
	//-------------------------------------------------------------------------
	/**
	 * add component to node. object will be paced at node center.
	 */
	virtual void add(sdc::AComponent::Ptr);
	//-------------------------------------------------------------------------
	virtual void remove(sdc::AComponent::Ptr);
	//-------------------------------------------------------------------------
	virtual sambag::com::Number getRadius() const;
	//-------------------------------------------------------------------------
	SAMBAG_STD_STATIC_COMPONENT_CREATOR(FrxNode)
}; // FrxNode
}}} // namespace(s)

#endif /* SAMBAG_FRXNODE_H */
