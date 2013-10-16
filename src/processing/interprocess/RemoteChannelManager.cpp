/*
 * RemoteChannelManager.cpp
 *
 *  Created on: Sat Sep 14 09:23:31 2013
 *      Author: Johannes Unger
 */
#include "RemoteChannelManager.hpp"
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
    const int MAX_CHANNELS = 128;
    const int CH_MAX_CHAR = 100;
    typedef boost::tuple<char[CH_MAX_CHAR], char[CH_MAX_CHAR]> ChannelData;
    const int RC_MAX_MEM_SIZE = sizeof(ChannelData)*MAX_CHANNELS + 6400;
    const int TOTMANN_UPDATE_INTERVAL_SEC = 3;
    const char * SHM_MANAGER_NAME = "VSTForx.RemoteChannelManager";
    /*const char * FRX_LAST_CHANGED_TIMESTAMP = "changedTimestamp";
    const char * FRX_MANAGER_MUTEX = "manager_mutex";
    const char * FRX_CHANNEL_DATA = "channelData";
    const char * FRX_TOTMANN_TIME = "totmanntime";
    const char * FRX_NUM_REFERENCES = "references";*/
} //namespace(s)

namespace frx { namespace processing { namespace interprocess {
typedef Loki::SingletonHolder<RemoteChannelManager> RemoteChannelManagerHolder;
//=============================================================================
//  Class RemoteChannelManager::RemoteChannels
//  Only as workaround -> simple impl.
struct RemoteChannelManager::RemoteChannels {
    ChannelData data[MAX_CHANNELS];
    UInteger _size;
    RemoteChannels();
    void addChannel(const std::string &id, const RCData &data);
    void removeChannel(const std::string &id);
    static void _removeChannel(ChannelData *channel);
    RCData getChannelData(const std::string &name);
    size_t size() const { return _size; }
    void clear();
    int findNextFreeChannel();
    int find(const std::string &id);
    static bool isEmpty(ChannelData *channel) {
        return strcmp(&(*boost::get<0>(*channel)), "") == 0;
    }
    static std::string getId(ChannelData *data) {
        return std::string(&(*boost::get<0>(*data)));
    }
    static std::string getChannelId(ChannelData *data) {
        return std::string(&(*boost::get<1>(*data)));
    }
    static bool isChannel(const std::string &id, ChannelData *data) {
        if (id.length()>=(size_t)CH_MAX_CHAR) {
            return false;
        }
        return strcmp(&(*boost::get<0>(*data)), id.c_str()) == 0;
    }
    static void setChannel(ChannelData *data, const std::string &id, const RCData &rcd);
    template <class Container>
    void getChannels(Container &out);
};
//=============================================================================
//-----------------------------------------------------------------------------
RemoteChannelManager::RemoteChannels::RemoteChannels()
{
    clear();
}
//-----------------------------------------------------------------------------
void RemoteChannelManager::RemoteChannels::
addChannel(const std::string &id, const RCData &rcd)
{
    int i = findNextFreeChannel();
    if (i<0) {
        SAMBAG_THROW(sambag::com::exceptions::IllegalStateException,
        "RemoteChannelManager: no channels left");
    }
    setChannel(&data[i], id, rcd);
    ++_size;
}
//-----------------------------------------------------------------------------
void RemoteChannelManager::RemoteChannels::
setChannel(ChannelData *data, const std::string &id, const RCData &rcd)
{
    const std::string &chname = boost::get<0>(rcd);
    if (id.length() >= (int)CH_MAX_CHAR || chname.length() >= (int)CH_MAX_CHAR) {
        SAMBAG_THROW(sambag::com::exceptions::IllegalStateException,
        "RemoteChannelManager: setChannel out of bounds");
    }
    strcpy(&(*boost::get<0>(*data)), id.c_str());
    strcpy(&(*boost::get<1>(*data)), chname.c_str());
}
//-----------------------------------------------------------------------------
void RemoteChannelManager::RemoteChannels::
removeChannel(const std::string &id) {
    int i = find(id);
    if (i<0) {
        return;
    }
    _removeChannel(&data[i]);
    --_size;
}
//-----------------------------------------------------------------------------
void RemoteChannelManager::RemoteChannels::
_removeChannel(ChannelData *channel)
{
    strcpy(&(*boost::get<0>(*channel)), "");
}
//-----------------------------------------------------------------------------
RemoteChannelManager::RCData RemoteChannelManager::RemoteChannels::
getChannelData(const std::string &id) {
    int i = find(id);
    if (i<0) {
        return RCData();
    }
    return RCData( getChannelId(&data[i]) );
}
//-----------------------------------------------------------------------------
void RemoteChannelManager::RemoteChannels::clear() {
    for (int i=0; i<MAX_CHANNELS; ++i) {
        _removeChannel(&data[i]);
    }
    _size = 0;
}
//-----------------------------------------------------------------------------
int RemoteChannelManager::RemoteChannels::
findNextFreeChannel()
{
    for (int i=0; i<MAX_CHANNELS; ++i) {
        if (isEmpty(&data[i])) {
            return i;
        }
    }
    return -1;
}
//-----------------------------------------------------------------------------
int RemoteChannelManager::RemoteChannels::
find(const std::string &id)
{
    for (int i=0; i<MAX_CHANNELS; ++i) {
        if (isChannel(id, &data[i])) {
            return i;
        }
    }
    return -1;
}
//-----------------------------------------------------------------------------
template <class Container>
void RemoteChannelManager::RemoteChannels::
getChannels(Container &out)
{
    for (int i=0; i<MAX_CHANNELS; ++i) {
        if (!isEmpty(&data[i])) {
            out.push_back(getId(&data[i]));
        }
    }
}
//=============================================================================
//  Class RemoteChannelManager
//=============================================================================
//-----------------------------------------------------------------------------
RemoteChannelManager::RemoteChannelManager() :
    changed(NULL),
    totmann_time(NULL),
    references(NULL),
    mutex(NULL),
    channels(NULL)
{
   
    //shmh = SHMHPtr(new SHMH(SHM_MANAGER_NAME, RC_MAX_MEM_SIZE));
    initManager();
}
//-----------------------------------------------------------------------------
bool RemoteChannelManager::isTotmann() {
    Integer diff = ::abs( (Integer)::time(NULL) - (Integer)*totmann_time );
    if ( *references > 0 &&
         diff  > (TOTMANN_UPDATE_INTERVAL_SEC+1))
    {
        return true;
    }
    return false;
}
//-----------------------------------------------------------------------------
void RemoteChannelManager::getChannels(std::vector<RCId> &out) {
    channels->getChannels(out);
}
//-----------------------------------------------------------------------------
void RemoteChannelManager::getChannels(std::list<RCId> &out) {
    channels->getChannels(out);
}
//-----------------------------------------------------------------------------
void RemoteChannelManager::initManager(int tries) {
    SAMBAG_LOG_INFO<<"init RemoteChannelManager: ...";
    /*changed = shmh->get().find_or_construct<UInteger>(FRX_LAST_CHANGED_TIMESTAMP)();
    mutex = shmh->get().find_or_construct<Mutex>(FRX_MANAGER_MUTEX)();
    channels = RemoteChannels::findOrCreate(FRX_CHANNEL_DATA, shmh->get());
    totmann_time = shmh->get().find_or_construct<UInteger>(FRX_TOTMANN_TIME)(::time(NULL));
    references = shmh->get().find_or_construct<UInteger>(FRX_NUM_REFERENCES)();*/
    
    using namespace boost::interprocess;
    using namespace ::sambag::com::interprocess;
    shm = SharedMemoryObjectPtr(
        new SAMBAG_SHARED_MEMORY_OBJECT_CREATE(open_or_create, SHM_MANAGER_NAME, read_write, RC_MAX_MEM_SIZE)
    );

    SAMBAG_SHARED_MEMORY_TRUNC(shm,RC_MAX_MEM_SIZE);
    mapped_region = MappedRegionPtr(
        new MappedRegion(*(shm.get()), read_write)
    );
    void *ptr = mapped_region->get_address();
    
    PointerIterator pIt(ptr, RC_MAX_MEM_SIZE);
    typedef PlacementAlloc<Integer> Allocator;
    Allocator alloc(pIt);
    
    changed = Allocator::rebind<UInteger>::other(alloc).allocate(1);
    mutex = Allocator::rebind<Mutex>::other(alloc).allocate(1);
    channels = Allocator::rebind<RemoteChannels>::other(alloc).allocate(1);
    totmann_time = Allocator::rebind<UInteger>::other(alloc).allocate(1);
    references = Allocator::rebind<UInteger>::other(alloc).allocate(1);
    
    if ((*references)==0) {
        new (mutex) Mutex();
        new (channels) RemoteChannels();
        *totmann_time = ::time(NULL);
    }
    
    // dead man found, clear up
    if (isTotmann()) {
        if (tries>2) {
            SAMBAG_LOG_ERR<<"init RemoteChannelManager: FAILED";
            SAMBAG_THROW(
                sambag::com::exceptions::IllegalStateException,
                "RemoteChannelManager::initManager failed."
            );
        }
        SAMBAG_LOG_INFO<<"  + cleanup remote memory";
        destroyShm();
        initManager(tries+1);
        return;
    }
    bi::scoped_lock<Mutex> lock(*mutex);
    ++(*references);
    
    //init totmann timer
    if (!totmannTimer) {
        trackingDummy = TrackingDummyPtr(new Dummy);
        totmannTimer = FrxAsyncDSPTimer::create(TOTMANN_UPDATE_INTERVAL_SEC*1000);
        totmannTimer->setNumRepetitions(-1);
        totmannTimer->addTrackedEventListener(
            boost::bind(&RemoteChannelManager::doTotmann, this),
            trackingDummy
        );
        totmannTimer->start();
    }
    SAMBAG_LOG_INFO<<"init RemoteChannelManager: SUCCEED";
}
//-----------------------------------------------------------------------------
void RemoteChannelManager::doTotmann() {
    bi::scoped_lock<Mutex> lock(*mutex);
    *totmann_time = ::time(NULL);
}
//-----------------------------------------------------------------------------
void RemoteChannelManager::destroyShm() {
    using namespace boost::interprocess;
    try {
        SAMBAG_LOG_INFO<<"destroying RemoteChannelManager: ...";
        /*shmh->get().destroy<UInteger>(FRX_LAST_CHANGED_TIMESTAMP);
        shmh->get().destroy<Mutex>(FRX_MANAGER_MUTEX);
        //shmh->get().destroy<RemoteChannels::Class>(FRX_CHANNEL_DATA);
        shmh->get().destroy<RemoteChannels>(FRX_CHANNEL_DATA);
        shmh->get().destroy<UInteger>(FRX_TOTMANN_TIME);
        shmh->get().destroy<UInteger>(FRX_NUM_REFERENCES);*/
        
        changed = NULL;
        mutex = NULL;
        channels = NULL;
        totmann_time = NULL;
        references = NULL;
        
        mapped_region.reset();
        shm.reset();
        SAMBAG_SHARED_MEMORY_REMOVE(SHM_MANAGER_NAME);
        SAMBAG_LOG_INFO<<"destroying RemoteChannelManager: SUCCEED";
    } catch (...) {
        SAMBAG_LOG_ERR<<"destroying RemoteChannelManager failed.";
    }
}
//-----------------------------------------------------------------------------
RemoteChannelManager::~RemoteChannelManager() {
    if (references && --(*references)<=0) {
        destroyShm();
    }
}
//-----------------------------------------------------------------------------
RemoteChannelManager & RemoteChannelManager::instance() {
	RemoteChannelManager &res = RemoteChannelManagerHolder::Instance();
    return res;
}
//-----------------------------------------------------------------------------
UInteger RemoteChannelManager::getNumChannels() const {
    return channels->size();
}
//-----------------------------------------------------------------------------
bool RemoteChannelManager::isStreamValid(const std::string &streamId) const {
    return streamId.length() > 0;
}
//-----------------------------------------------------------------------------
std::string RemoteChannelManager::getStreamId(const RCData &data) const
{
    return boost::get<0>(data);
}
//-----------------------------------------------------------------------------
Stream::Ptr RemoteChannelManager::getStream(const RCId &rc) {
    RCData data = getChannelData(rc);
    const std::string &streamId = getStreamId(data);
    if (!isStreamValid(streamId)) {
        return Stream::Ptr();
    }
    return Stream::open(streamId);
}
//-----------------------------------------------------------------------------
void RemoteChannelManager::addChannel(const RCId &id, const RCData &data)
{

    bi::scoped_lock<Mutex> lock(*mutex);
    /*si::String::TheAllocator str_alloc( shmh->get().get_segment_manager() );
    channels->insert( std::make_pair(
        SHM_RCId(id.c_str(), str_alloc),
        boost::make_tuple(
            SHM_String(boost::get<0>(data).c_str(), str_alloc)
        )
    ));*/
    channels->addChannel(id, data);
    *changed = ::time(NULL);
}
//-----------------------------------------------------------------------------
RemoteChannelManager::RCId RemoteChannelManager::addChannel(const RCData &data)
{
    RCId id = createUniqueName();
    addChannel(id, data);
    return id;
}
//-----------------------------------------------------------------------------
void RemoteChannelManager::removeChannel(const RCId &id) {

    bi::scoped_lock<Mutex> lock(*mutex);

    /*si::String::TheAllocator str_alloc( shmh->get().get_segment_manager() );
    channels->erase(SHM_String(id.c_str(), str_alloc));*/
    channels->removeChannel(id);
    *changed = ::time(NULL);
    
}
//-----------------------------------------------------------------------------
std::string RemoteChannelManager::createUniqueName() const {
    using namespace boost::uuids;
    uuid uuid = random_generator()();
    return boost::uuids::to_string(uuid);
}
//-----------------------------------------------------------------------------
std::string RemoteChannelManager::getName(const RCId &id) {
    // TODO: do some beauty things here
    return id;
}
//-----------------------------------------------------------------------------
RemoteChannelManager::RCData
RemoteChannelManager::getChannelData(const RCId &id) const {
    bi::scoped_lock<Mutex> lock(*mutex);
    /*si::String::TheAllocator str_alloc( shmh->get().get_segment_manager() );
    RemoteChannels::Class::const_iterator it = channels->find(SHM_String(id.c_str(), str_alloc));
    if (it==channels->end()) {
        return RCData();
    }
    return boost::make_tuple(
        boost::get<0>(it->second).c_str()
    );*/
    return channels->getChannelData(id);
}
//-----------------------------------------------------------------------------
void RemoteChannelManager::removeAllChannels() {
    bi::scoped_lock<Mutex> lock(*mutex);
    channels->clear();
}

}}} // namespace(s)