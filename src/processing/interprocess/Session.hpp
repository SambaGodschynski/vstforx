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


namespace frx { namespace processing { namespace interprocess {
//=============================================================================
/** 
  * @class Session.
  * Class for a bidirection interprocess communication.
  * Communictaion is provided by 2 channels. Every channel has an an
  * argument and a return buffer.
  */
class Session {
//=============================================================================
public:
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
    enum { PROCEEDED = -2, IDLE =-1 };
private:
    //-------------------------------------------------------------------------
    /**
     * @brief processes requests in a seperate thread.
     */
    void process();
    //-------------------------------------------------------------------------
    struct IPChannel;
    IPChannel *channelA, *channelB, *processChannel, *requestChannel;
    //-------------------------------------------------------------------------
    IPChannel * getChannel() const;
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
    Mutex *mutex;
    //-------------------------------------------------------------------------
    Integer *num_references;
    //-------------------------------------------------------------------------
    void openBuffer();
    //-------------------------------------------------------------------------
    void destroyBuffer();
    //-------------------------------------------------------------------------
    Integer getNeededSize(ChannelSize a, ChannelSize b);
    //-------------------------------------------------------------------------
    void createBuffer(ChannelSize a, ChannelSize b);
    //-------------------------------------------------------------------------
    typedef boost::tuple<ChannelSize, ChannelSize> ChannelSizes;
    void assignMemory(sambag::com::interprocess::PointerIterator &pIt,
        boost::optional<ChannelSizes> channelSizes = boost::optional<ChannelSizes>());
protected:
    //-------------------------------------------------------------------------
    virtual Opc processImpl(Opc opc, void *argmen, void *retmem) = 0;
    //-------------------------------------------------------------------------
    /**
     * @brief puts opc into related channel and waits until request is processed.
     */
    void waitForResult(Opc opc);
    //-------------------------------------------------------------------------
    void * getArgmem() const;
    //-------------------------------------------------------------------------
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
public:
    //-------------------------------------------------------------------------
    virtual ~Session();
}; // Session
}}} // namespace(s)

#endif /* SAMBAG_SESSION_H */

