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
#include <processing/interprocess/SessionManager.hpp>
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
    typedef struct Ret { char value[20]; } *RetPtr;
    typedef struct Arg {} *ArgPtr;
};

struct OpHello {
    enum {OPC = 3};
    typedef struct Ret { char value[20]; } *RetPtr;
    typedef struct Arg {} *ArgPtr;
};

struct OpCpyLongString {
    enum {OPC = 4};
    typedef struct Ret {} *RetPtr;
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
            OpWhoAreYou::Ret caller;
            waitForResult(OpWhoAreYou::OPC, caller, 1000);
            std::string rstr("Hello ");
            rstr+=caller.value;
            strcpy((char*)retmem, rstr.c_str());
        }
        if (opc == OpCpyLongString::OPC) {
            std::string ret("no luck, try again");
            void * data;
            TransferReceiverGuardPtr guard;
            boost::tie(data, guard) = getTransferedData(OpCpyLongString::OPC);
            if (data) {
                int byteSize = getTransferedDataSize(OpCpyLongString::OPC);
                char *toString = new char[byteSize+1];
                memcpy(toString, data, byteSize);
                toString[byteSize] = '\0';
                ret=std::string(toString) + " received.";
                delete[] toString;
            } else {
                SAMBAG_LOG_TRACE<<"nothing received";
            }
            transferData(OpCpyLongString::OPC, (void*)ret.c_str(), ret.length(), getTransferSenderGuard());
        }
    }

};
    
struct ClientSession : Session {
    ClientSession(const std::string &id) : Session(id),
        causeChannelOverload(false),
        timeout_catched(false)
    {}
    void processImpl(Opc opc, void *argmen, void *retmem) {
        if (opc == OpWhoAreYou::OPC) {
            // ask for callers name
            char * ret = static_cast<char*>(retmem);
            strcpy(ret, "Mike");
            
            if (causeChannelOverload) {
                try {
                    waitForProcess(OpHello::OPC, 1000);
                } catch (const Session::TimeOut &ex) {
                   timeout_catched = true;
                }
            }
            if (opc = OpCpyLongString::OPC) {}
        }

    }
    
    std::string transferString(const std::string &str) {
        transferData(OpCpyLongString::OPC, (void*)str.c_str(), str.length(), getTransferSenderGuard());
        waitForProcess(OpCpyLongString::OPC, 1000);
        void * data;
        TransferReceiverGuardPtr guard;
        boost::tie(data, guard) = getTransferedData(OpCpyLongString::OPC);
        std::string res;
        if (!data) {
            return "";
        } else {
            int byteSize = getTransferedDataSize(OpCpyLongString::OPC);
            char *toString = new char[byteSize+1];
            memcpy(toString, data, byteSize);
            toString[byteSize] = '\0';
            res = std::string(toString);
            delete[] toString;
        }
        return res;
    }
    
