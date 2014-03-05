/*
 * SerializationRegister.hpp
 *
 *  Created on: Tue Dec 11 13:27:18 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_SERIALIZATIONREGISTER_H
#define SAMBAG_SERIALIZATIONREGISTER_H


#include <com/Serialization.h>
#include <com/FrxConfig.h>

namespace frx { namespace processing {

	void register_types(::com::iArchive &ar, int version = FRX_ARCHIVE_VERSION);
	void register_types(::com::oArchive &ar, int version = FRX_ARCHIVE_VERSION);


}} // namespace(s)

#endif /* SAMBAG_SERIALIZATIONREGISTER_H */
