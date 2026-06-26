// Stub: interprocess bridge removed.
#pragma once
#include <loki/Singleton.h>
#include <string>

namespace frx { namespace processing { namespace interprocess {
class BridgeSessionManager {
friend struct Loki::CreateUsingNew<BridgeSessionManager>;
public:
    static BridgeSessionManager & instance() {
        static BridgeSessionManager inst;
        return inst;
    }
    bool isBridge() { return false; }
    void setBridgePath(const std::string &) {}
    const std::string & getBridgePath() const { static std::string s; return s; }
    std::string getBridgeSessionId() const { return ""; }
    bool isBridgeSessionEstabished() const { return false; }
};
}}} // namespace frx::processing::interprocess
