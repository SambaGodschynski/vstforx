/*
 * TestViewFactory.cpp
 *
 *  Created on: Wed Dec 18 09:57:35 2013
 *      Author: Johannes Unger
 */

#include "TestViewFactory.hpp"
#include <cppunit/config/SourcePrefix.h>
#include <gui/ViewFactory.hpp>
#include <gui/components/FrxConcreteProcessor.hpp>
#include <com/Serialization.h>
#include <sambag/com/exceptions/IllegalArgumentException.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( tests::TestViewFactory );

namespace tests {
//=============================================================================
//  Class TestViewFactory
//=============================================================================
//-----------------------------------------------------------------------------
void TestViewFactory::testCreateProducts() {
    using namespace frx::gui;
    using namespace frx::gui::components;
    ViewFactory &fac = ViewFactory::instance();
    CPPUNIT_ASSERT(fac.create("frx.gui.internal.Volume"));
    CPPUNIT_ASSERT( std::dynamic_pointer_cast<FrxVolumeNode>(
            fac.create("frx.gui.internal.Volume")
        )
    );
    CPPUNIT_ASSERT_THROW(
        fac.create("nope"),
        sambag::com::exceptions::IllegalArgumentException
    );
}
//-----------------------------------------------------------------------------
void TestViewFactory::testArchiveRegister() {
    using namespace frx::gui;
    using namespace frx::gui::components;
    ViewFactory &fac = ViewFactory::instance();
    std::stringstream ss;
    {
        com::oArchive oa(ss);
        fac.registerToArchive(oa);
        FrxProcessorNode::Ptr volume = fac.create("frx.gui.internal.Volume");
        oa<<volume;
    }
    com::iArchive ia(ss);
    fac.registerToArchive(ia);
    FrxProcessorNode::Ptr volume;
    ia>>volume;
    CPPUNIT_ASSERT(volume);
}
//-----------------------------------------------------------------------------
void TestViewFactory::testGetRegisteredIds() {
    using namespace frx::gui;
    using namespace frx::gui::components;
    ViewFactory &fac = ViewFactory::instance();
    std::vector<std::string> ids;
    ids.reserve(fac.getNumRegisteredIds());
    fac.getRegisteredIds(ids);
    SAMBAG_ASSERT(ids.size() == fac.getNumRegisteredIds());
}
} //namespace
