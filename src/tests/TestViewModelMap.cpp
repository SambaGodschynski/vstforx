/*
 * TestViewModelMap.cpp
 *
 *  Created on: Wed Oct 10 12:20:59 2012
 *      Author: Johannes Unger
 */

#include "TestViewModelMap.hpp"
#include <gui/ViewModelMap.hpp>
#include <cppunit/config/SourcePrefix.h>
#include <boost/shared_ptr.hpp>
#include <sambag/com/Common.hpp>
#include <sambag/com/exceptions/IllegalStateException.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( tests::TestViewModelMap );

namespace tests {
template <class Base>
struct TestClass : public Base {
	typedef boost::shared_ptr<TestClass> Ptr;
	static Ptr create() {
		return Ptr(new TestClass());
	}
};

//=============================================================================
//  Class TestViewModelMap
//=============================================================================
//-----------------------------------------------------------------------------
void TestViewModelMap::testCreating() {
	using namespace frx::gui;
	ViewModelMap::Ptr map = ViewModelMap::create();
	CPPUNIT_ASSERT(map);
}
//-----------------------------------------------------------------------------
void TestViewModelMap::testRegistering() {
	using namespace frx::gui;
	using namespace frx::processing;
	ViewModelMap::Ptr map = ViewModelMap::create();
	typedef TestClass<ViewObject> TestViewObject;
	typedef TestClass<ModelObject> TestModelObject;
	TestViewObject::Ptr v1 = TestViewObject::create();
	TestViewObject::Ptr v2 = TestViewObject::create();
	TestModelObject::Ptr m1 = TestModelObject::create();
	TestModelObject::Ptr m2 = TestModelObject::create();
	CPPUNIT_ASSERT_EQUAL((size_t)0, map->getSize());
	map->registerObjects(v1, m1);
	CPPUNIT_ASSERT_EQUAL((size_t)1, map->getSize());
	map->registerObjects(v1, m1);
	CPPUNIT_ASSERT_EQUAL((size_t)1, map->getSize());
	map->registerObjects(v1, m2);
	CPPUNIT_ASSERT_EQUAL((size_t)1, map->getSize());
	map->registerObjects(v2, m2);
	CPPUNIT_ASSERT_EQUAL((size_t)2, map->getSize());
	CPPUNIT_ASSERT(m1 == map->getModelObject(v1));
	CPPUNIT_ASSERT(m2 == map->getModelObject(v2));
	CPPUNIT_ASSERT(v1 == map->getViewObject(m1));
	CPPUNIT_ASSERT(v2 == map->getViewObject(m2));
	map->remove(v1, m1);
	CPPUNIT_ASSERT_EQUAL((size_t)1, map->getSize());
	CPPUNIT_ASSERT(!map->getModelObject(v1));
	CPPUNIT_ASSERT(!map->getViewObject(m1));
	// test throw
	CPPUNIT_ASSERT_THROW(map->registerObjects(TestViewObject::Ptr(), m1),
		sambag::com::exceptions::IllegalStateException);
	CPPUNIT_ASSERT_THROW(map->registerObjects(v1, TestModelObject::Ptr()),
		sambag::com::exceptions::IllegalStateException);
}
//-----------------------------------------------------------------------------
void TestViewModelMap::testHibernate() {
	throw std::exception("not implemented yet");
}
} //namespace
