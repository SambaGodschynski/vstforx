/*
 * TestIPSession.cpp
 *
 *  Created on: Thu Nov 28 11:21:11 2013
 *      Author: Johannes Unger
 */

#include "TestIPSession.hpp"
#include <cppunit/config/SourcePrefix.h>
#include <processing/interprocess/Session.hpp>
#include <sambag/com/Thread.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( tests::TestIPSession );


namespace {
using namespace frx::processing::interprocess;

struct OpAdd {
    enum {OPC = 0};
    typedef struct Ret { int value; } *RetPtr;
    typedef struct Arg { int a, b; } *ArgPtr;
    static void perform(ArgPtr arg, RetPtr ret) {
        ret->value = arg->a + arg->b;
    }
};

struct OpClose {
    enum {OPC = 1};
};

template <class Op>
void perform(void *argmem, void *retmem) {
    typename Op::ArgPtr arg = static_cast<typename Op::ArgPtr>(argmem);
    typename Op::RetPtr ret = static_cast<typename Op::RetPtr>(retmem);
    Op::perform(arg, ret);
}

struct HostSession : Session {
    bool isRunning;
    HostSession(const std::string &id) :
        Session(id, ChannelSize(100,100), ChannelSize(10,10) ),
        isRunning(true)
    {
    }
    Opc processImpl(Opc opc, void *argmen, void *retmem) {
        if (opc == OpAdd::OPC) {
            perform<OpAdd>(argmen, retmem);
        }
        if (opc == OpClose::OPC) {
            isRunning = false;
        }
        return PROCEEDED;
    }
};
    
struct ClientSession : Session {
    ClientSession(const std::string &id) : Session(id) {}
    Opc processImpl(Opc opc, void *argmen, void *retmem) {
        return PROCEEDED;
    }
    int add(int a, int b) {
        waitForResult(OpAdd::OPC);
        OpAdd::RetPtr ret = static_cast<OpAdd::RetPtr>(getRetmem());
        return ret->value;
    }
    int close() {
        waitForResult(OpClose::OPC);
        return PROCEEDED;
    }
};

void th_host() {
    HostSession session("TestIPSession");
    while(session.isRunning) {
        boost::this_thread::sleep(boost::posix_time::millisec(100));
    }
}
} // namespace





namespace tests {
//=============================================================================
//  Class TestIPSession
//=============================================================================
//-----------------------------------------------------------------------------
void TestIPSession::testSession() {
    boost::thread host( &th_host );
    ClientSession session("TestIPSession");
    CPPUNIT_ASSERT_EQUAL( (int)2, session.add(1, 1) );
    CPPUNIT_ASSERT_EQUAL( (int)20, session.add(10, 10) );
    CPPUNIT_ASSERT_EQUAL( (int)200, session.add(100, 100) );
    session.close();
    host.join();
}
//-----------------------------------------------------------------------------
void TestIPSession::testSessionHostStartDelayed() {
}
//-----------------------------------------------------------------------------
void TestIPSession::testNoHost() {
}
//-----------------------------------------------------------------------------
void TestIPSession::testHostLost() {
}
} //namespace
