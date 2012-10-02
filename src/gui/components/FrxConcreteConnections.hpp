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
//-----------------------------------------------------------------------------
template <class _ConnectionType>
std::string getConnectionName();
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
		setName(getConnectionName<ConnectionType>());
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
///////////////////////////////////////////////////////////////////////////////
template <class _ConnectionType>
std::string getConnectionName() { return "unkown connection type"; }
template <>
inline std::string getConnectionName<connectionTypes::IO>() { 
	return "input/output connection"; 
}
template <>
inline std::string getConnectionName<connectionTypes::ProcessorInput>() { 
	return "processor/input connection"; 
}
template <>
inline std::string getConnectionName<connectionTypes::ProcessorOutput>() { 
	return "processor/output connection"; 
}
template <>
inline std::string getConnectionName<connectionTypes::ProcessorParameter>() { 
	return "processor/parameter connection"; 
}
template <>
inline std::string getConnectionName<connectionTypes::Parameter>() { 
	return "parameter/parameter connection"; 
}
template <>
inline std::string getConnectionName<connectionTypes::ParameterOP>() { 
	return "parameter/operator parameter connection"; 
}
}}} // namespace(s)

#endif /* SAMBAG_CONCRETECONNECTIONS_H */
