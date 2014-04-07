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
#include <processing/dspTools.h>
#include "VSTPlugin2x.h"
#include <processing/NodeConnection.hpp>
#include <processing/NodeAdapter.hpp>
#include <processing/ProcessorAdapter.hpp>
#include <processing/PluginAdapter.hpp>
#include <processing/ParameterConnection.hpp>
#include <processing/ParameterAdapter.hpp>
#include <processing/interprocess/RemoteChReceiver.hpp>
#include <processing/ModelFactory.hpp>


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
	
	// we've got a lot of problems here with mvsc, because boost archive increases the sections
	// of an .obj file enormously. Using /bigobj (http://msdn.microsoft.com/de-de/library/ms173499.aspx)
	// helped for the first time but soon or later you've got LNK:out of memory errors.
	// A solution is to delegate the actual register calls to another location.
	ModelFactory &fac = ModelFactory::instance();
	fac.registerToArchive(ar, "frx.processing.internal.Volume"); //ar.template register_type<pr::Volume>();
	fac.registerToArchive(ar, "frx.processing.legacy.VST2xPlugin"); //ar.template register_type<pr::VSTPlugin>();
	fac.registerToArchive(ar, "frx.processing.internal.Pan"); //ar.template register_type<pr::Pan>();
	fac.registerToArchive(ar, "frx.processing.internal.OutputStep"); //ar.template register_type<pr::OutputStep>();
	fac.registerToArchive(ar, "frx.processing.internal.InputStep"); //ar.template register_type<pr::InputStep>();
	fac.registerToArchive(ar, "frx.processing.internal.OutputSwitch"); //ar.template register_type<pr::OutputSwitch>();
	fac.registerToArchive(ar, "frx.processing.internal.InputSwitch"); //ar.template register_type<pr::InputSwitch>();
	fac.registerToArchive(ar, "frx.processing.internal.PeakTracker"); //ar.template register_type<pr::PeakTracker>();
	fac.registerToArchive(ar, "frx.processing.internal.ADSRTrigger"); //ar.template register_type<pr::ADSRTrigger>();
	fac.registerToArchive(ar, "frx.processing.internal.MidiProcessor"); //ar.template register_type<pr::MidiProcessor>();
	fac.registerToArchive(ar, "frx.processing.interprocess.RemoteChReceiver"); //ar.template register_type<frx::processing::interprocess::RemoteChReceiver>();
	fac.registerToArchive(ar, "frx.processing.internal-private.DCTester"); //ar.template register_type<pr::DCTester>();


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
