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

namespace frx { namespace processing { namespace interprocess {
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
    void waitForResult(Opc opc, Integer timeout=1000) {
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
     * @throws IllegalStateException id is already occupied.
     */
    Session(const std::string &id);
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

