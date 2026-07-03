/*
 * ===========================================================================================================
 * MyString.h
 *      Author: Johannes Unger
 * ===========================================================================================================
 */



#ifndef MYSTRING_H
#define MYSTRING_H

#include <string>
#include <sstream>
#include "PPIError.h"
#include "com/SerializationFwd.h"
#include <boost/type_traits.hpp>
#include <boost/static_assert.hpp>

namespace com {
//============================================================================================================
// class MyString
// Erweitert die Klasse string um ein paar Features:
//    - Initalisierung mit Numerischen typen int, float , ...
//    - Implementierung von ValueChangedListener.
//	  - Verknuepfung von MyStings ueber ValueChangedListener : andert sich a aendert sich b. 
//============================================================================================================

//------------------------------------------------------------------------------------------------------------
class MyString : public std::string  {
friend class boost::serialization::access;
BOOST_SERIALIZATION_SPLIT_MEMBER()
private:
	//--------------------------------------------------------------------------------------------------------
	template < typename Archive >
	void save ( Archive &ar, const unsigned int version ) const {
		std::string str = (std::string) *this;
		ar << str;
	}
	//--------------------------------------------------------------------------------------------------------
	template < typename Archive >
	void load ( Archive &ar, const unsigned int version ) {
		std::string str;
		ar >> str;
		*this = str; 
	}
public:
	//--------------------------------------------------------------------------------------------------------
	enum ShortenStyle { kCenter, kLeft, kRight };
	//--------------------------------------------------------------------------------------------------------
	MyString ( const std::string &str = std::string() ) : std::string (str){}
    //--------------------------------------------------------------------------------------------------------
	MyString ( const MyString &str ) : std::string(str) {}
	//--------------------------------------------------------------------------------------------------------
	MyString ( const char * s ) : std::string ( s ){}
	//--------------------------------------------------------------------------------------------------------
	MyString ( char * s ) : std::string ( s ){}
	//--------------------------------------------------------------------------------------------------------
	MyString trim() const;
	//--------------------------------------------------------------------------------------------------------
	~MyString (){}
	//--------------------------------------------------------------------------------------------------------
	MyString & operator = ( const MyString &str ){
		std::string::operator = (str);
		return *this;
	}
	//--------------------------------------------------------------------------------------------------------
	MyString operator * ( int c ) const; // python style: a*4 = aaaa
	//--------------------------------------------------------------------------------------------------------
	MyString ( bool b ) : std::string( b ? "true" : "false" ) {}
	//--------------------------------------------------------------------------------------------------------
	// Kopiert MyString nach bereits allozoiertem c_str ptr.
	void copyToC_Str ( char *data ){
		memcpy ( data, c_str(), size() );
	}
	//--------------------------------------------------------------------------------------------------------
	// liefert Hashwert fuer std::string
	long hash() const {
		long h = 0;
		for ( size_t i = 0; i < length(); i++) h = 31*h + at(i);
		return h;
	}
	//--------------------------------------------------------------------------------------------------------
	// kuerzt std::string aus der mitte: "laaaaanng...eeees"
	MyString shorten( int max, ShortenStyle type = kCenter, const std::string & space = "..." ) const;
    //--------------------------------------------------------------------------------------------------------
	template <typename T>
	MyString(const T &val) {
        BOOST_STATIC_ASSERT(
            boost::is_integral<T>::value || boost::is_float<T>::value
        );
		std::ostringstream os;
        if (boost::is_float<T>::value) {
			os.precision (3);
			os << std::fixed;
		}
		os << val;
		*this = std::string ( os.str() );

	}
};
} //namespace com

#endif



