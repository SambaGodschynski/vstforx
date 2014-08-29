/*
 * TestLuaUserData.cpp
 *
 *  Created on: Fri Aug 29 10:44:19 2014
 *      Author: Johannes Unger
 */

#include "TestLuaUserData.hpp"
#include <cppunit/config/SourcePrefix.h>
#include <com/Serialization.h>
#include <scripts/LuaUserData.hpp>
#include <map>
#include <set>
// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( tests::TestLuaUserData );

namespace tests {
//=============================================================================
//  Class TestLuaUserData
//=============================================================================
//-----------------------------------------------------------------------------
void TestLuaUserData::testLuaUserData() {
    using namespace frx::scripts;
    LuaUserData data;
    data.add("keys.first", "1");
    data.add("keys.first", "2");
    data.add("keys.first", "3");
    data.add("keys.second", "321");
    data.add("keys.second", "23");
    data.add("keys.second", "1");
    data.add("keys.second", "00");

    {
        std::vector<std::string> out;
        data.get("blah", out);
        CPPUNIT_ASSERT_EQUAL((size_t)0, out.size());
    }

    
    {
        std::vector<std::string> out;
        data.get("keys.first", out);
        CPPUNIT_ASSERT_EQUAL((size_t)3, out.size());
        CPPUNIT_ASSERT_EQUAL(std::string("1"), out[0]);
        CPPUNIT_ASSERT_EQUAL(std::string("2"), out[1]);
        CPPUNIT_ASSERT_EQUAL(std::string("3"), out[2]);
    }
    
    {
        std::vector<std::string> out;
        data.get("keys.second", out);
        CPPUNIT_ASSERT_EQUAL((size_t)4, out.size());
        CPPUNIT_ASSERT_EQUAL(std::string("321"), out[0]);
        CPPUNIT_ASSERT_EQUAL(std::string("23"), out[1]);
        CPPUNIT_ASSERT_EQUAL(std::string("1"), out[2]);
        CPPUNIT_ASSERT_EQUAL(std::string("00"), out[3]);
    }
    
    {
        std::vector<std::string> out;
        data.get("keys", out);
        CPPUNIT_ASSERT_EQUAL((size_t)0, out.size());
    }
}
//-----------------------------------------------------------------------------
void TestLuaUserData::testPersistLuaUserData() {
    using namespace frx::scripts;
    std::stringstream ss;
    {
        LuaUserData data;
        data.add("keys.first", "1");
        data.add("keys.first", "2");
        data.add("keys.first", "3");
        data.add("keys.second", "321");
        data.add("keys.second", "23");
        data.add("keys.second", "1");
        data.add("keys.second", "00");
        com::oArchive oa(ss);
        oa<<data;
    }
    LuaUserData data;
    com::iArchive ia(ss);
    ia>>data;
    
    {
        std::vector<std::string> out;
        data.get("blah", out);
        CPPUNIT_ASSERT_EQUAL((size_t)0, out.size());
    }

    
    {
        std::vector<std::string> out;
        data.get("keys.first", out);
        CPPUNIT_ASSERT_EQUAL((size_t)3, out.size());
        CPPUNIT_ASSERT_EQUAL(std::string("1"), out[0]);
        CPPUNIT_ASSERT_EQUAL(std::string("2"), out[1]);
        CPPUNIT_ASSERT_EQUAL(std::string("3"), out[2]);
    }
    
    {
        std::vector<std::string> out;
        data.get("keys.second", out);
        CPPUNIT_ASSERT_EQUAL((size_t)4, out.size());
        CPPUNIT_ASSERT_EQUAL(std::string("321"), out[0]);
        CPPUNIT_ASSERT_EQUAL(std::string("23"), out[1]);
        CPPUNIT_ASSERT_EQUAL(std::string("1"), out[2]);
        CPPUNIT_ASSERT_EQUAL(std::string("00"), out[3]);
    }
    
    {
        std::vector<std::string> out;
        data.get("keys", out);
        CPPUNIT_ASSERT_EQUAL((size_t)0, out.size());
    }
    
}
//-----------------------------------------------------------------------------
void TestLuaUserData::testMigration() {
    using namespace frx::scripts;
    std::multimap<std::string, std::string> map;
    map.insert(std::make_pair("keys.first", "1"));
    map.insert(std::make_pair("keys.first", "2"));
    map.insert(std::make_pair("keys.first", "3"));
    map.insert(std::make_pair("keys.second", "321"));
    map.insert(std::make_pair("keys.second", "23"));
    map.insert(std::make_pair("keys.second", "1"));
    map.insert(std::make_pair("keys.second", "00"));
    
    LuaUserData data;
    data.migrate(map);
    // multimap data value ordering not guaranteed
    typedef std::set<std::string> Set;
    {
        Set out;
        data.get("keys.first", out);
        CPPUNIT_ASSERT_EQUAL((size_t)3, out.size());
        CPPUNIT_ASSERT(out.find("xxx")==out.end());
        CPPUNIT_ASSERT(out.find("1")!=out.end());
        CPPUNIT_ASSERT(out.find("2")!=out.end());
        CPPUNIT_ASSERT(out.find("3")!=out.end());
        
    }
    {
        Set out;
        data.get("keys.second", out);
        CPPUNIT_ASSERT_EQUAL((size_t)4, out.size());
        CPPUNIT_ASSERT(out.find("321")!=out.end());
        CPPUNIT_ASSERT(out.find("23")!=out.end());
        CPPUNIT_ASSERT(out.find("1")!=out.end());
        CPPUNIT_ASSERT(out.find("00")!=out.end());
    }
}
} //namespace
