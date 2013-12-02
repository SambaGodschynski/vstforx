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

struct OpWhoAreYou {
    enum {OPC = 2};
    typedef struct Ret { char * value; } *RetPtr;
    typedef struct Arg {} *ArgPtr;
};

struct OpHello {
    enum {OPC = 3};
    typedef struct Ret { char * value; } *RetPtr;
    typedef struct Arg {} *ArgPtr;
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
        Session(id, ChannelSize(100,100), ChannelSize(100,100) ),
        isRunning(true)
    {
    }
    void processImpl(Opc opc, void *argmen, void *retmem) {
        if (opc == OpAdd::OPC) {
            perform<OpAdd>(argmen, retmem);
        }
        if (opc == OpClose::OPC) {
            isRunning = false;
        }
        if (opc == OpHello::OPC) {
            // ask for callers name
            char * name = waitForResult<char*>(OpWhoAreYou::OPC);
            char * ret = static_cast<char*>(retmem);
            std::string rstr("Hello ");
            rstr+=name;
            strcpy(ret, rstr.c_str());
        }
    }
};
    
struct ClientSession : Session {
    ClientSession(const std::string &id) : Session(id),
        causeChannelOverload(false)
    {}
    void processImpl(Opc opc, void *argmen, void *retmem) {
        if (opc == OpWhoAreYou::OPC) {
            // ask for callers name
            char * ret = static_cast<char*>(retmem);
            strcpy(ret, "Mike");
            
            if (causeChannelOverload) {
                waitForResult<char*>(OpHello::OPC);
            }
        }

    }
    int add(int a, int b) {
        OpAdd::ArgPtr args = static_cast<OpAdd::ArgPtr>(getArgmem());
        args->a = a;
        args->b = b;
        OpAdd::RetPtr ret = waitForResult<OpAdd::RetPtr>(OpAdd::OPC);
        return ret->value;
    }
    void closeHost() {
        waitForResult(OpClose::OPC);
    }
    std::string greetHost() {
        char * res = waitForResult<char*>(OpHello::OPC);
        return std::string(res);
    }
    bool causeChannelOverload; // Cl requests Ho requests> Cl requests Ho
};

void th_host(std::string sId) {
    HostSession session(sId);
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
    std::string sId("testSession");
    boost::thread host( boost::bind( &th_host, sId ));
    boost::this_thread::sleep(boost::posix_time::millisec(100));
    ClientSession session(sId);
    CPPUNIT_ASSERT_EQUAL( (int)2, session.add(1, 1) );
    CPPUNIT_ASSERT_EQUAL( (int)20, session.add(10, 10) );
    CPPUNIT_ASSERT_EQUAL( (int)200, session.add(100, 100) );
    CPPUNIT_ASSERT_EQUAL( std::string("Hello Mike"), session.greetHost() );
    session.closeHost();
    host.join();
}
//-----------------------------------------------------------------------------
void TestIPSession::testNoHost() {
    std::string sId("testNoHost");
    CPPUNIT_ASSERT_THROW(ClientSession session(sId), Session::Exception);
}
//-----------------------------------------------------------------------------
void TestIPSession::testHostLost() {
    std::string sId("testHostLost");
    boost::thread host( boost::bind( &th_host, sId ));
    boost::this_thread::sleep(boost::posix_time::millisec(100));
    ClientSession session(sId);
    CPPUNIT_ASSERT_EQUAL( (int)2, session.add(1, 1) );
    session.closeHost();
    host.join();
    CPPUNIT_ASSERT_THROW(session.add(1, 1), Session::TimeOut);
}
} //namespace




