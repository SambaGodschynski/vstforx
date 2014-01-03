/*
 * TestModelFactory.cpp
 *
 *  Created on: Wed Dec 18 09:55:05 2013
 *      Author: Johannes Unger
 */

#include "TestModelFactory.hpp"
#include <cppunit/config/SourcePrefix.h>
#include "DummyFX.h"
#include <processing/ModelFactory.hpp>
#include <processing/ConcreteProcessAdapter.h>
#include <sambag/com/exceptions/IllegalArgumentException.hpp>
#include <com/Serialization.h>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( tests::TestModelFactory );

namespace tests {
//=============================================================================
//  Class TestModelFactory
//=============================================================================
//-----------------------------------------------------------------------------
void TestModelFactory::testCreateProducts() {
    using namespace processing;
    using namespace frx::processing;
    boost::shared_ptr<DummyHostInfo> hI(new DummyHostInfo(44100.f, 255));
    ModelFactory &fac = ModelFactory::instance();
    CPPUNIT_ASSERT(fac.create("frx.processing.internal.Volume", hI));
    CPPUNIT_ASSERT(
        boost::dynamic_pointer_cast<Volume>(
            fac.create("frx.processing.internal.Volume", hI)
        )
    );
    CPPUNIT_ASSERT_THROW(
        fac.create("nope", hI),
        sambag::com::exceptions::IllegalArgumentException
    );
}
//-----------------------------------------------------------------------------
void TestModelFactory::testArchiveRegister() {
    using namespace processing;
    using namespace frx::processing;
    ModelFactory &fac = ModelFactory::instance();
    std::stringstream ss;
    {
        com::oArchive oa(ss);
        fac.registerToArchive(oa);
        boost::shared_ptr<DummyHostInfo> hI(new DummyHostInfo(44100.f, 255));
        ProcessAdapter::Ptr volume = fac.create("frx.processing.internal.Volume", hI);
        oa<<hI;
        oa<<volume;
    }
    com::iArchive ia(ss);
    fac.registerToArchive(ia);
    boost::shared_ptr<DummyHostInfo> hI;
    ProcessAdapter::Ptr volume;
    ia>>hI;
    ia>>volume;
    CPPUNIT_ASSERT(volume);
}
//-----------------------------------------------------------------------------
void TestModelFactory::testGetRegisteredIds() {
    using namespace processing;
    using namespace frx::processing;
    ModelFactory &fac = ModelFactory::instance();
    std::vector<std::string> ids;
    ids.reserve(fac.getNumRegisteredIds());
    fac.getRegisteredIds(ids);
    CPPUNIT_ASSERT(ids.size() == fac.getNumRegisteredIds());
    // test filter
    ids.clear();
    fac.getRegisteredIds(ids, ".*");
    CPPUNIT_ASSERT(ids.size() == fac.getNumRegisteredIds());
    
    ids.clear();
    fac.getRegisteredIds(ids, "no match");
    CPPUNIT_ASSERT_EQUAL((size_t)0, ids.size());
    
    ids.clear();
    fac.getRegisteredIds(ids, "internal\\..*?Step");
    std::stringstream ss;
    std::copy(ids.begin(), ids.end(), std::ostream_iterator<std::string>(ss, " "));
    std::string exp = "internal.OutputStep internal.InputStep ";
    CPPUNIT_ASSERT_EQUAL(exp, ss.str());
    
    
}
} //namespace
