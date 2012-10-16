/*
 * FrxSerializationRegister.hpp
 *
 *  Created on: Thu Oct 11 13:25:48 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXSERIALIZATIONREGISTER_H
#define SAMBAG_FRXSERIALIZATIONREGISTER_H

/*
	Type registrations for boost archive.
	!Strong dependency avoid including in header files.
*/

#include <boost/shared_ptr.hpp>
#include "FrxConcreteConnections.hpp"
#include "FrxConcreteProcessor.hpp"
#include "FrxConcreteIO.hpp"
#include "FrxConcreteParameter.hpp"
#include "FrxSelection.hpp"
#include "FrxHover.hpp"

namespace frx { namespace gui { namespace components {
struct RegisterFrxTypes {
	template <class Archive>
	static void register_types( Archive &ar ) {
		ar.template register_type<IOCn>();
		ar.template register_type<ProcessorInputCn>();
		ar.template register_type<ProcessorOutputCn>();
		ar.template register_type<ProcessorParameterCn>();
		ar.template register_type<ParameterCn>();
		ar.template register_type<ParameterOPCn>();
		ar.template register_type<FrxPluginNode>();
		ar.template register_type<FrxVolumeNode>();
		ar.template register_type<FrxPanNode>();
		ar.template register_type<FrxInStepNode>();
		ar.template register_type<FrxOutStepNode>();
		ar.template register_type<FrxInSwitchNode>();
		ar.template register_type<FrxOutSwitchNode>();
		ar.template register_type<FrxADSRNode>();
		ar.template register_type<FrxPeakTrackerNode>();
		ar.template register_type<FrxInputNode>();
		ar.template register_type<FrxOutputNode>();
		ar.template register_type<FrxEntryNode>();
		ar.template register_type<FrxExitNode>();
		ar.template register_type<FrxStdKnob>();
		ar.template register_type<FrxSelection>();
		ar.template register_type<FrxHover>();
	}
};
}}} // namespace(s)

#endif /* SAMBAG_FRXSERIALIZATIONREGISTER_H */
