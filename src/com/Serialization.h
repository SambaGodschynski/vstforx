#ifndef SERIALIZATION_H
#define SERIALIZATION_H

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
*/


#include <boost/archive/text_oarchive.hpp> 
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/string.hpp> 
#include <boost/serialization/vector.hpp>
#include <boost/serialization/list.hpp> 
#include <boost/serialization/map.hpp> 
#include <boost/serialization/set.hpp> 
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/weak_ptr.hpp>
#include <com/boostPathSerialization.hpp> 

namespace com {
//------------------------------------------------------------------------------------------------------------
typedef boost::archive::text_oarchive oArchive;
//------------------------------------------------------------------------------------------------------------
typedef boost::archive::text_iarchive iArchive;
//============================================================================================================
// Schnittstelle Serializable:
//============================================================================================================
class Serializable {
private:
public:
	//--------------------------------------------------------------------------------------------------------
	virtual ~Serializable(){}
};
} // namespace com

#endif