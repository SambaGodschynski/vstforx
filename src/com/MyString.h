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
#include "Serialization.h"
#include <boost/type_traits.hpp>
#include <boost/static_assert.hpp>

using namespace std;
namespace com {
//============================================================================================================
// class MyString
// Erweitert die Klasse string um ein paar Features:
//    - Initalisierung mit Numerischen typen int, float , ...
//    - Implementierung von ValueChangedListener.
//	  - Verknuepfung von MyStings ueber ValueChangedListener : andert sich a aendert sich b. 
//============================================================================================================

//------------------------------------------------------------------------------------------------------------
class MyString : public string  {
friend class boost::serialization::access;
BOOST_SERIALIZATION_SPLIT_MEMBER()
private:
	//--------------------------------------------------------------------------------------------------------
	template < typename Archive >
	void save ( Archive &ar, const unsigned int version ) const {
		string str = (string) *this;
		ar << str;
	}
	//--------------------------------------------------------------------------------------------------------
	template < typename Archive >
	void load ( Archive &ar, const unsigned int version ) {
		string str;
		ar >> str;
		*this = str; 
	}
public:
	//--------------------------------------------------------------------------------------------------------
	enum ShortenStyle { kCenter, kLeft, kRight };
	//--------------------------------------------------------------------------------------------------------
	MyString ( const string &str = string() ) : string (str){}
	//--------------------------------------------------------------------------------------------------------
	MyString ( const char * s ) : string ( s ){}
	//--------------------------------------------------------------------------------------------------------
	MyString trim() const;
	//--------------------------------------------------------------------------------------------------------
	~MyString (){}
	//--------------------------------------------------------------------------------------------------------
	MyString & operator = ( const MyString &str ){
		string::operator = (str);
		return *this;
	}
	//--------------------------------------------------------------------------------------------------------
	MyString operator * ( int c ) const; // python style: a*4 = aaaa
	//--------------------------------------------------------------------------------------------------------
	MyString ( bool b ) : string( b ? "true" : "false" ) {}
	//--------------------------------------------------------------------------------------------------------
	// Kopiert MyString nach bereits allozoiertem c_str ptr.
	void copyToC_Str ( char *data ){
		memcpy ( data, c_str(), size() );
	}
	//--------------------------------------------------------------------------------------------------------
	// liefert Hashwert fuer String
	long hash() const {
		long h = 0;
		for ( size_t i = 0; i < length(); i++) h = 31*h + at(i);
		return h;
	}
	//--------------------------------------------------------------------------------------------------------
	// kuerzt string aus der mitte: "laaaaanng...eeees"
	MyString shorten( int max, ShortenStyle type = kCenter, const string & space = "..." ) const;
	//--------------------------------------------------------------------------------------------------------
private:
	enum ConvType{ 
		Integral = 1 << 1,
		Float    = 1 << 2
	};
public:
	//--------------------------------------------------------------------------------------------------------
	template <typename T>
	MyString(const T &val) {
		enum { 
			CType = 1 << (boost::is_integral<T>::value * 1)
			          << (boost::is_float<T>::value * 2)
		};
		BOOST_STATIC_ASSERT_MSG(CType==Integral || 
			                    CType==Float, "frx::MyString Invalid conversions type.");
		std::ostringstream os;
		if (CType == Float) {
			os.precision (3);
			os << std::fixed;
		}
		os << val;
		*this = std::string ( os.str() );

	}
};
} //namespace com

#endif



