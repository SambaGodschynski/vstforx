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

namespace frx { namespace gui { namespace components {
namespace sdc = sambag::disco::components;
namespace sdcu = sdc::ui;
namespace sce = sambag::com::events;
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
	FrxComponent::Ptr frxA, frxB;
	//-------------------------------------------------------------------------
	typedef FrxComponent::EventSender<sce::PropertyChanged>::Connection Connection;
	//-------------------------------------------------------------------------
	Connection cnA, cnB;
	//-------------------------------------------------------------------------
	Connection connect(FrxComponent::Ptr c);
protected:
	//-------------------------------------------------------------------------
	void onComponentsPropertyChanged(void*, const sce::PropertyChanged &ev);
	//-------------------------------------------------------------------------
	void onPropertyChanged(void*, const sce::PropertyChanged &ev);
	//-------------------------------------------------------------------------
	FrxConnection();
	//-------------------------------------------------------------------------
	void resetBounds();
private:
public:
	//-------------------------------------------------------------------------
	Ptr getPtr() const {
		return boost::shared_dynamic_cast<FrxConnection>(Super::getPtr());
	}
	//-------------------------------------------------------------------------
	void setSrcComponent(FrxComponent::Ptr a);
	//-------------------------------------------------------------------------
	void setDstComponent(FrxComponent::Ptr b);
	//-------------------------------------------------------------------------
	FrxComponent::Ptr getComponentA() const { return frxA; }
	//-------------------------------------------------------------------------
	FrxComponent::Ptr getComponentB() const { return frxB; }
}; // FrxConnection
}}} // namespace(s)

#endif /* SAMBAG_FRXCONNECTION_H */
