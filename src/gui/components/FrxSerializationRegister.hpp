/*
 * FrxSerializationRegister.hpp
 *
 *  Created on: Thu Oct 11 13:25:48 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXSERIALIZATIONREGISTER_H
#define SAMBAG_FRXSERIALIZATIONREGISTER_H

#include <com/Serialization.h>

namespace frx { namespace gui { namespace components {
	void register_types( ::com::iArchive &ar );
	void register_types( ::com::oArchive &ar );
}}} // namespace(s)

#endif /* SAMBAG_FRXSERIALIZATIONREGISTER_H */
