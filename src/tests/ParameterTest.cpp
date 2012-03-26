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
//=============================================================================
struct ListenerFClass {
	float v;
	void onParameterChanged(void *src, const float &newValue) {
		v = newValue;
	}
	ListenerFClass() :  v(0) {}
};
//=============================================================================
void ParameterTest::testParameterListenerF() {
//=============================================================================
	using namespace std;
	using namespace com;
	using namespace processing;
	using namespace processing::parameter;
	ListenerFClass f01;
	ListenerFClass f02;
	Parameter::Ptr p01 = Parameter::create();
	
	Parameter::ParameterListenerFunction ev01 =
		boost::bind(&ListenerFClass::onParameterChanged, &f01, _1, _2);
	Parameter::ParameterListenerFunction ev02 =
		boost::bind(&ListenerFClass::onParameterChanged, &f02, _1, _2);
	
	Parameter::Connection cn01 = p01->addValueChangedListener(ev01);
	Parameter::Connection cn02 = p01->addValueChangedListener(ev02);

	p01->setValue(.5f);
	CPPUNIT_ASSERT_EQUAL(.5f, f01.v);
	CPPUNIT_ASSERT_EQUAL(.5f, f02.v);
	
	cn02.disconnect();
	p01->setValue(1.f);
	
	CPPUNIT_ASSERT_EQUAL(1.f, f01.v);
	CPPUNIT_ASSERT_EQUAL(.5f, f02.v); // nothing changed
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<test tracked listener
	{ // extra scope
		Parameter::Ptr toTrack = Parameter::create();
		p01->addTrackedValueChangedListener(ev02, toTrack);
		p01->setValue(.1f);
		CPPUNIT_ASSERT_EQUAL(.1f, f01.v);
		CPPUNIT_ASSERT_EQUAL(.1f, f02.v);
	} // auto disconnect on toTrack's dispose
	p01->setValue(1.f);
	CPPUNIT_ASSERT_EQUAL(1.f, f01.v);
	CPPUNIT_ASSERT_EQUAL(.1f, f02.v); // nothing changed
}
} // namespace tests
