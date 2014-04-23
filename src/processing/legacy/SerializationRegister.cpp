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

namespace frx { namespace processing { namespace legacy { namespace v0 {
extern void register_types_impl_BIGOBJ_BUDGE_1(::com::iArchive &ar);
extern void register_types_impl_BIGOBJ_BUDGE_2(::com::iArchive &ar);
extern void register_types_impl_BIGOBJ_BUDGE_3(::com::iArchive &ar);
//=============================================================================
//  Class SerializationRegister
//=============================================================================
//-----------------------------------------------------------------------------
void register_types_impl(::com::iArchive &ar) {
	namespace pr = ::processing;
	ar.register_type<pr::parameter::Parameter>();
	ar.register_type<pr::parameter::InverseConnection>();
	ar.register_type<pr::parameter::ExpConnection>();
	ar.register_type<pr::parameter::LogConnection>();
	ar.register_type<pr::parameter::OffsetConnection>();
	ar.register_type<pr::parameter::MultiplierConnection>();
	ar.register_type<pr::parameter::MinMaxConnection>();
	ar.register_type<pr::NOPNode>();
	ar.register_type<pr::ProcessAdapter::OutputNode>();
	ar.register_type<pr::ProcessAdapter::InputNode>();
	ar.register_type<pr::StartNode>();
	ar.register_type<pr::EndNode>();
	ar.register_type<pr::ProcessAdapterNode>();
	
	// we've got a lot of problems here with mvsc, because boost archive increases the sections
	// of an .obj file enormously. Using /bigobj (http://msdn.microsoft.com/de-de/library/ms173499.aspx)
	// helped for the first time but soon or later you've got LNK:out of memory errors.
	// A solution is to delegate the actual register calls to another location.
    register_types_impl_BIGOBJ_BUDGE_1(ar);
    register_types_impl_BIGOBJ_BUDGE_2(ar);
    register_types_impl_BIGOBJ_BUDGE_3(ar);
}
void register_types(::com::iArchive &ar) {
	register_types_impl(ar);
}
}}}} // namespace(s)
