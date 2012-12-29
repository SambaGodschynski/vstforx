/*
 * TestFrxCircuidView.cpp
 *
 *  Created on: Thu Aug 23 12:20:07 2012
 *      Author: Johannes Unger
 */

#include "TestFrxCircuidView.hpp"
#include <cppunit/config/SourcePrefix.h>
#include <gui/components/FrxCircuidView.hpp>
#include <gui/components/FrxConcreteProcessor.hpp>
#include <processing/IModelController.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( tests::TestFrxCircuidView );

namespace tests {
//=============================================================================
//  Class TestFrxCircuidView
//=============================================================================
//-----------------------------------------------------------------------------
void TestFrxCircuidView::testZOrder() {
	using namespace frx::gui::components;
	using namespace sambag::disco::components;
	FrxCircuidView::Ptr circ = FrxCircuidView::create();
	const AContainer::Components &comps = circ->getContentPane()->getComponents();
	FrxPluginNode::Ptr p01 = FrxPluginNode::create();
	FrxPluginNode::Ptr p02 = FrxPluginNode::create();
	FrxPluginNode::Ptr p025 = FrxPluginNode::create();
	FrxPluginNode::Ptr p03 = FrxPluginNode::create();
	FrxPluginNode::Ptr p04 = FrxPluginNode::create();
	circ->add(p01, 4.f);
	circ->add(p02, 3.f);
	circ->add(p025, 3.5f);
	circ->add(p03, 1.f);
	circ->add(p04, 2.f);
	std::string expStr("{FrxSelection[1], FrxPlugin[1], FrxPlugin[2], FrxPlugin[3], FrxPlugin[3.5], FrxPlugin[4]}");
	CPPUNIT_ASSERT_EQUAL(expStr, circ->componentsToString());
}
namespace {
	using namespace sambag::disco::components;
	using namespace frx::gui::components;
	int filterFunc( AComponent::Ptr c ) {
		return 1;
	}
	struct TestFilter {
		int operator()( AComponent::Ptr c ) {
			float z = 0;
			c->getClientProperty(FrxCircuidView::PROPERTY_ZORDER, z);
				if (z>=99.f)
					return 1;
				return 0;
		}
		int filter(AComponent::Ptr c) {
			return -1;
		}
	};
} // namespace
//-----------------------------------------------------------------------------
void TestFrxCircuidView::testFindComponentsFiltered() {
	using namespace frx::gui::components;
	using namespace sambag::disco::components;
	FrxCircuidView::Ptr circ = FrxCircuidView::create();
	FrxPluginNode::Ptr p01 = FrxPluginNode::create();
	FrxPluginNode::Ptr p02 = FrxPluginNode::create();
	FrxPluginNode::Ptr p03 = FrxPluginNode::create();
	FrxPluginNode::Ptr p04 = FrxPluginNode::create();
	circ->add(p01, 4.f);
	circ->add(p02, 3.f);
	circ->add(p03, 99.f);
	circ->add(p04, 100.f);

	std::vector<AComponent::Ptr> res;
	TestFilter filter;
	circ->findComponents(res, filter);
	CPPUNIT_ASSERT_EQUAL((size_t)2, res.size());
	CPPUNIT_ASSERT(res[0] == p03);
	CPPUNIT_ASSERT(res[1] == p04);
	res.clear();
	circ->findComponents(res, filterFunc);
	CPPUNIT_ASSERT_EQUAL((size_t)4 + 1, res.size());

	res.clear();
	TestFilter f;
	boost::function<int(AComponent::Ptr)> bf = 
		boost::bind(&TestFilter::filter, &f, _1);
	circ->findComponents(res, bf);
	CPPUNIT_ASSERT_EQUAL((size_t)0, res.size());
}
//-----------------------------------------------------------------------------
void TestFrxCircuidView::testFindComponentsInArea() {
	using namespace frx::gui::components;
	using namespace sambag::disco;
	using namespace sambag::disco::components;
	FrxCircuidView::Ptr circ = FrxCircuidView::create();
	FrxPluginNode::Ptr p01 = FrxPluginNode::create();
	FrxPluginNode::Ptr p02 = FrxPluginNode::create();
	FrxPluginNode::Ptr p03 = FrxPluginNode::create();
	FrxPluginNode::Ptr p04 = FrxPluginNode::create();
	circ->add(p01, 1.f);
	p01->setSize(Dimension(15,15));
	p01->setLocation(0,0); // outside

	circ->add(p02, 2.f);
	p02->setSize(Dimension(15,15));
	p02->setLocation(40,40); // center is not in searching area  

	circ->add(p03, 3.f);
	p03->setSize(Dimension(15,15));
	p03->setLocation(45,45); // fits

	circ->add(p04, 4.f);
	p04->setSize(Dimension(15,15));
	p04->setLocation(45,45); // beyond z

	std::vector<AComponent::Ptr> res;
	circ->findComponentsInArea(res, Rectangle(50,50,100,100), 1.f, 3.f);
	CPPUNIT_ASSERT_EQUAL((size_t)1, res.size());
	CPPUNIT_ASSERT(res[0] == p03);
	res.clear();

	circ->findComponentsInArea(res, Rectangle(50,50,100,100), 4.f, 4.f);
	CPPUNIT_ASSERT_EQUAL((size_t)1, res.size());
	CPPUNIT_ASSERT(res[0] == p04);
	res.clear();

	circ->findComponentsInArea(res, Rectangle(50,50,100,100));
	CPPUNIT_ASSERT_EQUAL((size_t)2, res.size());
	CPPUNIT_ASSERT(res[0] == p03);
	CPPUNIT_ASSERT(res[1] == p04);
}
//-----------------------------------------------------------------------------
void TestFrxCircuidView::testFindAllComponents() {
	using namespace frx::gui::components;
	using namespace sambag::disco;
	using namespace sambag::disco::components;
	FrxCircuidView::Ptr circ = FrxCircuidView::create();
	FrxPluginNode::Ptr p01 = FrxPluginNode::create();
	FrxPluginNode::Ptr p02 = FrxPluginNode::create();
	FrxPluginNode::Ptr p03 = FrxPluginNode::create();
	FrxPluginNode::Ptr p04 = FrxPluginNode::create();
	circ->add(p01, 1.f);
	p01->setSize(Dimension(15,15));
	p01->setLocation(0,0);

	circ->add(p02, 2.f);
	p02->setSize(Dimension(15,15));
	p02->setLocation(40,40);

	circ->add(p03, 3.f);
	p03->setSize(Dimension(15,15));
	p03->setLocation(45,45);

	circ->add(p04, 4.f);
	p04->setSize(Dimension(15,15));
	p04->setLocation(45,45);

	std::vector<AComponent::Ptr> res;
	circ->findAllComponents(res, 3.f, 4.f);
	CPPUNIT_ASSERT_EQUAL((size_t)2, res.size());
	CPPUNIT_ASSERT(res[0] == p03);
	CPPUNIT_ASSERT(res[1] == p04);
	res.clear();

	circ->findAllComponents(res, 4.f, 4.f);
	CPPUNIT_ASSERT_EQUAL((size_t)1, res.size());
	CPPUNIT_ASSERT(res[0] == p04);
	res.clear();


}
//-----------------------------------------------------------------------------
void TestFrxCircuidView::testGetIndexOf() {
	using namespace frx::gui::components;
	using namespace sambag::disco::components;
	FrxCircuidView::Ptr circ = FrxCircuidView::create();
	const AContainer::Components &comps = circ->getContentPane()->getComponents();
	FrxPluginNode::Ptr p01 = FrxPluginNode::create();
	FrxPluginNode::Ptr p02 = FrxPluginNode::create();
	FrxPluginNode::Ptr p025 = FrxPluginNode::create();
	FrxPluginNode::Ptr p03 = FrxPluginNode::create();
	FrxPluginNode::Ptr p04 = FrxPluginNode::create();
	FrxPluginNode::Ptr p05 = FrxPluginNode::create();
	FrxPluginNode::Ptr p06 = FrxPluginNode::create();
	FrxPluginNode::Ptr p07 = FrxPluginNode::create();
	circ->add(p01, 0.f);
	circ->add(p02, 3.f);
	circ->add(p025, 3.5f);
	circ->add(p03, 1.f);
	circ->add(p04, 2.f);
	circ->add(p05, 5.f);
	circ->add(p06, 6.f);
	circ->add(p07, 7.f);
	//std::fstream f("outp.txt", std::fstream::out);
	//f<<circ->componentsToString();
	//f.close();
	std::string ist("{FrxPlugin[0], FrxSelection[1], FrxPlugin[1], FrxPlugin[2], FrxPlugin[3], FrxPlugin[3.5], FrxPlugin[5], FrxPlugin[6], FrxPlugin[7]}");
	CPPUNIT_ASSERT_EQUAL(ist, circ->componentsToString());
	CPPUNIT_ASSERT_EQUAL((int)0, circ->getIndexOf(0.));
	CPPUNIT_ASSERT_EQUAL((int)1, circ->getIndexOf(0.5));
	CPPUNIT_ASSERT_EQUAL((int)1, circ->getIndexOf(1.));
	CPPUNIT_ASSERT_EQUAL((int)3, circ->getIndexOf(2.));
	CPPUNIT_ASSERT_EQUAL((int)4, circ->getIndexOf(3.));
	CPPUNIT_ASSERT_EQUAL((int)5, circ->getIndexOf(3.5));
	CPPUNIT_ASSERT_EQUAL((int)6, circ->getIndexOf(5.));
	CPPUNIT_ASSERT_EQUAL((int)8, circ->getIndexOf(7.));
	CPPUNIT_ASSERT_EQUAL((int)-1, circ->getIndexOf(8.));
	CPPUNIT_ASSERT_EQUAL((int)0, circ->getIndexOf(-1.));
}
} //namespace
