/*
 * Session.hpp
 *
 *  Created on: Thu Nov 28 11:20:05 2013
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_SESSION_H
#define SAMBAG_SESSION_H

#include <boost/shared_ptr.hpp>
#include <string>
#include <boost/interprocess/sync/interprocess_upgradable_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/sharable_lock.hpp>
#include <processing/Frames.h>
#include <sambag/com/SharedMemory.hpp>
#include <sambag/com/PlacementAlloc.hpp>
#include <processing/AsyncBuffer.hpp>
#include <boost/static_assert.hpp>
#include <boost/optional.hpp>
#include "ShmCom.hpp"
#include <sambag/com/Thread.hpp>
#include <sambag/com/exceptions/IllegalStateException.hpp>
#include <loki/Typelist.h>
#include <loki/HierarchyGenerators.h>
#include <com/FrxConfig.h>


#define FRX_OP_RET() typedef struct Ret {} *RetPtr
#define FRX_OP_RET_1(a1) typedef struct Ret {a1;} *RetPtr
#define FRX_OP_RET_2(a1,a2) typedef struct Ret {a1;a2;} *RetPtr
#define FRX_OP_RET_3(a1,a2,a3) typedef struct Ret {a1;a2;a3;} *RetPtr
#define FRX_OP_RET_4(a1,a2,a3,a4) typedef struct Ret {a1;a2;a3;a4;} *RetPtr
#define FRX_OP_RET_5(a1,a2,a3,a4,a5) typedef struct Ret {a1;a2;a3;a4;a5;} *RetPtr
#define FRX_OP_RET_6(a1,a2,a3,a4,a5,a6) typedef struct Ret {a1;a2;a3;a4;a5;a6;} *RetPtr
#define FRX_OP_RET_7(a1,a2,a3,a4,a5,a6,a7) typedef struct Ret {a1;a2;a3;a4;a5;a6;a7;} *RetPtr
#define FRX_OP_RET_8(a1,a2,a3,a4,a5,a6,a7,a8) typedef struct Ret {a1;a2;a3;a4;a5;a6;a7;a8;} *RetPtr
#define FRX_OP_RET_9(a1,a2,a3,a4,a5,a6,a7,a8,a9) typedef struct Ret {a1;a2;a3;a4;a5;a6;a7;a8;a9;} *RetPtr
#define FRX_OP_RET_10(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10) typedef struct Ret {a1;a2;a3;a4;a5;a6;a7;a8;a9;a10;} *RetPtr

#define FRX_OP_ARG() typedef struct Arg {} *ArgPtr
#define FRX_OP_ARG_1(a1) typedef struct Arg {a1;} *ArgPtr
#define FRX_OP_ARG_2(a1,a2) typedef struct Arg {a1;a2;} *ArgPtr
#define FRX_OP_ARG_3(a1,a2,a3) typedef struct Arg {a1;a2;a3;} *ArgPtr
#define FRX_OP_ARG_4(a1,a2,a3,a4) typedef struct Arg {a1;a2;a3;a4;} *ArgPtr
#define FRX_OP_ARG_5(a1,a2,a3,a4,a5) typedef struct Arg {a1;a2;a3;a4;a5;} *ArgPtr
#define FRX_OP_ARG_6(a1,a2,a3,a4,a5,a6) typedef struct Arg {a1;a2;a3;a4;a5;a6;} *ArgPtr
#define FRX_OP_ARG_7(a1,a2,a3,a4,a5,a6,a7) typedef struct Arg {a1;a2;a3;a4;a5;a6;a7;} *ArgPtr
#define FRX_OP_ARG_8(a1,a2,a3,a4,a5,a6,a7,a8) typedef struct Arg {a1;a2;a3;a4;a5;a6;a7;a8;} *ArgPtr
#define FRX_OP_ARG_9(a1,a2,a3,a4,a5,a6,a7,a8,a9) typedef struct Arg {a1;a2;a3;a4;a5;a6;a7;a8;a9;} *ArgPtr
#define FRX_OP_ARG_10(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10) typedef struct Arg {a1;a2;a3;a4;a5;a6;a7;a8;a9;a10;} *ArgPtr



#define FRX_OP_PROCESS_IMPL                                                             \
    void processImpl(Session::Opc opc, void *argmem, void *retmem) {                    \
        try {                                                                           \
            Operations::OpcManager::process(opc, this, argmem, retmem);                 \
        } catch(const std::exception &ex) {                                             \
            SAMBAG_LOG_ERR<<ex.what()<<" on opc("<<opc<<") "<<__FILE__<<":"<<__LINE__;  \
            throw;                                                                      \
        }                                                                               \
    }
#define FRX_OP_BEGIN_OPERATIONS struct Operations {
#define FRX_OP_END_OPERATIONS(ops) typedef frx::processing::interprocess::helper::AutoOPC<ops> OpcManager; }; \
    typedef Operations::OpcManager OpcM;

#define FRX_OP_END_OPERATIONS_AND_IMPL_PROCESS(ops) FRX_OP_END_OPERATIONS(ops) FRX_OP_PROCESS_IMPL

#define FRX_OP_OPERATION(name, arg, ret) struct name {arg; ret;}


#define FRX_OP_CALLBACK_METHOD(opname) void auto_opc_callback(Operations::opname::ArgPtr arg, Operations::opname::RetPtr ret)
#define FRX_OP_CALLBACK_METHOD_IMPL(classname, opname) void classname::auto_opc_callback(Operations::opname::ArgPtr arg, Operations::opname::RetPtr ret)

namespace frx { namespace processing { namespace interprocess {
namespace helper {
template <class Caller, typename Operation>
struct Functor {
    typedef void (Caller::*F)(typename Operation::ArgPtr, typename Operation::RetPtr);
};
template <class Operation, class Caller>
void delegate( typename Functor<Caller, Operation>::F f,
                Caller *caller, void *arg,  void *ret)
{
    (caller->*f)(static_cast<typename Operation::ArgPtr>(arg),
                 static_cast<typename Operation::RetPtr>(ret));
}

namespace {
template <class T>
struct IsNullType {
    enum { Value = false };
};
template <>
struct IsNullType<Loki::NullType> {
    enum { Value = true };
};
template <int I, class Caller, class List>
struct find_delegate {
    static void _do(int opc, Caller *caller, void *args, void *rets) {
        if (opc != I) {
            return find_delegate<I-1, Caller, List>::_do(
                opc,
                caller,
                args,
                rets);
        }
        typedef typename Loki::TL::TypeAt<List, I>::Result Operation;
        BOOST_STATIC_ASSERT( !IsNullType<Operation>::Value );
        delegate<Operation, Caller>(&Caller::auto_opc_callback,
            caller,
            args,
            rets);
        
    };
};

template <class Caller, class List>
struct find_delegate<-1, Caller, List> {
    static void _do(int opc, Caller *caller, void *args, void *rets) {
        SAMBAG_THROW(
            sambag::com::exceptions::IllegalStateException,
            "unsupported opc"
        );
    };
};

template <int I, class List>
struct max_argmem {
    enum { HeadSize = sizeof(typename Loki::TL::TypeAt<List, I>::Result::Arg) };
    enum { PreSize = max_argmem<I-1, List>::Value };
    enum { Value = HeadSize > PreSize ? HeadSize : PreSize };
};
template <class List>
struct max_argmem<-1, List> {
    enum { Value = 1 }; // to avoid malloc problems min size is always 1
};

template <int I, class List>
struct max_retmem {
    enum { HeadSize = sizeof(typename Loki::TL::TypeAt<List, I>::Result::Ret) };
    enum { PreSize = max_retmem<I-1, List>::Value };
    enum { Value = HeadSize > PreSize ? HeadSize : PreSize };
};
template <class List>
struct max_retmem<-1, List> {
    enum { Value = 1 }; // to avoid malloc problems min size is always 1
};


} // namespace
/**
 * A Helperclass for Managing OPCs using a typelist.
 */
