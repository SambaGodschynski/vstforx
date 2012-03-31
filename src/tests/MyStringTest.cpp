/*
 * ===========================================================================================================
 * MyStringTest.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */

#include <cppunit/config/SourcePrefix.h>
#include "MyStringTest.hpp"
#include "com/MyString.h"
#include <iostream>


// Registers the fixture into the 'registry'
//CPPUNIT_TEST_SUITE_REGISTRATION( tests::MyStringTest );


namespace tests {
//=============================================================================
void MyStringTest::testConstructor() {
//=============================================================================
	using namespace std;
	using namespace com;
	const string str1("abcdefghijklmnopqrstuvwxyz");
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> std constr.
	MyString myString01;
	CPPUNIT_ASSERT( myString01.length() == 0 );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> constr. const &str
	MyString myString02(str1);
	CPPUNIT_ASSERT_EQUAL( str1, string(myString02) );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>constr. const char*
	MyString myString03( str1.c_str() );
	CPPUNIT_ASSERT_EQUAL( str1, string(myString03) );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>constr. copy
	MyString myString04 = str1;
	CPPUNIT_ASSERT_EQUAL( str1, string(myString04) );
}
//=============================================================================
void MyStringTest::testShortenCenter() {
//=============================================================================
	using namespace std;
	using namespace com;
	const string str1("abcdefghijklmnopqrstuvwxyz");
	MyString::ShortenStyle style = MyString::kCenter;
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> max. 10
	MyString myString = str1;
	myString = myString.shorten ( 10, style, "..." );
	CPPUNIT_ASSERT ( myString.length() <= 10 );
	CPPUNIT_ASSERT_EQUAL ( string("abc...xyz"), string(myString) );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> max == str.length - 1
	myString = str1;
	myString = myString.shorten ( str1.length() - 1, style, "..." );
	CPPUNIT_ASSERT ( myString.length() <= str1.length() - 1 );
	CPPUNIT_ASSERT_EQUAL ( string("abcdefghijk...pqrstuvwxyz"), string(myString) );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> max > str.length
	myString = str1;
	myString = myString.shorten ( str1.length() + 1, style, "..." );
	CPPUNIT_ASSERT ( myString.length() == str1.length() );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> max < 0
	myString = str1;
	myString = myString.shorten ( -1, style, "..." );
	CPPUNIT_ASSERT ( myString.length() == str1.length() );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> space = max + 1
	myString = str1;
	myString = myString.shorten ( 10, style, MyString(".") * 11 );
	CPPUNIT_ASSERT ( myString.length() == str1.length() );
	CPPUNIT_ASSERT_EQUAL (str1, string(myString) );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> space = max - 1
	myString = str1;
	myString = myString.shorten ( 10, style, MyString(".") * 9 );
	CPPUNIT_ASSERT ( myString.length() == str1.length() );
	CPPUNIT_ASSERT_EQUAL ( str1, string(myString) );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> space = max - 2
	myString = str1;
	myString = myString.shorten ( 10, style, MyString(".") * 8 );
	CPPUNIT_ASSERT ( myString.length() <= 10 );
	CPPUNIT_ASSERT_EQUAL ( string("a........z"), string(myString) );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> space > max
	myString = str1;
	myString = myString.shorten ( 10, style, MyString(".") * 100 );
	CPPUNIT_ASSERT ( myString.length() == str1.length() );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> very long string
	myString = MyString("x") * 9999999;
	myString = myString.shorten ( 11, style, MyString(".") * 3 );
	CPPUNIT_ASSERT ( myString.length() <= 11 );
}
//=============================================================================
void MyStringTest::testShortenLeft() {
//=============================================================================
	using namespace std;
	using namespace com;
	const string str1("abcdefghijklmnopqrstuvwxyz");
	MyString::ShortenStyle style = MyString::kLeft;
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> max. 10
	MyString myString = str1;
	myString = myString.shorten ( 10, style, "..." );
	CPPUNIT_ASSERT ( myString.length() <= 10 );
	CPPUNIT_ASSERT_EQUAL ( string("...tuvwxyz"), string(myString) );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> max == str.length - 1
	myString = str1;
	myString = myString.shorten ( str1.length() - 1, style, "..." );
	CPPUNIT_ASSERT ( myString.length() <= str1.length() - 1 );
	CPPUNIT_ASSERT_EQUAL ( string("...efghijklmnopqrstuvwxyz"), string(myString) );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> max > str.length
	myString = str1;
	myString = myString.shorten ( str1.length() + 1, style, "..." );
	CPPUNIT_ASSERT ( myString.length() == str1.length() );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> max < 0
	myString = str1;
	myString = myString.shorten ( -1, style, "..." );
	CPPUNIT_ASSERT ( myString.length() == str1.length() );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> space = max + 1
	myString = str1;
	myString = myString.shorten ( 10, style, MyString(".") * 11 );
	CPPUNIT_ASSERT ( myString.length() == str1.length() );
	CPPUNIT_ASSERT_EQUAL ( str1, string(myString) );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> space = max - 1
	myString = str1;
	myString = myString.shorten ( 10, style, MyString(".") * 9 );
	CPPUNIT_ASSERT ( myString.length() <= 10 );
	CPPUNIT_ASSERT_EQUAL ( string(".........z"), string(myString) );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> space = max - 2
	myString = str1;
	myString = myString.shorten ( 10, style, MyString(".") * 8 );
	CPPUNIT_ASSERT ( myString.length() <= 10 );
	CPPUNIT_ASSERT_EQUAL ( string("........yz"), string(myString) );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> space > max
	myString = str1;
	myString = myString.shorten ( 10, style, MyString(".") * 100 );
	CPPUNIT_ASSERT ( myString.length() == str1.length() );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> very long string
	myString = MyString("x") * 9999999;
	myString = myString.shorten ( 11, style, MyString(".") * 3 );
	CPPUNIT_ASSERT ( myString.length() <= 11 );
}
//=============================================================================
void MyStringTest::testShortenRight() {
//=============================================================================
	using namespace std;
	using namespace com;
	const string str1("abcdefghijklmnopqrstuvwxyz");
	MyString::ShortenStyle style = MyString::kRight;
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> max. 10
	MyString myString = str1;
	myString = myString.shorten ( 10, style, "..." );
	CPPUNIT_ASSERT ( myString.length() <= 10 );
	CPPUNIT_ASSERT_EQUAL ( string("abcdefg..."), string(myString) );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> max == str.length - 1
	myString = str1;
	myString = myString.shorten ( str1.length() - 1, style, "..." );
	CPPUNIT_ASSERT ( myString.length() <= str1.length() - 1 );
	CPPUNIT_ASSERT_EQUAL ( string("abcdefghijklmnopqrstuv..."), string(myString) );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> max > str.length
	myString = str1;
	myString = myString.shorten ( str1.length() + 1, style, "..." );
	CPPUNIT_ASSERT ( myString.length() == str1.length() );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> max < 0
	myString = str1;
	myString = myString.shorten ( -1, style, "..." );
	CPPUNIT_ASSERT ( myString.length() == str1.length() );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> space = max + 1
	myString = str1;
	myString = myString.shorten ( 10, style, MyString(".") * 11 );
	CPPUNIT_ASSERT ( myString.length() == str1.length() );
	CPPUNIT_ASSERT_EQUAL ( str1, string(myString) );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> space = max - 1
	myString = str1;
	myString = myString.shorten ( 10, style, MyString(".") * 9 );
	CPPUNIT_ASSERT ( myString.length() <= 10 );
	CPPUNIT_ASSERT_EQUAL ( string("a........."), string(myString) );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> space = max - 2
	myString = str1;
	myString = myString.shorten ( 10, style, MyString(".") * 8 );
	CPPUNIT_ASSERT ( myString.length() <= 10 );
	CPPUNIT_ASSERT_EQUAL ( string("ab........"), string(myString) );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> space > max
	myString = str1;
	myString = myString.shorten ( 10, style, MyString(".") * 100 );
	CPPUNIT_ASSERT ( myString.length() == str1.length() );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> very long string
	myString = MyString("x") * 9999999;
	myString = myString.shorten ( 11, style, MyString(".") * 3 );
	CPPUNIT_ASSERT ( myString.length() <= 11 );
}
//=============================================================================
void MyStringTest::testMultiplier() {
//=============================================================================
	using namespace std;
	using namespace com;
	MyString str1("x");
	MyString str2("xY");
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>  * 2
	string neu = str1 * 2;
	CPPUNIT_ASSERT_EQUAL ( neu, string("xx") );
	neu = str2 * 2;
	CPPUNIT_ASSERT_EQUAL ( neu, string("xYxY") );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>  * 1
	neu = str1 * 1;
	CPPUNIT_ASSERT_EQUAL ( neu, string("x") );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>  * 0
	neu = str1 * 0;
	CPPUNIT_ASSERT_EQUAL ( neu, string("") );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>  * 0
	neu = str1 * -1;
	CPPUNIT_ASSERT_EQUAL ( neu, string("") );
}
} // namespace tests
