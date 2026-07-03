/*
 * ===========================================================================================================
 * Serialization.h
 *      Author: Johannes Unger
 * ===========================================================================================================
 */

/*
	:::::::::::::::::::::::::::::::::::::::::::::::::
	Serialisation via boost::serilailization::archive
	:::::::::::::::::::::::::::::::::::::::::::::::::

	Abhandlung:
		+ implementierung der serialize() bzw. load(), save() methoden. ( save() ist const methode : beachten! )
		+ Erben von Serialize: eigentlich nicht notwendig. Da aber Oberklassen Zeiger Serialisiert werden
		  kommt es zu boost::archive exception wenn unterklassen existieren die nicht serialize() impl.
	    + Registrieren des Unterklassen types. Ueber register_types() in PpiVst.cpp
		+ Frei stehende Methoden save_construct_data(), bzw. load_construct_data impl().
		  save_construct_data() : Objekt zeiger ist const !!
		  (schon ein paar mal vergessen->schwer zu findender fehler)

	Include this header ONLY in .cpp files that actually read or write archives.
	In .h/.hpp files use <com/SerializationFwd.h> instead.
*/

#ifndef SERIALIZATION_H
#define SERIALIZATION_H

#include <com/SerializationFwd.h>

// Full archive definitions — the heavy part.
// These are only needed where archives are actually instantiated.
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

// oArchive, iArchive, Serializable already defined in SerializationFwd.h

#endif
