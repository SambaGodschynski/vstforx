
#ifndef PARAMETER_TEST_H
#define PARAMETER_TEST_H

#include <cppunit/extensions/HelperMacros.h>

namespace tests {
//=============================================================================
class ParameterTest : public CPPUNIT_NS::TestFixture {
//=============================================================================
	CPPUNIT_TEST_SUITE( ParameterTest );
	CPPUNIT_TEST( testConstructor );
	CPPUNIT_TEST( testMinMax );
	CPPUNIT_TEST( testConnection );
	CPPUNIT_TEST( testConnectionSet );
	CPPUNIT_TEST( testParameterListenerF );
	CPPUNIT_TEST_SUITE_END();
private:
public:
	void setUp(){}
	void tearDown(){}
	void testConnectionSet();
	void testConnection();
	void testConstructor();
	void testParameterListenerF();
	void testMinMax();
};
} // namespace tests
#endif 
