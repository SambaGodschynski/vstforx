/*
 * SerializationRegister.cpp
 *
 *  Created on: Tue Dec 11 13:27:18 2012
 *      Author: Johannes Unger
 */

#include "SerializationRegister.hpp"
#include <boost/shared_ptr.hpp>
#include <processing/processing.h>
#include <processing/parameter/parameter.h>
#include <processing/parameter/ConnectionOperators.h>
#include <processing/dspTools.h>
#include <processing/NodeConnection.hpp>
#include <processing/NodeAdapter.hpp>
#include <processing/ProcessorAdapter.hpp>
#include <processing/PluginAdapter.hpp>
#include <processing/ParameterConnection.hpp>
#include <processing/ParameterAdapter.hpp>
#include <processing/ModelFactory.hpp>
#include <exception>

namespace frx { namespace processing {

namespace legacy { namespace v0 {
    extern void register_types( ::com::iArchive &ar );
}}

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
	ar.template register_type<pr::FadeValue>();
	ar.template register_type<ProcessorAdapter>();
	ar.template register_type<PluginAdapter>();
	ar.template register_type<ParameterConnection>();
	ar.template register_type<ParameterAdapter>();
	ar.template register_type<NodeAdapter>();
	ar.template register_type<NodeConnection>();
}
void register_types(::com::iArchive &ar, int version) {
    if (version == 0) {
        SAMBAG_LOG_TRACE<<"legacy archive version "<<version;
        legacy::v0::register_types(ar);
        return;
    }
	register_types_impl(ar);
    ModelFactory::instance().registerToArchive(ar);
}
void register_types(::com::oArchive &ar, int version) {
    if (version != FRX_ARCHIVE_VERSION) {
        throw std::runtime_error("registering an invalid archive version");
    }
	register_types_impl(ar);
	ModelFactory::instance().registerToArchive(ar);
}
}} // namespace(s)
