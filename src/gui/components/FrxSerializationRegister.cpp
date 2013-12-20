/*
 * FrxSerializationRegister.cpp
 *
 *  Created on: Thu Oct 11 13:25:48 2012
 *      Author: Johannes Unger
 */

#include "FrxSerializationRegister.hpp"
#include <boost/shared_ptr.hpp>
#include "FrxConcreteConnections.hpp"
#include "FrxConcreteIO.hpp"
#include "FrxConcreteParameter.hpp"
#include "FrxSelection.hpp"
#include "FrxHover.hpp"
#include "FrxFlag.hpp"
#include <gui/ViewFactory.hpp>

namespace frx { namespace gui { namespace components {
//=============================================================================
//  Class FrxSerializationRegister
//=============================================================================
//-----------------------------------------------------------------------------
template <class Archive>
void register_types_impl( Archive &ar ) {
	ar.template register_type<IOCn>();
	ar.template register_type<ProcessorInputCn>();
	ar.template register_type<ProcessorOutputCn>();
	ar.template register_type<ProcessorParameterCn>();
	ar.template register_type<ParameterCn>();
	ar.template register_type<ParameterOPCn>();
    ar.template register_type<FrxInputNode>();
	ar.template register_type<FrxOutputNode>();
	ar.template register_type<FrxEntryNode>();
	ar.template register_type<FrxExitNode>();
	ar.template register_type<FrxStdKnob>();
	ar.template register_type<FrxSelection>();
	ar.template register_type<FrxHover>();
	ar.template register_type<FrxFlag>();
}
//-----------------------------------------------------------------------------
void register_types( ::com::iArchive &ar ) {
	register_types_impl(ar);
    ViewFactory::instance().registerToArchive(ar);
}
void register_types( ::com::oArchive &ar ) {
	register_types_impl(ar);
    ViewFactory::instance().registerToArchive(ar);
}
}}} // namespace(s)
