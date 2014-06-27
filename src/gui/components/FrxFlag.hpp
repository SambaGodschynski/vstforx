/*
 * FrxFlag.hpp
 *
 *  Created on: Sun Dec 23 14:03:14 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXFLAG_H
#define SAMBAG_FRXFLAG_H

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include "FrxComponent.hpp"
#include <gui/HandyNamespaces.hpp>

namespace frx { namespace gui { namespace components {
//=============================================================================
/** 
  * @class FrxFlag.
  */
class FrxFlag : public FrxComponent {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxFlag> Ptr;
	//-------------------------------------------------------------------------
	typedef boost::weak_ptr<FrxFlag> WPtr;
	//-------------------------------------------------------------------------
	typedef FrxComponent Super;
	//-------------------------------------------------------------------------
	static const std::string PROPERTY_TARGET;
protected:
	//-------------------------------------------------------------------------
	FrxFlag(){}
	//-------------------------------------------------------------------------
	void onComponentRemoving(void *src, const OnRemoving &ev);
	//-------------------------------------------------------------------------
	void onTargetProperty(const sce::PropertyChanged &ev);
    //-------------------------------------------------------------------------
    void installListeners();
private:
	//-------------------------------------------------------------------------
	typedef sce::EventSender<OnRemoving>::Connection RemovingConnection;
	RemovingConnection rmvConnection;
	typedef sce::EventSender<sce::PropertyChanged>::Connection PropertyEvConnection;
	PropertyEvConnection propertyConnection;
	//-------------------------------------------------------------------------
	FrxComponent::Ptr target;
	//-------------------------------------------------------------------------
	friend class boost::serialization::access;
	//-------------------------------------------------------------------------
	template <typename Archive> 
	void serialize(Archive &ar, const unsigned int version) {
		ar & boost::serialization::base_object<Super>(*this); 
		ar & target;
        if (Archive::is_loading::value) {
            installListeners();
        }
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
	virtual ~FrxFlag();
	//-------------------------------------------------------------------------
	void setTarget(FrxComponent::Ptr target);
	//-------------------------------------------------------------------------
	FrxComponent::Ptr getTarget() const {
		return target;
	}
	//-------------------------------------------------------------------------
	virtual sdcu::AComponentUIPtr
	createComponentUI(sdcu::ALookAndFeelPtr laf) const;
	//-------------------------------------------------------------------------
	SAMBAG_STD_STATIC_COMPONENT_CREATOR(FrxFlag)
}; // FrxFlag
}}} // namespace(s)

#endif /* SAMBAG_FRXFLAG_H */
