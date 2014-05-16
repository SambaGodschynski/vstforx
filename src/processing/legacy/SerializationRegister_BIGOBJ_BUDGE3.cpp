/*
 * SerializationRegister.cpp
 *
 *  Created on: Tue Dec 11 13:27:18 2012
 *      Author: Johannes Unger
 */

#include <com/Serialization.h>
#include <boost/shared_ptr.hpp>
#include <processing/dspTools.h>
#include <processing/processing.h>
#include <processing/NodeConnection.hpp>
#include <processing/NodeAdapter.hpp>
#include <processing/ProcessorAdapter.hpp>
#include <processing/PluginAdapter.hpp>
#include <processing/ParameterConnection.hpp>
#include <processing/ParameterAdapter.hpp>

namespace frx { namespace processing { namespace legacy { namespace v0 {
//=============================================================================
//  Class SerializationRegister
//=============================================================================
//-----------------------------------------------------------------------------
void register_types_impl_BIGOBJ_BUDGE_3(::com::iArchive &ar) {
    namespace pr = ::processing;
	ar.register_type<pr::FadeValue>();
	ar.register_type<ProcessorAdapter>();
	ar.register_type<PluginAdapter>();
	ar.register_type<ParameterConnection>();
	ar.register_type<ParameterAdapter>();
	ar.register_type<NodeAdapter>();
	ar.register_type<NodeConnection>();
}
}}}} // namespace(s)
