/*
 * SerializationRegister.cpp
 *
 *  Created on: Tue Dec 11 13:27:18 2012
 *      Author: Johannes Unger
 */

#include <com/Serialization.h>
#include <boost/shared_ptr.hpp>
#include <processing/concreteAdapter/InputSwitch.h>
#include <processing/concreteAdapter/PeakTracker.h>
#include <processing/concreteAdapter/Pan.h>
#include <processing/concreteAdapter/ADSRTrigger.h>
#include <processing/interprocess/RemoteChReceiver.hpp>
#include <processing/concreteAdapter/DCTester.hpp>
#include <processing/concreteAdapter/MidiProcessor.h>


namespace frx { namespace processing { namespace legacy { namespace v0 {
//=============================================================================
//  Class SerializationRegister
//=============================================================================
//-----------------------------------------------------------------------------
void register_types_impl_BIGOBJ_BUDGE_2(::com::iArchive &ar) {
    namespace pr = ::processing;
	ar.register_type<pr::InputSwitch>();
	ar.register_type<pr::PeakTracker>();
	ar.register_type<pr::ADSRTrigger>();
	ar.register_type<pr::MidiProcessor>();
	ar.register_type<frx::processing::interprocess::RemoteChReceiver>();
	ar.register_type<pr::DCTester>();
}
}}}} // namespace(s)