template <class OperationList>
struct AutoOPC {
    typedef OperationList OPs;
    enum { NumOps = Loki::TL::Length<OPs>::value };
    template <class T>
    static int getOPC() {
        return Loki::TL::IndexOf<OPs, T>::value;
    }
    /**
     * delegates call to related opc method.
     * Assumes that Caller impl:
     * void auto_opc_callback(args, rets);
     * @return flase if no related opc impl. were found
     */
    template <class Caller>
    static void process(int opc, Caller *caller, void *args, void *rets) {
        find_delegate<NumOps-1, Caller, OPs>::_do(opc,
            caller,
            args,
            rets);
    }
    enum { MaxArgmemSize = max_argmem<NumOps-1, OPs>::Value };
    enum { MaxRetmemSize = max_retmem<NumOps-1, OPs>::Value };
};

} // namespace
//=============================================================================
/** 
  * @class Session.
  * @brief Class for a bidirection interprocess communication.
  * Communictaion is provided by 2 channels A and B. Every channel has an an
  * argument and a return buffer and will be processed by a seperated
  * thread.
  * \par
  * Helper:
  * \par
  * Delegation concept:
  * \par
  * delegation between a operation definition and a executing function
  * \par
  * Operation concept:
  * \code{.cpp}
  *
  * // Operation definitions:
  * struct Operations {
  *     struct DoSome { 
  *         typedef struct Arg {} *ArgPtr;
  *         typedef struct Ret {} *RetPtr;
  *     };
  *     typedef LOKI_TYPELIST_1( DoSome ) OPs;
  *     typedef helper::AutoOPC<OPs> OpcManager;
  * };
  * 
  * // Impl. example:
  * struct ExClass {
  *   // delegate
  *   void processImpl(Opc opc, void *argmem, void *retmem) {
  *      try {
  *          // calls auto_opc_callback
  *          Operations::OpcManager::process(opc, this, argmem, retmem);
  *      } catch(const std::exception &ex) {
  *         SAMBAG_LOG_ERR<<ex.what()<<" opc("<<opc<<")";
  *         throw;
  *      }
  *    }
  *    // impl.:
  *    void auto_opc_callback(Operations::DoSome::ArgPtr, Operations::DoSome::RetPtr);
  * };
  *
  * // OR the convinient Way
  * FRX_OP_BEGIN_OPERATIONS
  *     FRX_OP_OPERATION(DoSome, FRX_OP_ARG_1(int value), FRX_OP_RET());
  *     typedef LOKI_TYPELIST_1( DoSome ) OPs;
  * FRX_OP_END_OPERATIONS(OPs)
  *
  * // impl.
  * struct ExClass {
  *     FRX_OP_PROCESS_IMPL
  *     FRX_OP_CALLBACK_METHOD(DoSome);
  * };
  * FRX_OP_CALLBACK_METHOD_IMPL(ExClass, DoSome) {
  *    // doSome using arg and ret
  * }
  * \endcode
  */
