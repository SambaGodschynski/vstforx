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
#include <boost/archive/text_oarchive.hpp> 
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/access.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( tests::TestViewModelMap );

namespace tests {
template <class Base>
struct TestClass : public Base {
	int id;
	typedef boost::shared_ptr<TestClass> Ptr;
	static Ptr create(int id=0) {
		Ptr res(new TestClass());
		res->id = id;
		return res;
	}
	template <typename Archive> 
	void serialize(Archive &ar, const unsigned int version) {
		ar & boost::serialization::base_object<Base>(*this); 
		ar & id;
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
	using namespace frx::processing;
	using namespace frx::gui;
	ViewModelMap::Ptr map = ViewModelMap::create();
	typedef TestClass<ViewObject> TestViewObject;
	typedef TestClass<ModelObject> TestModelObject;
	TestViewObject::Ptr v1 = TestViewObject::create(1);
	TestViewObject::Ptr v2 = TestViewObject::create(2);
	TestViewObject::Ptr v3 = TestViewObject::create(3);
	TestViewObject::Ptr v4 = TestViewObject::create(4);
	TestModelObject::Ptr m1 = TestModelObject::create(1);
	TestModelObject::Ptr m2 = TestModelObject::create(2);
	TestModelObject::Ptr m3 = TestModelObject::create(3);
	TestModelObject::Ptr m4 = TestModelObject::create(4);
	map->registerObjects(v1, m1);
	map->registerObjects(v2, m2);
	map->registerObjects(v3, m3);
	map->registerObjects(v4, m4);
	CPPUNIT_ASSERT_EQUAL((size_t)4, map->getSize());
	// serialize
	std::stringstream ss;
	{
		boost::archive::text_oarchive ar(ss);
		ar.register_type<TestViewObject>();
		ar & v1 & v2 & v3 & v4;
		map->lock(ar);
		CPPUNIT_ASSERT(map->isLocked());
		CPPUNIT_ASSERT_EQUAL((size_t)4, map->getSize());
		v1 = v2 = v3 = v4 = TestViewObject::Ptr();
		CPPUNIT_ASSERT(!v1 && !v2 && !v3 && !v4);
	}
	{
		boost::archive::text_iarchive ar(ss);
		ar.register_type<TestViewObject>();
		ar & v1 & v2 & v3 & v4;
		CPPUNIT_ASSERT(v1 && v2 && v3 && v4);
		CPPUNIT_ASSERT((v1->id < v2->id) && (v2->id < v3->id) && (v3->id < v4->id));
		map->unlock(ar);
		CPPUNIT_ASSERT(!map->isLocked());
		CPPUNIT_ASSERT(m1 == map->getModelObject(v1));
		CPPUNIT_ASSERT(m2 == map->getModelObject(v2));
		CPPUNIT_ASSERT(m3 == map->getModelObject(v3));
		CPPUNIT_ASSERT(m4 == map->getModelObject(v4));
		CPPUNIT_ASSERT(v1 == map->getViewObject(m1));
		CPPUNIT_ASSERT(v2 == map->getViewObject(m2));
		CPPUNIT_ASSERT(v3 == map->getViewObject(m3));
		CPPUNIT_ASSERT(v4 == map->getViewObject(m4));
	}
	TestViewObject::Ptr v5 = TestViewObject::create(5);
	TestModelObject::Ptr m5 = TestModelObject::create(5);
	map->registerObjects(v5, m5);
	CPPUNIT_ASSERT(m5 == map->getModelObject(v5));
	CPPUNIT_ASSERT(v5 == map->getViewObject(m5));
}
} //namespace
