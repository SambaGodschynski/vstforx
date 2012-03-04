/*
 * ===========================================================================================================
 * ParameterTest.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */

#include <cppunit/config/SourcePrefix.h>
#include "ParameterTest.hpp"
#include "com/MyString.h"
#include "processing/processing.h"
#include "processing/parameter/parameter.h"
#include "processing/parameter/ConnectionOperators.h"
#include "com/one4All.h"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( tests::ParameterTest );

#define SMALL_VALUE 0.001f
#define TEST_BORDERS(p,_min,_max) \
	(p)->setMin( (_min) ); \
	(p)->setMax( (_max) ); \
	(p)->setValue ( (_min) + SMALL_VALUE ); \
	CPPUNIT_ASSERT_EQUAL ( (float)(_min) + SMALL_VALUE, (p)->getValue() ); \
	(p)->setValue ( (_max) - SMALL_VALUE ); \
	CPPUNIT_ASSERT_EQUAL ( (float)(_max) - SMALL_VALUE, (p)->getValue() ); \
	(p)->setValue ( (_min) - SMALL_VALUE ); \
	CPPUNIT_ASSERT_EQUAL ( (float)(_min), (p)->getValue() ); \
	(p)->setValue ( (_max) + SMALL_VALUE ); \
	CPPUNIT_ASSERT_EQUAL ( (float)(_max), (p)->getValue() ); \
	(p)->setValue ( (_min) + (_max) / 2.0f ); \
	CPPUNIT_ASSERT_EQUAL ( (float)(_min) + (_max) / 2.0f, (p)->getValue() ); \
	(p)->setValue ( 0.0f ); \
	CPPUNIT_ASSERT_EQUAL ( 0.0f, (p)->getValue() ); 

namespace tests {
//=============================================================================
void ParameterTest::testConstructor() {
//=============================================================================
	using namespace std;
	using namespace com;
	using namespace processing;
	using namespace processing::parameter;

	Parameter::Ptr p = Parameter::create();
	float value = 0.0f;
	CPPUNIT_ASSERT_EQUAL ( value, p->getValue() );
}
//=============================================================================
void ParameterTest::testMinMax() {
//=============================================================================
	using namespace std;
	using namespace com;
	using namespace processing;
	using namespace processing::parameter;
	
	Parameter::Ptr p = Parameter::create();
	float v = 0.0f;
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>std: 0..1
	TEST_BORDERS(p, 0.0f, 1.0f );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>std: -10..10
	TEST_BORDERS(p, -10.0f, 10.0f );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>std: -FLT_MAX..FLT_MAX
	TEST_BORDERS(p, -FLT_MAX, FLT_MAX );
}
//=============================================================================
void ParameterTest::testConnectionSet() {
//=============================================================================
	using namespace std;
	using namespace com;
	using namespace processing;
	using namespace processing::parameter;
	ParameterConnectionSet cs;
	Parameter::Ptr p1 = Parameter::create();
	Parameter::Ptr p2 = Parameter::create();
	Parameter::Ptr p3 = Parameter::create();
	CPPUNIT_ASSERT(cs.connectParameter(p1,p2));
	CPPUNIT_ASSERT(!cs.connectParameter(p2,p1)); // existing connection
	CPPUNIT_ASSERT(cs.getConnection(p2,p1));
	ParameterConnection::Ptr p = cs.getConnection(p1,p2);
	CPPUNIT_ASSERT(p);
	CPPUNIT_ASSERT(p->getParameterA() == p1);
	CPPUNIT_ASSERT(p->getParameterB() == p2);
	CPPUNIT_ASSERT(cs.connectParameter(p1,p3));
	CPPUNIT_ASSERT(cs.connectParameter(p2,p3));
	CPPUNIT_ASSERT(!cs.connectParameter(p3,p1)); // existing connection
	CPPUNIT_ASSERT(cs.removeConnection(p1,p2));
	CPPUNIT_ASSERT(cs.removeConnection(p3,p2));
	CPPUNIT_ASSERT(cs.removeConnection(p3,p1));
	CPPUNIT_ASSERT(cs.empty());
}
//=============================================================================
void ParameterTest::testConnection() {
//=============================================================================
	using namespace std;
	using namespace com;
	using namespace processing;
	using namespace processing::parameter;
	ParameterConnectionSet cs;
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>init.
	Parameter::Ptr p1 = Parameter::create();
	Parameter::Ptr p2 = Parameter::create();
	com::VstNumber value  = 0.0f;
	com::VstNumber value2 = 0.0f;
	CPPUNIT_ASSERT_EQUAL( value, p1->getValue() );
	CPPUNIT_ASSERT_EQUAL( value, p2->getValue() );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>conect
	ParameterConnection::Ptr cn = cs.connectParameter(p1, p2);
	value = 0.7776f;
	*p1 = value;
	CPPUNIT_ASSERT_EQUAL( value, p1->getValue() );
	CPPUNIT_ASSERT_EQUAL( value, p2->getValue() );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>inv. conn.
	cn->addOperator( InverseConnection::create() );
	value = 0.5f;
	*p1 = value;
	CPPUNIT_ASSERT_EQUAL( value, p1->getValue() );
	CPPUNIT_ASSERT_EQUAL( value, p2->getValue() );
	value  = 0.0f;
	value2 = 1.0f; 
	*p1 = value;
	CPPUNIT_ASSERT_EQUAL( value , p1->getValue() );
	CPPUNIT_ASSERT_EQUAL( value2, p2->getValue() );
	value  = 1.0f;
	value2 = 0.0f; 
	*p1 = value;
	CPPUNIT_ASSERT_EQUAL( value , p1->getValue() );
	CPPUNIT_ASSERT_EQUAL( value2, p2->getValue() );
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>inv. conn.2
	cn->addOperator( InverseConnection::create() ); 
	value = 0.5f;
	*p1 = value;
	CPPUNIT_ASSERT_EQUAL( value, p1->getValue() );
	CPPUNIT_ASSERT_EQUAL( value, p2->getValue() );
	value  = 0.0f;
	value2 = 0.0f; 
	*p1 = value;
	CPPUNIT_ASSERT_EQUAL( value , p1->getValue() );
	CPPUNIT_ASSERT_EQUAL( value2, p2->getValue() );
	value  = 1.0f;
	value2 = 1.0f; 
	*p1 = value;
	CPPUNIT_ASSERT_EQUAL( value , p1->getValue() );
	CPPUNIT_ASSERT_EQUAL( value2, p2->getValue() );
}
} // namespace tests
