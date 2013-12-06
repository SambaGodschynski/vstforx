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


namespace frx { namespace processing { namespace interprocess {
namespace helper {

/**
 * @brief delegation between a operation definition and a executing function 
 * Operation concept:
 * struct Operation {
 *     typedef struct Arg {} *ArgPtr;
 *     typedef struct Ret {} *RetPtr;
 * };
 *
 * example:
 * struct ExClass {
 *   void processImpl(void *arg, void *ret) {
 *       delegate<Operation>(&ExClass::doOperation, this, arg, ret);
 *   }
 *   doOperation(Operation::ArgPtr, Operation::RetPtr);
 * };
 */
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
    static bool _do(int opc, Caller *caller, void *args, void *rets) {
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
        
        return true;
    };
};

template <class Caller, class List>
struct find_delegate<-1, Caller, List> {
    static bool _do(int opc, Caller *caller, void *args, void *rets) {
        return false;
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
    static bool process(int opc, Caller *caller, void *args, void *rets) {
        return find_delegate<NumOps-1, Caller, OPs>::_do(opc,
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
  * Class for a bidirection interprocess communication.
  * Communictaion is provided by 2 channels A and B. Every channel has an an
  * argument and a return buffer and will be processed by a seperated
  * thread.
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
    static const int DEFAULT_SLEEPING_TIME = 100;
private:
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
    IPChannel *channelA, *channelB, *processChannel, *requestChannel;
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
    Integer *sleepingTime;
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
    void * waitForResultImpl(Opc opc, Integer timeout);
protected:
    //-------------------------------------------------------------------------
    virtual void processImpl(Opc opc, void *argmen, void *retmem) = 0;
    //-------------------------------------------------------------------------
    /**
     * @brief puts opc into related channel and waits until request is processed.
     * @param opcode
     * @param time in millisec to wait, throws TimeOut after elapsed with no result.
     * @return retmem ptr
     */
    template <typename T>
    T waitForResult(Opc opc, Integer timeout=1000) {
        return static_cast<T>(waitForResultImpl(opc, timeout));
    }
    //-------------------------------------------------------------------------
    void waitForResult(Opc opc, Integer timeout=5000) {
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
    const std::string & getId() const {
        return id;
    }
    //-------------------------------------------------------------------------
    /**
     * @param the max. sleeping time while waiting for result in millisec
     */
    void setMaxSleeping (Integer ms);
    //-------------------------------------------------------------------------
    /**
     * @return the max. sleeping time for channel threads
     */
     Integer getMaxSleeping() const {
        if (!sleepingTime) {
            return 0;
        }
        return *sleepingTime;
     }
public:
    //-------------------------------------------------------------------------
    virtual ~Session();
}; // Session
}}} // namespace(s)

#endif /* SAMBAG_SESSION_H */

