/*
 * ConcreteConnections.hpp
 *
 *  Created on: Fri Aug 24 10:03:59 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_CONCRETECONNECTIONS_H
#define SAMBAG_CONCRETECONNECTIONS_H

#include "FrxConnection.hpp"

namespace frx { namespace gui { namespace components {
namespace sdc = sambag::disco::components;
namespace sdcu = sdc::ui;
//=============================================================================
/** 
  * @class FrxConnection.
  */
template <class _ConnectionType>
class FrxConcreteConnection : public FrxConnection,
	public _ConnectionType
{
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef FrxConnection Super;
	//-------------------------------------------------------------------------
	typedef _ConnectionType ConnectionType;
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxConcreteConnection> Ptr;
protected:
	//-------------------------------------------------------------------------
	virtual void postConstructor() {
		ConnectionType::init( getPtr() );
	}
	//-------------------------------------------------------------------------
	FrxConcreteConnection() {
		setName("FrxConcreteConnection");
	}
private:
public:
	//-------------------------------------------------------------------------
	Ptr getPtr() const {
		return boost::shared_dynamic_cast<
			FrxConcreteConnection<ConnectionType>
		>(Super::getPtr());
	}
	//-------------------------------------------------------------------------
	static Ptr create() {
		Ptr res( new FrxConcreteConnection<_ConnectionType>() );
		res->self = res;
		res->postConstructor();
		return res;
	}
	//-------------------------------------------------------------------------
	sdcu::AComponentUIPtr createComponentUI(sdcu::ALookAndFeelPtr laf) const {
		return laf->getUI< FrxConcreteConnection<ConnectionType> >();
	}
}; // FrxConnection
//=============================================================================
// Types
//=============================================================================
namespace connectionTypes {
	struct ConnectionTypeBase{ void init( FrxConnection::Ptr ){} };
	struct IO : ConnectionTypeBase{};
	struct ProcessorInput : ConnectionTypeBase{};
	struct ProcessorOutput : ConnectionTypeBase{};
	struct ProcessorParameter : ConnectionTypeBase{};
	struct Parameter : ConnectionTypeBase{};
	struct ParameterOP : ConnectionTypeBase{};
}
typedef FrxConcreteConnection<connectionTypes::IO> IOCn;
typedef FrxConcreteConnection<connectionTypes::ProcessorInput> ProcessorInputCn;
typedef FrxConcreteConnection<connectionTypes::ProcessorOutput> ProcessorOutputCn;
typedef FrxConcreteConnection<connectionTypes::ProcessorParameter> ProcessorParameterCn;
typedef FrxConcreteConnection<connectionTypes::Parameter> ParameterCn;
typedef FrxConcreteConnection<connectionTypes::ParameterOP> ParameterOPCn;

}}} // namespace(s)

#endif /* SAMBAG_CONCRETECONNECTIONS_H */
