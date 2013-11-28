/*
 * RemoteChannelManager.cpp
 *
 *  Created on: Sat Sep 14 09:23:31 2013
 *      Author: Johannes Unger
 */
#include "RemoteChannelManager.hpp"
namespace frx { namespace processing { namespace interprocess {
typedef Loki::SingletonHolder<RemoteChannelManager> RemoteChannelManagerHolder;
//=============================================================================
//  Class RemoteChannelManager
//=============================================================================
//-----------------------------------------------------------------------------
RemoteChannelManager::RemoteChannelManager()
{
    initManager("VSTForx.RemoteChannelManager");
}
//-----------------------------------------------------------------------------
RemoteChannelManager & RemoteChannelManager::instance() {
	RemoteChannelManager &res = RemoteChannelManagerHolder::Instance();
    return res;
}
//-----------------------------------------------------------------------------
UInteger RemoteChannelManager::getNumChannels() const {
    return Super::getNumSessions();
}
//-----------------------------------------------------------------------------
void RemoteChannelManager::getChannels(std::vector<SessionId> &out) {
    Super::getSessions(out);
}

//-----------------------------------------------------------------------------
void RemoteChannelManager::getChannels(std::list<SessionId> &out) {
    Super::getSessions(out);
}
//-----------------------------------------------------------------------------
RemoteChannelManager::SessionData
RemoteChannelManager::getChannelData(const SessionId &id) const
{
    return Super::getSessionData(id);
}
//-----------------------------------------------------------------------------
bool RemoteChannelManager::isStreamIdValid(const std::string &id) const {
    return id.length() > 0;
}
//-----------------------------------------------------------------------------
Stream::Ptr RemoteChannelManager::getStream(const SessionId &rc) {
    SessionData data = getChannelData(rc);
    const std::string &streamId = Super::getObjectId(data);
    if (!isStreamIdValid(streamId)) {
        return Stream::Ptr();
    }
    return Stream::open(streamId);
}
//-----------------------------------------------------------------------------
void RemoteChannelManager::
addChannel(const SessionId &hnd, const SessionData &data)
{
    Super::addSession(hnd, data);
}
//-----------------------------------------------------------------------------
RemoteChannelManager::SessionId
RemoteChannelManager::addChannel(const SessionData &data)
{
    return Super::addSession(data);
}
//-----------------------------------------------------------------------------
void RemoteChannelManager::removeChannel(const SessionId &hnd) {
    Super::removeSession(hnd);
}
//-----------------------------------------------------------------------------
void RemoteChannelManager::removeAllChannels() {
    Super::removeAllSessions();
}

//-----------------------------------------------------------------------------
std::string RemoteChannelManager::getName(const SessionId &id) {
    return Super::getSessionName(id);
}
}}} // namespace(s)