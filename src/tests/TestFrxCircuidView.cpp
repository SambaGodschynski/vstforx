/*
 * TestFrxCircuidView.cpp
 *
 *  Created on: Thu Aug 23 12:20:07 2012
 *      Author: Johannes Unger
 */

#include "TestFrxCircuidView.hpp"
#include <cppunit/config/SourcePrefix.h>
#include <gui/components/FrxCircuidView.hpp>
#include <gui/components/FrxPluginNode.hpp>

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
} //namespace
