/*
 * SerializationRegister.cpp
 *
 *  Created on: Tue Dec 11 13:27:18 2012
 *      Author: Johannes Unger
 */

#include <com/Serialization.h>
#include <boost/shared_ptr.hpp>
#include <processing/processing.h>
#include <processing/parameter/parameter.h>
#include <processing/parameter/ConnectionOperators.h>
#include <processing/ConcreteProcessAdapter.h>
#include <processing/dspTools.h>
#include "VSTPlugin2x.h"
#include <processing/NodeConnection.hpp>
#include <processing/NodeAdapter.hpp>
#include <processing/ProcessorAdapter.hpp>
#include <processing/PluginAdapter.hpp>
#include <processing/ParameterConnection.hpp>
#include <processing/ParameterAdapter.hpp>
#include <processing/interprocess/RemoteChReceiver.hpp>


namespace frx { namespace processing { namespace legacy { namespace v0 {
//=============================================================================
//  Class SerializationRegister
//=============================================================================
//-----------------------------------------------------------------------------
template <class Archive>
void register_types_impl( Archive &ar ) {
	namespace pr = ::processing;
	ar.template register_type<pr::parameter::Parameter>();
	ar.template register_type<pr::parameter::InverseConnection>();
	ar.template register_type<pr::parameter::ExpConnection>();
	ar.template register_type<pr::parameter::LogConnection>();
	ar.template register_type<pr::parameter::OffsetConnection>();
	ar.template register_type<pr::parameter::MultiplierConnection>();
	ar.template register_type<pr::parameter::MinMaxConnection>();
	ar.template register_type<pr::NOPNode>();
	ar.template register_type<pr::ProcessAdapter::OutputNode>();
	ar.template register_type<pr::ProcessAdapter::InputNode>();
	ar.template register_type<pr::StartNode>();
	ar.template register_type<pr::EndNode>();
	ar.template register_type<pr::ProcessAdapterNode>();
	ar.template register_type<pr::Volume>();
	ar.template register_type<pr::VSTPlugin>();
	ar.template register_type<pr::Pan>();
	ar.template register_type<pr::OutputStep>();
	ar.template register_type<pr::InputStep>();
	ar.template register_type<pr::OutputSwitch>();
	ar.template register_type<pr::InputSwitch>();
	ar.template register_type<pr::PeakTracker>();
	ar.template register_type<pr::ADSRTrigger>();
	ar.template register_type<pr::MidiProcessor>();
	ar.template register_type<frx::processing::interprocess::RemoteChReceiver>();
	ar.template register_type<pr::DCTester>();
	ar.template register_type<pr::FadeValue>();

	ar.template register_type<ProcessorAdapter>();
	ar.template register_type<PluginAdapter>();
	ar.template register_type<ParameterConnection>();
	ar.template register_type<ParameterAdapter>();
	ar.template register_type<NodeAdapter>();
	ar.template register_type<NodeConnection>();
}
void register_types(::com::iArchive &ar) {
	register_types_impl(ar);
}
}}}} // namespace(s)
