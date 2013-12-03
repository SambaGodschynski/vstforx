/*
 * SessionManager.hpp
 *
 *  Created on: Sat Sep 14 09:23:31 2013
 *      Author: Johannes Unger
 */
#include "SessionManager.hpp"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <sambag/com/exceptions/IllegalStateException.hpp>
#include <sambag/com/Common.hpp>
#include <math.h>
#include <sambag/com/SharedMemory.hpp> 
#include <sambag/com/SharedMemoryImpl.hpp> 

namespace {
    const int MAX_SESSIONS = 128; // TODO: fails with values > 150
    const int CH_MAX_CHAR = 100;
	//                        session_id              object_id         channel_name
    typedef boost::tuple<char[CH_MAX_CHAR], char[CH_MAX_CHAR], char[CH_MAX_CHAR]> RawSessionData;
    const int RC_MAX_MEM_SIZE = sizeof(RawSessionData)*MAX_SESSIONS + 6400;
    const int TOTMANN_UPDATE_INTERVAL_SEC = 3;
    /*const char * FRX_LAST_CHANGED_TIMESTAMP = "changedTimestamp";
    const char * FRX_MANAGER_MUTEX = "manager_mutex";
    const char * FRX_CHANNEL_DATA = "RawSessionData";
    const char * FRX_TOTMANN_TIME = "totmanntime";
    const char * FRX_NUM_REFERENCES = "references";*/
} //namespace(s)