    int add(int a, int b) {
        OpAdd::Arg args;
        args.a = a;
        args.b = b;
        OpAdd::Ret ret;
        waitForResult(OpAdd::OPC, args, ret, 1000);
        return ret.value;
    }
    void closeHost() {
        waitForProcess(OpClose::OPC, 1000);
    }
    std::string greetHost() {
        OpHello::Ret res;
        waitForResult(OpHello::OPC, res, 1000);
        return std::string(res.value);
    }
    bool causeChannelOverload; // Cl requests Ho requests> Cl requests Ho
    bool timeout_catched;
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
    using frx::processing::interprocess::SessionManager;
    std::string sId("testSession-"+SessionManager::createUniqueName());
    boost::thread host( boost::bind( &th_host, sId ));
    boost::this_thread::sleep(boost::posix_time::millisec(1000));
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
    using frx::processing::interprocess::SessionManager;
    std::string sId("testNoHost-"+SessionManager::createUniqueName());
    CPPUNIT_ASSERT_THROW(ClientSession session(sId), Session::Exception);
}
//-----------------------------------------------------------------------------
void TestIPSession::testHostLost() {
    using frx::processing::interprocess::SessionManager;
    std::string sId("testHostLost-"+SessionManager::createUniqueName());
    boost::thread host( boost::bind( &th_host, sId ));
    boost::this_thread::sleep(boost::posix_time::millisec(1000));
    ClientSession session(sId);
    CPPUNIT_ASSERT_EQUAL( (int)2, session.add(1, 1) );
    session.closeHost();
    host.join();
    CPPUNIT_ASSERT_THROW(session.add(1, 1), Session::TimeOut);
}
//-----------------------------------------------------------------------------
void TestIPSession::testFailures() {
    using frx::processing::interprocess::SessionManager;
    {
        std::string sId("testFailures3-"+SessionManager::createUniqueName());
        HostSession host_session(sId);
        CPPUNIT_ASSERT_THROW(HostSession second(sId), Session::Exception);
        ClientSession session(sId);
        CPPUNIT_ASSERT_EQUAL( (int)2, session.add(1, 1) );
    }
    { // cause overload
        std::string sId("testFailures3-"+SessionManager::createUniqueName());
        boost::thread host( boost::bind( &th_host, sId ));
        boost::this_thread::sleep(boost::posix_time::millisec(1000));
        ClientSession session(sId);
        
        session.causeChannelOverload = true;
        CPPUNIT_ASSERT_EQUAL( std::string("Hello Mike"), session.greetHost() );
        CPPUNIT_ASSERT( session.timeout_catched );
        session.closeHost();
        host.join();
    }
}
//-----------------------------------------------------------------------------
void TestIPSession::testTransferData() {
    using frx::processing::interprocess::SessionManager;
    std::string sId("testTransferData."+SessionManager::createUniqueName());
    boost::thread host( boost::bind( &th_host, sId ));
    boost::this_thread::sleep(boost::posix_time::millisec(1000));
    ClientSession session(sId);
    
    CPPUNIT_ASSERT_EQUAL(std::string("hallo received."), session.transferString("hallo"));
    
    // create big string
    std::stringstream ss;
    for (int i=0; i<1024; ++i) {
        ss<<i<<", ";
    }
    std::string bigString = ss.str();
    std::string exp = bigString + " received.";
    
    CPPUNIT_ASSERT_EQUAL(exp, session.transferString(bigString));
    
    session.closeHost();
    host.join();
}
//-----------------------------------------------------------------------------
namespace {
    struct OP0 {
        struct A {
        };
        typedef helper::AutoOPC<Loki::NullType> OpcManager;
    };
    struct OP1 {
        struct A {
            typedef struct Arg { char a; char b; } *ArgPtr;
            typedef struct Ret {} *RetPtr;
        };
        typedef LOKI_TYPELIST_1(A) OPs;
        typedef helper::AutoOPC<OPs> OpcManager;
    };
    struct OP2 {
        struct A {
            typedef struct Arg { char a; } *ArgPtr;
            typedef struct Ret { char a[10]; } *RetPtr;
        };
        struct B {
            typedef struct Arg { char a[3]; } *ArgPtr;
            typedef struct Ret {} *RetPtr;
        };
        typedef LOKI_TYPELIST_2(A, B) OPs;
        typedef helper::AutoOPC<OPs> OpcManager;
    };
    struct OP3 {
        struct A {
            typedef struct Arg { char a[10]; } *ArgPtr;
            typedef struct Ret { char a[30]; } *RetPtr;
        };
        struct B {
            typedef struct Arg { char a[20]; } *ArgPtr;
            typedef struct Ret { char a[20]; } *RetPtr;
        };
        struct C {
            typedef struct Arg { char a[30]; } *ArgPtr;
            typedef struct Ret { char a[10]; } *RetPtr;
        };
        typedef LOKI_TYPELIST_3(A,B,C) OPs;
        typedef helper::AutoOPC<OPs> OpcManager;
    };

}
void TestIPSession::testHelper() {
    BOOST_STATIC_ASSERT(OP0::OpcManager::MaxArgmemSize==1);
    BOOST_STATIC_ASSERT(OP0::OpcManager::MaxRetmemSize==1);
    
    BOOST_STATIC_ASSERT(OP1::OpcManager::MaxArgmemSize==2);
    BOOST_STATIC_ASSERT(OP1::OpcManager::MaxRetmemSize==1);
    
    BOOST_STATIC_ASSERT(OP2::OpcManager::MaxArgmemSize==3);
    BOOST_STATIC_ASSERT(OP2::OpcManager::MaxRetmemSize==10);

    BOOST_STATIC_ASSERT(OP3::OpcManager::MaxArgmemSize==30);
    BOOST_STATIC_ASSERT(OP3::OpcManager::MaxRetmemSize==30);
}

} //namespace




