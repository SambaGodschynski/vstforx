/*
 * RemoteChannelManager.hpp
 *
 *  Created on: Sat Sep 14 09:23:31 2013
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_REMOTECHANNELMANAGER_H
#define SAMBAG_REMOTECHANNELMANAGER_H

#include <loki/Singleton.h>
#include "Stream.hpp"
#include "SessionManager.hpp"

namespace frx { namespace processing { namespace interprocess {
//=============================================================================
/** 
  * @class RemoteChannelManager<Singleton>.
  *
  * Manages between RemoteChannels/Receiver and it's related RCHandler.
  */
class RemoteChannelManager : public SessionManager {
//=============================================================================
friend struct Loki::CreateUsingNew<RemoteChannelManager>;
private:
    //-------------------------------------------------------------------------
    bool isStreamIdValid(const std::string &streamId) const;
protected:
    //-------------------------------------------------------------------------
    RemoteChannelManager();
public:
    //-------------------------------------------------------------------------
    typedef SessionManager Super;
    //-------------------------------------------------------------------------
    typedef Super::SessionId RCId;
    //-------------------------------------------------------------------------
    typedef Super::SessionData RCData;
    //-------------------------------------------------------------------------
	static RemoteChannelManager & instance();
    //-------------------------------------------------------------------------
    UInteger getNumChannels() const;
    //-------------------------------------------------------------------------
    void getChannels(std::vector<SessionId> &out);
    void getChannels(std::list<SessionId> &out);
    //-------------------------------------------------------------------------
    SessionData getChannelData(const SessionId &id) const;
    //-------------------------------------------------------------------------
    Stream::Ptr getStream(const SessionId &rc);
    //-------------------------------------------------------------------------
    // Sender stuff
    //-------------------------------------------------------------------------
    void addChannel(const SessionId &hnd, const SessionData &data);
    //-------------------------------------------------------------------------
    SessionId addChannel(const SessionData &data);
    //-------------------------------------------------------------------------
    void removeChannel(const SessionId &hnd);
    //-------------------------------------------------------------------------
    void removeAllChannels();
    //-------------------------------------------------------------------------
    std::string getName(const SessionId &id);
}; // RemoteChannelManager
}}} // namespace(s)

#endif /* SAMBAG_REMOTECHANNELMANAGER_H */