namespace frx { namespace processing { namespace interprocess {
//=============================================================================
/**
 * Class SessionManager::Sessions
 * Only as workaround -> simple impl: find first free session
 */
struct SessionManager::Sessions {
    RawSessionData data[MAX_SESSIONS];
    UInteger _size;
    Sessions();
    void addChannel(const std::string &id, const SessionData &data);
    void removeSession(const std::string &id);
    static void _removeSession(RawSessionData *session);
    SessionData getSessionData(const std::string &name);
    size_t size() const { return _size; }
    void clear();
    int findNextFreeSession();
    int find(const std::string &id);
    static bool isEmpty(RawSessionData *channel)
    {
        return strcmp(&(*boost::get<0>(*channel)), "") == 0;
    }
    static std::string getId(RawSessionData *data)
    {
        return std::string(&(*boost::get<0>(*data)));
    }
    static std::string getObjectId(RawSessionData *data)
    {
        return std::string(&(*boost::get<1>(*data)));
    }
    static std::string getSessionName(RawSessionData *data)
    {
        return std::string(&(*boost::get<2>(*data)));
    }
    static bool isSession(const std::string &id, RawSessionData *data)
    {
        if (id.length()>=(size_t)CH_MAX_CHAR) {
            return false;
        }
        return strcmp(&(*boost::get<0>(*data)), id.c_str()) == 0;
    }
    static void setSession(RawSessionData *data, const std::string &id, const SessionData &rcd);
    template <class Container>
    void getSessions(Container &out);
};
//=============================================================================
//-----------------------------------------------------------------------------
SessionManager::Sessions::Sessions()
{
    clear();
}
//-----------------------------------------------------------------------------
void SessionManager::Sessions::
addChannel(const std::string &id, const SessionData &sd)
{
    int i = findNextFreeSession();
    if (i<0) {
        SAMBAG_THROW(sambag::com::exceptions::IllegalStateException,
        "SessionManager: no sessions left");
    }
    setSession(&data[i], id, sd);
    ++_size;
}
//-----------------------------------------------------------------------------
void SessionManager::Sessions::
setSession(RawSessionData *data, const std::string &id, const SessionData &sd)
{
    const std::string &objId = boost::get<0>(sd);
	const std::string &name = boost::get<1>(sd);
    if (id.length() >= (int)CH_MAX_CHAR ||
		objId.length() >= (int)CH_MAX_CHAR ||
		name.length() >= (int)CH_MAX_CHAR) 
	{
        SAMBAG_THROW(sambag::com::exceptions::IllegalStateException,
        "SessionManager: setSession out of bounds");
    }
    strcpy(&(*boost::get<0>(*data)), id.c_str());
    strcpy(&(*boost::get<1>(*data)), objId.c_str());
	strcpy(&(*boost::get<2>(*data)), name.c_str());
}
//-----------------------------------------------------------------------------
void SessionManager::Sessions::
removeSession(const std::string &id) {
    int i = find(id);
    if (i<0) {
        return;
    }
    _removeSession(&data[i]);
    --_size;
}
//-----------------------------------------------------------------------------
void SessionManager::Sessions::
_removeSession(RawSessionData *channel)
{
    strcpy(&(*boost::get<0>(*channel)), "");
}
//-----------------------------------------------------------------------------
SessionManager::SessionData SessionManager::Sessions::
getSessionData(const std::string &id) {
    int i = find(id);
    if (i<0) {
        return SessionData();
    }
    return SessionData( getObjectId(&data[i]), getSessionName(&data[i]) );
}
//-----------------------------------------------------------------------------
void SessionManager::Sessions::clear() {
    for (int i=0; i<MAX_SESSIONS; ++i) {
        _removeSession(&data[i]);
    }
    _size = 0;
}
//-----------------------------------------------------------------------------
int SessionManager::Sessions::
findNextFreeSession()
{
    for (int i=0; i<MAX_SESSIONS; ++i) {
        if (isEmpty(&data[i])) {
            return i;
        }
    }
    return -1;
}
//-----------------------------------------------------------------------------
int SessionManager::Sessions::
find(const std::string &id)
{
    for (int i=0; i<MAX_SESSIONS; ++i) {
        if (isSession(id, &data[i])) {
            return i;
        }
    }
    return -1;
}
//-----------------------------------------------------------------------------
template <class Container>
void SessionManager::Sessions::
getSessions(Container &out)
{
    for (int i=0; i<MAX_SESSIONS; ++i) {
        if (!isEmpty(&data[i])) {
            out.push_back(getId(&data[i]));
        }
    }
}
//=============================================================================
//  Class SessionManager
//=============================================================================
//-----------------------------------------------------------------------------
SessionManager::SessionManager() :
    changed(NULL),
    totmann_time(NULL),
    references(NULL),
    mutex(NULL),
    sessions(NULL)
{
}
//-----------------------------------------------------------------------------
bool SessionManager::isTotmann() {
    Integer diff = ::abs( (Integer)::time(NULL) - (Integer)*totmann_time );
    if ( *references > 0 &&
         diff  > (TOTMANN_UPDATE_INTERVAL_SEC+1))
    {
        return true;
    }
    return false;
}
//-----------------------------------------------------------------------------
void SessionManager::getSessions(std::vector<SessionId> &out) {
    sessions->getSessions(out);
}
//-----------------------------------------------------------------------------
void SessionManager::getSessions(std::list<SessionId> &out) {
    sessions->getSessions(out);
}
//-----------------------------------------------------------------------------
void SessionManager::initManager(const std::string shmName, int tries) {
    
    this->shmName = shmName;
    
    SAMBAG_LOG_INFO<<"init SessionManager: ...";
    using namespace boost::interprocess;
    using namespace ::sambag::com::interprocess;
    shm = SharedMemoryObjectPtr(
        new SAMBAG_SHARED_MEMORY_OBJECT_CREATE(open_or_create, shmName.c_str(), read_write, RC_MAX_MEM_SIZE)
    );

    SAMBAG_SHARED_MEMORY_TRUNC(*(shm.get()),RC_MAX_MEM_SIZE);
    mapped_region = MappedRegionPtr(
        new MappedRegion(*(shm.get()), read_write)
    );
    void *ptr = mapped_region->get_address();
    
    PointerIterator pIt(ptr, RC_MAX_MEM_SIZE);
    typedef PlacementAlloc<Integer> Allocator;
    Allocator alloc(pIt);
    
    changed = Allocator::rebind<UInteger>::other(alloc).allocate(1);
    mutex = Allocator::rebind<Mutex>::other(alloc).allocate(1);
    sessions = Allocator::rebind<Sessions>::other(alloc).allocate(1);
    totmann_time = Allocator::rebind<UInteger>::other(alloc).allocate(1);
    references = Allocator::rebind<UInteger>::other(alloc).allocate(1);
    
    if ((*references)==0) {
        new (mutex) Mutex();
        new (sessions) Sessions();
        *totmann_time = ::time(NULL);
    }
    
    // dead man found, clear up
    if (isTotmann()) {
        if (tries>2) {
            SAMBAG_LOG_ERR<<"init SessionManager: FAILED";
            SAMBAG_THROW(
                sambag::com::exceptions::IllegalStateException,
                "SessionManager::initManager failed."
            );
        }
        SAMBAG_LOG_INFO<<"  + cleanup remote memory";
        destroyShm();
        initManager(shmName, tries+1);
        return;
    }
    bi::scoped_lock<Mutex> lock(*mutex);
    ++(*references);
    
    //init totmann timer
    if (!totmannTimer) {
        trackingDummy = TrackingDummyPtr(new Dummy);
        totmannTimer = TotmannTimer::create(TOTMANN_UPDATE_INTERVAL_SEC*1000);
        totmannTimer->setNumRepetitions(-1);
        totmannTimer->addTrackedEventListener(
            boost::bind(&SessionManager::doTotmann, this),
            trackingDummy
        );
        totmannTimer->start();
    }
    SAMBAG_LOG_INFO<<"init SessionManager: SUCCEED";
}
//-----------------------------------------------------------------------------
void SessionManager::doTotmann() {
    if (!mutex || !totmann_time) {
		return;
	}
    bi::scoped_lock<Mutex> lock(*mutex);
    *totmann_time = ::time(NULL);
}
//-----------------------------------------------------------------------------
void SessionManager::destroyShm() {
    using namespace boost::interprocess;
    try {
        SAMBAG_LOG_INFO<<"destroying SessionManager: ...";
        changed = NULL;
        mutex = NULL;
        sessions = NULL;
        totmann_time = NULL;
        references = NULL;
        
        mapped_region.reset();
        shm.reset();
        SAMBAG_SHARED_MEMORY_REMOVE(shmName.c_str());
        SAMBAG_LOG_INFO<<"destroying SessionManager: SUCCEED";
    } catch (...) {
        SAMBAG_LOG_ERR<<"destroying SessionManager failed.";
    }
}
//-----------------------------------------------------------------------------
SessionManager::~SessionManager() {
	if (references && --(*references)==0) {
		destroyShm();
	}
}
//-----------------------------------------------------------------------------
UInteger SessionManager::getNumSessions() const {
    return sessions->size();
}
//-----------------------------------------------------------------------------
std::string SessionManager::getObjectId(const SessionData &data) const
{
    return boost::get<0>(data);
}
//-----------------------------------------------------------------------------
void SessionManager::addSession(const SessionId &id, const SessionData &data)
{

    bi::scoped_lock<Mutex> lock(*mutex);
    sessions->addChannel(id, data);
    *changed = ::time(NULL);
}
//-----------------------------------------------------------------------------
SessionManager::SessionId SessionManager::addSession(const SessionData &data)
{
    SessionId id = createUniqueName();
    addSession(id, data);
    return id;
}
//-----------------------------------------------------------------------------
void SessionManager::removeSession(const SessionId &id) {

    bi::scoped_lock<Mutex> lock(*mutex);
    sessions->removeSession(id);
    *changed = ::time(NULL);
    
}
//-----------------------------------------------------------------------------
std::string SessionManager::createUniqueName() const {
    using namespace boost::uuids;
    uuid uuid = random_generator()();
    return boost::uuids::to_string(uuid);
}
//-----------------------------------------------------------------------------
std::string SessionManager::getSessionName(const SessionId &id) {
	SessionData data = getSessionData(id);
	return boost::get<1>(data);
}
//-----------------------------------------------------------------------------
SessionManager::SessionData
SessionManager::getSessionData(const SessionId &id) const {
    bi::scoped_lock<Mutex> lock(*mutex);
    return sessions->getSessionData(id);
}
//-----------------------------------------------------------------------------
void SessionManager::removeAllSessions() {
    bi::scoped_lock<Mutex> lock(*mutex);
    sessions->clear();
}

}}} // namespace(s)