/*
 * RemoteChannelManager.cpp
 *
 *  Created on: Sat Sep 14 09:23:31 2013
 *      Author: Johannes Unger
 */

#include "RemoteChannelManager.hpp"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace {
    const int RC_MAX_MEM_SIZE = 64000;
} //namespace(s)

namespace frx { namespace processing { namespace interprocess {
typedef Loki::SingletonHolder<RemoteChannelManager> RemoteChannelManagerHolder;
//=============================================================================
//  Class RemoteChannelManager
//=============================================================================
//-----------------------------------------------------------------------------
void RemoteChannelManager::initManager() {
    shmh.initMemory("VSTForx.RemoteChannelManager", RC_MAX_MEM_SIZE);
    changed = shmh.get().find_or_construct<time_t>("changedTimestamp")();
    mutex = shmh.get().find_or_construct<Mutex>("mutex")();
    channels = RemoteChannels::findOrCreate("channelData", shmh.get());
}
//-----------------------------------------------------------------------------
RemoteChannelManager & RemoteChannelManager::instance() {
	RemoteChannelManager &res = RemoteChannelManagerHolder::Instance();
    res.initManager();
    return res;
}
//-----------------------------------------------------------------------------
size_t RemoteChannelManager::getNumChannels() const {
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
void RemoteChannelManager::addChannel(const RCId &id, const RCData &data)
{

    bi::scoped_lock<Mutex> lock(*mutex);
    
    si::String::TheAllocator str_alloc( shmh.get().get_segment_manager() );
    channels->insert( std::make_pair(
        SHM_RCId(id.c_str(), str_alloc),
        boost::make_tuple(
            SHM_String(boost::get<0>(data).c_str(), str_alloc)
        )
    ));
    *changed = ::time(NULL);
}
//-----------------------------------------------------------------------------
void RemoteChannelManager::removeChannel(const RCId &id) {

    bi::scoped_lock<Mutex> lock(*mutex);

    si::String::TheAllocator str_alloc( shmh.get().get_segment_manager() );
    channels->erase(SHM_String(id.c_str(), str_alloc));
    *changed = ::time(NULL);
    
}
//-----------------------------------------------------------------------------
std::string RemoteChannelManager::getUniqueName() const {
    boost::uuids::uuid uuid;
    return boost::uuids::to_string(uuid);
}
//-----------------------------------------------------------------------------
RemoteChannelManager::RCData
RemoteChannelManager::getChannelData(const RCId &id) const {
    bi::scoped_lock<Mutex> lock(*mutex);
    si::String::TheAllocator str_alloc( shmh.get().get_segment_manager() );
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
    channels->clear();
}

}}} // namespace(s)