class Session {
//=============================================================================
public:
    //-------------------------------------------------------------------------
    SAMBAG_DERIVATED_EXCEPTION_CLASS(sambag::com::exceptions::IllegalStateException,
        TimeOut
    );
    //-------------------------------------------------------------------------
    SAMBAG_DERIVATED_EXCEPTION_CLASS(sambag::com::exceptions::IllegalStateException,
        Exception
    );
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<Session> Ptr;
    //-------------------------------------------------------------------------
    /**
     * @brief operation codes for communication,
     * @note all values <0 are reserved
     */
    typedef Integer Opc;
    //-------------------------------------------------------------------------
    typedef UInteger BffArgSize; //<< size of the channel argument buffer
    typedef UInteger BffRetSize; //<< size of the channel return buffer
    typedef boost::tuple<BffArgSize, BffRetSize> ChannelSize;
    //-------------------------------------------------------------------------
    enum { IDLE = -1 };
    //-------------------------------------------------------------------------
    enum Priority { Normal, High };
    //-------------------------------------------------------------------------
    static const Priority DEFAULT_PRIORITY = Normal;
    static const int FRX_PRIOR_HIGH_MICROSEC = 10;
    static const int FRX_PRIOR_NORMAL_MICROSEC = 50 * 1000;
    
private:
    //-------------------------------------------------------------------------
    /**
     * @return the number of microseconds which were slept
     */
    inline int sleep() const {
        SAMBAG_ASSERT(priority);
        if (*priority == (Integer)High) {
            boost::this_thread::sleep(boost::posix_time::microsec(
                FRX_PRIOR_HIGH_MICROSEC));
            return FRX_PRIOR_HIGH_MICROSEC;
        }
        boost::this_thread::sleep(boost::posix_time::microsec(
            FRX_PRIOR_NORMAL_MICROSEC));
        return FRX_PRIOR_NORMAL_MICROSEC;
    }
    //-------------------------------------------------------------------------
    typedef boost::shared_ptr<boost::thread> ThreadPtr;
    ThreadPtr processThread;
    //-------------------------------------------------------------------------
    /**
     * @brief processes requests in a seperate thread.
     */
    void process();
    //-------------------------------------------------------------------------
    struct IPChannel;
    mutable IPChannel *channelA, *channelB, *processChannel, *requestChannel;
    //-------------------------------------------------------------------------
    SharedMemoryObjectPtr shm;
    MappedRegionPtr mapped_region;
    //-------------------------------------------------------------------------
    std::string id;
    //-------------------------------------------------------------------------
    UInteger memorySize;
    //-------------------------------------------------------------------------
    void *memory_ptr;
    //-------------------------------------------------------------------------
    sambag::com::interprocess::PointerIterator pIt;
    //-------------------------------------------------------------------------
    typedef boost::interprocess::interprocess_upgradable_mutex Mutex;
    //-------------------------------------------------------------------------
    Integer *priority;
    //-------------------------------------------------------------------------
    Integer *num_references;
    //-------------------------------------------------------------------------
    void openBuffer();
    //-------------------------------------------------------------------------
    void destroyShm();
    //-------------------------------------------------------------------------
    Integer getNeededSize(ChannelSize a, ChannelSize b);
    //-------------------------------------------------------------------------
    void createBuffer(ChannelSize a, ChannelSize b);
    //-------------------------------------------------------------------------
    typedef boost::tuple<ChannelSize, ChannelSize> ChannelSizes;
    void assignMemory(sambag::com::interprocess::PointerIterator &pIt,
        boost::optional<ChannelSizes> channelSizes = boost::optional<ChannelSizes>());
    //-------------------------------------------------------------------------
    void startProcessThread();
    //-------------------------------------------------------------------------
    void * waitForResultImpl(Opc opc, Integer timeout) const;
protected:
    //-------------------------------------------------------------------------
    virtual void processImpl(Opc opc, void *argmen, void *retmem) = 0;
    //-------------------------------------------------------------------------
    /**
     * @brief puts opc into related channel and waits until request is processed.
     * \code{.cpp}
     * // example call
     * typedef Host::Operations::DoSome Op;
     * Op::ArgPtr args = static_cast<Op::ArgPtr>(getArgmem());
     * // copy arg values
     * args->arg1 = 0;
     * args->arg2 = 0;
     * Op::RetPtr rets rets = waitForResult<Op::RetPtr>(
     *   Host::OpcM::getOPC<Op>(),
     *   FRX_BRIDGE_CREATE_PL_SESSION_TIMEOUT
     * );
     * \endcode
     * @param opcode
     * @param time in millisec to wait, throws TimeOut after elapsed with no result.
     * @return retmem ptr
     */
    template <typename T>
    T waitForResult(Opc opc, Integer timeout=FRX_BRIDGE_CREATE_PL_SESSION_TIMEOUT) const
    {
        return static_cast<T>(waitForResultImpl(opc, timeout));
    }
    //-------------------------------------------------------------------------
    void waitForResult(Opc opc, Integer timeout=FRX_BRIDGE_CREATE_PL_SESSION_TIMEOUT) const
    {
        waitForResultImpl(opc, timeout);
    }
    //-------------------------------------------------------------------------
    /**
     * @return argmem for an request.
     */
    void * getArgmem() const;
    //-------------------------------------------------------------------------
    /**
     * @return retmem for an request.
     */
    void * getRetmem() const;
    //-------------------------------------------------------------------------
    /**
     * @return argmem for an request.
     */
    void * getArgmem();
    //-------------------------------------------------------------------------
    /**
     * @return retmem for an request.
     */
    void * getRetmem();

