/*
 * FrxConnection.hpp
 *
 *  Created on: Mon Aug 20 10:44:56 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXCONNECTION_H
#define SAMBAG_FRXCONNECTION_H

#include <boost/shared_ptr.hpp>
#include <sambag/disco/components/ui/ALookAndFeel.hpp>
#include "FrxComponent.hpp"
#include <sambag/com/events/PropertyChanged.hpp>
#include <gui/HandyNamespaces.hpp>

namespace frx { namespace gui { namespace components {
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
private:
	//-------------------------------------------------------------------------
	mutable std::string tmpName;
	//-------------------------------------------------------------------------
	FrxComponent::Ptr src, dst;
	//-------------------------------------------------------------------------
	typedef sce::EventSender<sce::PropertyChanged>::Connection Connection;
	//-------------------------------------------------------------------------
	Connection srcConnection, dstConnection;
	//-------------------------------------------------------------------------
	Connection connect(FrxComponent::Ptr c);
	///////////////////////////////////////////////////////////////////////////
	// Archive:
	//-------------------------------------------------------------------------
	friend class boost::serialization::access;
	//-------------------------------------------------------------------------
	template <typename Archive> 
	void serialize(Archive &ar, const unsigned int version) { 
		ar & boost::serialization::base_object<Super>(*this);
		ar & src;
		ar & dst;
        
        if (version>=1) {
            ar & tmpName;
        }
        
		if (Archive::is_loading::value) {
			installComponentListeners(src);
			installComponentListeners(dst);
			connect(src);
			connect(dst);
		}
	} 
protected:
	//-------------------------------------------------------------------------
	void onComponentsPropertyChanged(void*, const sce::PropertyChanged &ev);
	//-------------------------------------------------------------------------
	void onPropertyChanged(void*, const sce::PropertyChanged &ev);
	//-------------------------------------------------------------------------
	FrxConnection();
	//-------------------------------------------------------------------------
	void resetBounds();
	//-------------------------------------------------------------------------
	void onComponentRemoving(void *src, const OnRemoving &ev);
private:
	//-------------------------------------------------------------------------
	void installComponentListeners(FrxComponent::Ptr c);
	//-------------------------------------------------------------------------
	void installListeners();
public:
	//-------------------------------------------------------------------------
	/**
	 * @override for performance reasons
	 * @note: disables PropertyChanged notification.
	 */
	virtual void setBounds(const sd::Rectangle &r);
	//-------------------------------------------------------------------------
	virtual const std::string & getName() const;
	//-------------------------------------------------------------------------
	Ptr getPtr() const {
		return boost::dynamic_pointer_cast<FrxConnection>(Super::getPtr());
	}
	//-------------------------------------------------------------------------
	void setSrcComponent(FrxComponent::Ptr a);
	//-------------------------------------------------------------------------
	void setDstComponent(FrxComponent::Ptr b);
	//-------------------------------------------------------------------------
	FrxComponent::Ptr getSrcComponent() const { return src; }
	//-------------------------------------------------------------------------
	FrxComponent::Ptr getDstComponent() const { return dst; }
}; // FrxConnection
}}} // namespace(s)

BOOST_CLASS_VERSION(frx::gui::components::FrxConnection, 1)

#endif /* SAMBAG_FRXCONNECTION_H */
