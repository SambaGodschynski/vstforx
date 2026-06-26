// Stub: interprocess bridge removed. Provides no-op RemoteChannelManager.
#pragma once
#include <string>
#include <list>
#include <vector>

namespace frx { namespace processing { namespace interprocess {

class RemoteChannelManager {
public:
    typedef std::string SessionId;
    typedef std::string RCId;
    struct SessionData {};
    typedef SessionData RCData;

    static RemoteChannelManager & instance() {
        static RemoteChannelManager inst;
        return inst;
    }
    int getLastChangedTime() const { return 0; }
    void getChannels(std::list<SessionId> &) const {}
    void getChannels(std::vector<SessionId> &) const {}
    SessionData getChannelData(const SessionId &) const { return {}; }
    std::string getName(const SessionId &) const { return ""; }
    std::string getAddress(const SessionId &) const { return ""; }
    void addChannel(const SessionId &, const SessionData &) {}
    SessionId addChannel(const SessionData &) { return {}; }
    void removeChannel(const SessionId &) {}
    void removeAllChannels() {}
    unsigned getNumChannels() const { return 0; }
};

}}} // namespace frx::processing::interprocess
