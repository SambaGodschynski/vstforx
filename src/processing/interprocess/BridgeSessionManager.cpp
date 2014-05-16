/*
 * BridgeSessionManager.cpp
 *
 *  Created on: Mon Dec  2 10:48:48 2013
 *      Author: Johannes Unger
 */

#include "BridgeSessionManager.hpp"
#include <sambag/com/Config.h>
#include "BridgeSession.hpp"
#include <boost/filesystem.hpp>
#include <sambag/com/exceptions/IllegalStateException.hpp>
#include "SessionManager.hpp"
#include <processing/FrxAsyncDSPTimer.hpp>
#include <sambag/com/BoostTimer2.hpp>

namespace com {
    extern void startProcess(const char *path, int argc, const char **argv);
}

const char * FRX_BRIDGE_EXEC_NAME = "VSTForx.bridge";

namespace frx { namespace processing { namespace interprocess {
#ifdef SAMBAG_64
    const std::string FRX_BRIDGE_ARCH_STR("64-32");
#else
    const std::string FRX_BRIDGE_ARCH_STR("32-64");
#endif
typedef Loki::SingletonHolder<BridgeSessionManager> BridgeSessionManagerHolder;

//=============================================================================
//  Class BridgeSessionManager
//=============================================================================
//-----------------------------------------------------------------------------
BridgeSessionManager::BridgeSessionManager() : path(std::string("./") + FRX_BRIDGE_EXEC_NAME)
{
}
//-----------------------------------------------------------------------------
BridgeSessionManager & BridgeSessionManager::instance() {
	return BridgeSessionManagerHolder::Instance();
}
//-----------------------------------------------------------------------------
std::string BridgeSessionManager::getBridgeSessionId() const {
    return "bs-"+SessionManager::createUniqueName();
}
//-----------------------------------------------------------------------------
void BridgeSessionManager::setBridgePath(const std::string &path) {
    this->path = path;
}
//-----------------------------------------------------------------------------
void BridgeSessionManager::onHostClosingAsync() {
      ___bridge_.reset();
}
//-----------------------------------------------------------------------------
void BridgeSessionManager::onHostClosing() {
    typedef sambag::com::BoostTimer2 Timer;
    Timer::Ptr timer = Timer::create(10);
    timer->addEventListener(
        boost::bind(&BridgeSessionManager::onHostClosingAsync, this)
    );
    timer->start();
}
//-----------------------------------------------------------------------------
void BridgeSessionManager::startBridge() {
    namespace bs=boost::filesystem;
    using sambag::com::exceptions::IllegalStateException;
    using sambag::com::events::EventSender;
    
    if (isBridge()) {
        SAMBAG_THROW(IllegalStateException, "bridge recursion error.");
    }
    
    if (!bs::exists(bs::path( getBridgePath()))) {
        SAMBAG_THROW(IllegalStateException, getBridgePath() + " not found.");
    }
    std::string id = getBridgeSessionId();
    
    // note: to make startup syncronization easier,
    //       the client creates the session memory
    ___bridge_ = BridgeSessionClient::create(id);
    
    // start process now
    const char *args[] = { id.c_str() };
    com::startProcess(getBridgePath().c_str(), 1, &args[0]);
    SAMBAG_ASSERT(___bridge_);
    
    ___bridge_->EventSender<BridgeSessionClient::ClosingEvent>::addEventListener(
        boost::bind(&BridgeSessionManager::onHostClosing, this)
    );
}
//-----------------------------------------------------------------------------
BridgeSessionClientPtr BridgeSessionManager::getBridgeClient() {
    if (!___bridge_) {
        startBridge();
    }
    SAMBAG_ASSERT(___bridge_);
    return ___bridge_;
}
//-----------------------------------------------------------------------------
PluginSessionClientPtr BridgeSessionManager::
createPluginSession(IHostInfo::Ptr hI, const ::processing::PluginInfo &pI)
{
   return createPluginSession(pI.location, hI);
}
//-----------------------------------------------------------------------------
PluginSessionClientPtr BridgeSessionManager::createPluginSession(
    const std::string &path, IHostInfo::Ptr hI)
{
    BridgeSessionClient::Ptr session = getBridgeClient();
    PluginSessionClientPtr res = session->createPluginSession(path, hI);
    return res;
}
//-----------------------------------------------------------------------------
void BridgeSessionManager::closePluginSession(PluginSessionClientPtr plSession) {
    BridgeSessionClient::Ptr session = getBridgeClient();
    session->closePluginSession(plSession);
}
//-----------------------------------------------------------------------------
bool BridgeSessionManager::isBridgeSessionEstabished() const {
    return ___bridge_.get() != NULL;
}
}}} // namespace(s)
