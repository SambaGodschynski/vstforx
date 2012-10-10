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

namespace frx { namespace processing {
	IModelController::Ptr
	getModelController(frx::gui::components::FrxCircuidView::Ptr) {
		return IModelController::Ptr();
	}
}}
namespace frx { namespace gui {
	IViewModelMap::Ptr 
	getViewModelMap(components::FrxCircuidViewPtr view)
	{
		return IViewModelMap::Ptr();
	}
}}

namespace tests {
//=============================================================================
//  Class TestFrxCircuidView
//=============================================================================
//-----------------------------------------------------------------------------
void TestFrxCircuidView::testZOrder() {
	using namespace frx::gui::components;
	using namespace sambag::disco::components;
	FrxCircuidView::Ptr circ = FrxCircuidView::create();
	FrxPluginNode::Ptr p01 = FrxPluginNode::create();
	FrxPluginNode::Ptr p02 = FrxPluginNode::create();
	FrxPluginNode::Ptr p03 = FrxPluginNode::create();
	FrxPluginNode::Ptr p04 = FrxPluginNode::create();
	circ->add(p01, 4.f);
	circ->add(p02, 3.f);
	circ->add(p03, 1.f);
	circ->add(p04, 2.f);
	const AContainer::Components &comps = circ->getComponents();
	CPPUNIT_ASSERT(comps[0]);
	CPPUNIT_ASSERT(comps[0] == p03);
	CPPUNIT_ASSERT(comps[1]);
	CPPUNIT_ASSERT(comps[1] == p04);
	CPPUNIT_ASSERT(comps[2]);
	CPPUNIT_ASSERT(comps[2] == p02);
	CPPUNIT_ASSERT(comps[3]);
	CPPUNIT_ASSERT(comps[3] == p01);
}
namespace {
	int filterFunc( sambag::disco::components::AComponent::Ptr c ) {
		return 1;
	}
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
	circ->add(p03, 1.f);
	circ->add(p04, 2.f);

	struct Filter {
		int operator()( AComponent::Ptr c ) {
			float z = 0;
			c->getClientProperty(FrxCircuidView::PROPERTY_ZORDER, z);
			if (z<3.f)
				return 1;
			return 0;
		}
		int filter(AComponent::Ptr c) {
			return -1;
		}
	};
	std::vector<AComponent::Ptr> res;
	circ->findComponents(res, Filter());
	CPPUNIT_ASSERT_EQUAL((size_t)2, res.size());
	CPPUNIT_ASSERT(res[0] == p03);
	CPPUNIT_ASSERT(res[1] == p04);
	res.clear();
	circ->findComponents(res, filterFunc);
	CPPUNIT_ASSERT_EQUAL((size_t)4, res.size());

	res.clear();
	Filter f;
	circ->findComponents(res, boost::bind(&Filter::filter, &f, _1));
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
	circ->findComponentsInArea(res, Rectangle(50,50,100,100));
	CPPUNIT_ASSERT_EQUAL((size_t)2, res.size());
	CPPUNIT_ASSERT(res[0] == p03);
	CPPUNIT_ASSERT(res[1] == p04);


}
} //namespace
