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

namespace {
    const int RC_MAX_MEM_SIZE = 64000;
    const int TOTMANN_UPDATE_INTERVAL_SEC = 3;
    const char * SHM_MANAGER_NAME = "VSTForx.RemoteChannelManager6";
} //namespace(s)

namespace frx { namespace processing { namespace interprocess {
typedef Loki::SingletonHolder<RemoteChannelManager> RemoteChannelManagerHolder;
//=============================================================================
//  Class RemoteChannelManager
//=============================================================================
//-----------------------------------------------------------------------------
RemoteChannelManager::RemoteChannelManager() :
    changed(NULL),
    mutex(NULL),
    channels(NULL),
    shmh(NULL)
{
    initManager();
}
//-----------------------------------------------------------------------------
bool RemoteChannelManager::isTotmann() {
    // XXX
    return false;
    
    
    
    
    int diff = ::abs( ::time(NULL) - *totmann_time );
    if ( *references > 0 &&
         diff  > (TOTMANN_UPDATE_INTERVAL_SEC+1))
    {
        return true;
    }
    return false;
}
//-----------------------------------------------------------------------------
void RemoteChannelManager::initManager(int tries) {
    shmh = new sambag::com::interprocess::SharedMemoryHolder(SHM_MANAGER_NAME, RC_MAX_MEM_SIZE);
    changed = shmh->get().find_or_construct<UInteger>("changedTimestamp")();
    mutex = shmh->get().find_or_construct<Mutex>("mutex")();
    channels = RemoteChannels::findOrCreate("channelData", shmh->get());
    totmann_time = shmh->get().find_or_construct<UInteger>("totmanntime")(::time(NULL));
    references = shmh->get().find_or_construct<UInteger>("references")();
    
    // dead man found, clear up
    if (isTotmann()) {
        if (tries>100) {
            SAMBAG_THROW(
                sambag::com::exceptions::IllegalStateException,
                "RemoteChannelManager::initManager failed."
            );
        }
        SAMBAG_LOG_INFO<<"cleanup remote memory";
        //XXX destroyShm();
        //shmh->initMemory(SHM_MANAGER_NAME, RC_MAX_MEM_SIZE);
        //totmann_time = shmh->get().find_or_construct<UInteger>("totmanntime")();
        //*totmann_time = ::time(NULL);
        //initManager(tries+1);
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
}
//-----------------------------------------------------------------------------
void RemoteChannelManager::doTotmann() {
    bi::scoped_lock<Mutex> lock(*mutex);
    *totmann_time = ::time(NULL);
}
//-----------------------------------------------------------------------------
void RemoteChannelManager::destroyShm() {
    using namespace boost::interprocess;
    // DO WE NEED THIS, BECAUSE AT THE END WE REMOVE THE WHOLE MEMORY?
    //shmh.get().destroy_ptr(changed);
    //shmh.get().destroy_ptr(mutex);
    //shmh.get().destroy_ptr(channels);
    //shmh.get().destroy_ptr(references);
}
//-----------------------------------------------------------------------------
RemoteChannelManager::~RemoteChannelManager() {
 /*  if (--(*references)<=0) {
        destroyShm();
   }*/
    if (shmh) {
        delete shmh;
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
std::string RemoteChannelManager::getStreamId(const RCData &data) const
{
    return boost::get<0>(data);
}
//-----------------------------------------------------------------------------
Stream::Ptr RemoteChannelManager::getStream(const RCId &rc) {
    RCData data = getChannelData(rc);
    return Stream::open(getStreamId(data));
}
//-----------------------------------------------------------------------------
void RemoteChannelManager::__addChannel_(const RCId &id, const RCData &data)
{

    bi::scoped_lock<Mutex> lock(*mutex);
    si::String::TheAllocator str_alloc( shmh->get().get_segment_manager() );
    channels->insert( std::make_pair(
        SHM_RCId(id.c_str(), str_alloc),
        boost::make_tuple(
            SHM_String(boost::get<0>(data).c_str(), str_alloc)
        )
    ));
    *changed = ::time(NULL);
}
//-----------------------------------------------------------------------------
RemoteChannelManager::RCId RemoteChannelManager::addChannel(const RCData &data)
{
    RCId id = createUniqueName();
    __addChannel_(id, data);
    return id;
}
//-----------------------------------------------------------------------------
void RemoteChannelManager::removeChannel(const RCId &id) {

    bi::scoped_lock<Mutex> lock(*mutex);

    si::String::TheAllocator str_alloc( shmh->get().get_segment_manager() );
    channels->erase(SHM_String(id.c_str(), str_alloc));
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
    si::String::TheAllocator str_alloc( shmh->get().get_segment_manager() );
    RemoteChannels::Class::const_iterator it = channels->find(SHM_String(id.c_str(), str_alloc));
    if (it==channels->end()) {
        return RCData();
    }
    return boost::make_tuple(
        boost::get<0>(it->second).c_str()
    );
}
//-----------------------------------------------------------------------------
void RemoteChannelManager::removeAllChannels() {
    bi::scoped_lock<Mutex> lock(*mutex);
    channels->clear();
}

}}} // namespace(s)
