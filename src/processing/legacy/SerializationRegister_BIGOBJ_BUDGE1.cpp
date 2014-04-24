/*
 * SerializationRegister.cpp
 *
 *  Created on: Tue Dec 11 13:27:18 2012
 *      Author: Johannes Unger
 */

#include <com/Serialization.h>
#include <boost/shared_ptr.hpp>
#include <processing/concreteAdapter/Volume.h>
#include <processing/legacy/VSTPlugin2x.h>
#include <processing/concreteAdapter/Pan.h>
#include <processing/concreteAdapter/InputStep.h>
#include <processing/concreteAdapter/Volume.h>
#include <processing/concreteAdapter/OutputSwitch.h>
#include <processing/concreteAdapter/OutputStep.h>


namespace frx { namespace processing { namespace legacy { namespace v0 {
//=============================================================================
//  Class SerializationRegister
//=============================================================================
//-----------------------------------------------------------------------------
void register_types_impl_BIGOBJ_BUDGE_1(::com::iArchive &ar) {
    namespace pr = ::processing;
	ar.register_type<pr::Volume>();
	ar.register_type<pr::VSTPlugin>();
	ar.register_type<pr::Pan>();
	ar.register_type<pr::OutputStep>();
	ar.register_type<pr::InputStep>();
	ar.register_type<pr::OutputSwitch>();
}
}}}} // namespace(s)
