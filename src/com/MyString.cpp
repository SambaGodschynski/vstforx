#include "MyString.h"
#include <boost/algorithm/string.hpp> 


namespace {
//-----------------------------------------------------------------------------------------------------------
inline com::MyString shortenCenter ( const com::MyString &str, int max, const string & space ) {
	// space muss min. 2 einheiten kleiner sein
	if ( space.length()  > max - 2 ) return str;
	size_t l = str.length();
	using namespace com;	
	int diff = l - ( max - space.length() - 1 );
	if ( diff <= 0 ) return str;
	size_t m = l/2;
	size_t s = m - diff/2;
	size_t e = m + diff/2;
	MyString out = str.substr( 0, s ) + space + str.substr(e);
	return out;
}
//-----------------------------------------------------------------------------------------------------------
inline com::MyString shortenLeft ( const com::MyString &str, int max, const string & space ) {
	size_t l = str.length();
	using namespace com;	
	int diff = l - ( max - space.length()  );
	if ( diff <= 0 ) return str;
	MyString out = space + str.substr( diff );
	return out;
}
//-----------------------------------------------------------------------------------------------------------
inline com::MyString shortenRight ( const com::MyString &str, int max, const string & space ) {
	size_t l = str.length();
	using namespace com;	
	int diff = l - ( max - space.length() );
	if ( diff <= 0 ) return str;
	MyString out = str.substr( 0, l - diff ) + space;
	return out;
}
} // anonymous namespace


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
MyString MyString::trim() const {
	return MyString ( boost::algorithm::trim_copy( (string)*this ) );
}
//------------------------------------------------------------------------------------------------------------
MyString MyString::shorten( int max, ShortenStyle type, const string & space ) const {
	if ( this->length() < max ) return *this;
	// space muss mind. 2 kleiner sein als max
	if ( space.length() - 1 >= max ) return *this;
	switch (type) {
		case kCenter:
			return shortenCenter( *this, max, space );
		case kLeft:
			return shortenLeft( *this, max, space );
		case kRight:
			return shortenRight( *this, max, space );
	}
	return *this;
}
//------------------------------------------------------------------------------------------------------------
MyString MyString::operator * ( int c ) const {
	if ( c == 1 ) return *this;
	if ( c <= 0 ) return "";
	MyString out = *this;
	while ( --c > 0 ) {
		out+=*this;
	}
	return out;
}
} //namespace com


