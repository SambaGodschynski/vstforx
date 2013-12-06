/*
 * SessionManager.hpp
 *
 *  Created on: Sat Sep 14 09:23:31 2013
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_SESSIONMANAGER_H
#define SAMBAG_SESSIONMANAGER_H

#include "Stream.hpp"
#include <time.h>
#include <sambag/com/PlacementAlloc.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/foreach.hpp>
#include <processing/FrxAsyncDSPTimer.hpp>
#include <sambag/com/BoostTimer2.hpp>
#include <sambag/com/SharedMemory.hpp> 

namespace frx { namespace processing { namespace interprocess {
namespace bi = boost::interprocess;
namespace si = sambag::com::interprocess;
using ::sambag::com::interprocess::Integer;
using ::sambag::com::interprocess::UInteger;
//=============================================================================
/** 
  * @class SessionManager
  * Baseclass for all managers which has to manage interprocess sessions. 
  * A Session has + a sessionid
  *               + an id for the specific session object 
  *               + and a name.
  * @note since of interarchitecutre problems in the boost interprocess library
  * we can't use managed_shared_memory, 
  * so we are forced to impl. a more raw c style approach.
  */
class SessionManager {
//=============================================================================
public:
    //-------------------------------------------------------------------------
    typedef std::string SessionId;
    //-------------------------------------------------------------------------
    /**
     * tuple(streamid, channelname)
     */
	typedef boost::tuple<std::string, std::string> SessionData;
private:
    //-------------------------------------------------------------------------
    struct Sessions;
    //-------------------------------------------------------------------------
    UInteger *changed;
    //-------------------------------------------------------------------------
    UInteger *totmann_time;
    //-------------------------------------------------------------------------
    UInteger *references;
    //-------------------------------------------------------------------------
    std::string shmName;
    //-------------------------------------------------------------------------
    typedef boost::interprocess::interprocess_mutex Mutex;
    //-------------------------------------------------------------------------
    Mutex *mutex;
    //-------------------------------------------------------------------------
    Sessions *sessions;
    //-------------------------------------------------------------------------
  	typedef boost::shared_ptr<si::SharedMemoryObject> SharedMemoryObjectPtr;
    SharedMemoryObjectPtr shm;
	typedef boost::shared_ptr<si::MappedRegion> MappedRegionPtr;
    MappedRegionPtr mapped_region;
    //-------------------------------------------------------------------------
    typedef sambag::com::BoostTimer2 TotmannTimer;
    TotmannTimer::Ptr totmannTimer;
    //-------------------------------------------------------------------------
    struct Dummy {};
    typedef boost::shared_ptr<Dummy> TrackingDummyPtr;
    TrackingDummyPtr trackingDummy;
protected:
    //-------------------------------------------------------------------------
    SessionManager();
    //-------------------------------------------------------------------------
    void doTotmann();
    //-------------------------------------------------------------------------
    bool isTotmann();
    //-------------------------------------------------------------------------
    /**
     * @param shmName the shared memory namespace
     * @param used when called recursive hat to be 0 for client
     */
    void initManager(const std::string shmName, int tries=0);
    //-------------------------------------------------------------------------
    void destroyShm();
public:
    //-------------------------------------------------------------------------
    /**
     * @return the number of running SessionManager instances.
     */
    UInteger getNumReferences() const {
        return *references;
    }
	//-------------------------------------------------------------------------
	virtual ~SessionManager();
    //-------------------------------------------------------------------------
    /**
     * task: determine when something was changed.
     * Since the traditional event approach is a bit to expensive (we have
     * to check frequently whether something was changed in the interprocess
     * memory) we use a "inter process" timestamp.
     * Every instance which is interrested, has to check this value.
     *
     * @return the recent timepoint when RemoteChannel was added or removed.
     */
    UInteger getLastChangedTime() const {
        if (!changed) {
            return 0;
        }
        return *changed;
    }
    //-------------------------------------------------------------------------
    // Receiver stuff
    //-------------------------------------------------------------------------
    UInteger getNumSessions() const;
    //-------------------------------------------------------------------------
    void getSessions(std::vector<SessionId> &out);
    void getSessions(std::list<SessionId> &out);
    //-------------------------------------------------------------------------
    SessionData getSessionData(const SessionId &id) const;
    //-------------------------------------------------------------------------
    // Sender stuff
    //-------------------------------------------------------------------------
    void addSession(const SessionId &id, const SessionData &data);
    //-------------------------------------------------------------------------
    SessionId addSession(const SessionData &data);
    //-------------------------------------------------------------------------
    void removeSession(const SessionId &id);
    //-------------------------------------------------------------------------
    static std::string createUniqueName();
    //-------------------------------------------------------------------------
    void removeAllSessions();
    //-------------------------------------------------------------------------
    std::string getSessionName(const SessionId &id);
   //-------------------------------------------------------------------------
    std::string getObjectId(const SessionData &data) const;
}; // SessionManager
}}} // namespace(s)

#endif /* SAMBAG_SessionManager_H */