    //-------------------------------------------------------------------------
    /**
     * @note creates a session.
     * uses channel A for process and B for request
     * @param creates session with this id
     */
    Session(const std::string &id, ChannelSize a, ChannelSize b);
    //-------------------------------------------------------------------------
    /**
     * @note creates a session.
     * uses channel B for process and A for request
     * @param tries to open this id
     */
    Session(const std::string &id);
    //-------------------------------------------------------------------------
    /**
     * @param the session thread priority
     */
    void setPriority (Priority val);
    //-------------------------------------------------------------------------
    /**
     * @return the channel thread priority, or -1 when undefined
     */
     Integer getPriority() const {
        if (!priority) {
            return -1;
        }
        return (Integer)*priority;
     }
public:
    //-------------------------------------------------------------------------
    const std::string & getId() const {
        return id;
    }
    //-------------------------------------------------------------------------
    virtual ~Session();
    //-------------------------------------------------------------------------
    /**
     * @return argmem bytesize of processing channel 
     */
    size_t getProcessArgmemSize() const;
    //-------------------------------------------------------------------------
    /**
     * @return retmem bytesize of processing channel 
     */
    size_t getProcessRetmemSize() const;
    //-------------------------------------------------------------------------
    /**
     * @return argmem bytesize of request channel 
     */
    size_t getRequestArgmemSize() const;
    //-------------------------------------------------------------------------
    /**
     * @return retmem bytesize of request channel 
     */
    size_t getRequestRetmemSize() const;

}; // Session
}}} // namespace(s)

#endif /* SAMBAG_SESSION_H */

