/*
 * BridgeSessionManager.cpp
 *
 *  Created on: Mon Dec  2 10:48:48 2013
 *      Author: Johannes Unger
 */

#include "BridgeSessionManager.hpp"
#include <sambag/com/Config.h>
#include <boost/regex.hpp>
#include "BridgeSession.hpp"
#include <boost/filesystem.hpp>
#include <sambag/com/exceptions/IllegalStateException.hpp>
#include "SessionManager.hpp"
#include <processing/FrxAsyncDSPTimer.hpp>
#include <sambag/com/BoostTimer2.hpp>

namespace com {
    extern void startProcess(const char *path, int argc, const char **argv);
}

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
BridgeSessionManager::BridgeSessionManager() : path("./bridge") {
}
//-----------------------------------------------------------------------------
BridgeSessionManager & BridgeSessionManager::instance() {
	return BridgeSessionManagerHolder::Instance();
}
//-----------------------------------------------------------------------------
std::string BridgeSessionManager::getBridgeSessionId() const {
    // macosx fails to ceate shm when '/' in name
    //std::string pathId = boost::regex_replace(getBridgePath(), boost::regex("[^\\w]"), "");
    //return pathId+"."+"VSTForx.Bridge.Mainsession." + FRX_BRIDGE_ARCH_STR;
    return SessionManager::createUniqueName();
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
    if (!bs::exists(bs::path( getBridgePath()))) {
        SAMBAG_THROW(IllegalStateException, getBridgePath() + " not found.");
    }
    std::string id = getBridgeSessionId();
    const char *args[] = { id.c_str() };
    com::startProcess(getBridgePath().c_str(), 1, &args[0]);
    boost::this_thread::sleep(boost::posix_time::millisec(
        Session::DEFAULT_SLEEPING_TIME * 2
    ));
    ___bridge_ = BridgeSessionClient::create(id);
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
   return createPluginSession(pI.location, hI->getSampleRate(), hI->getBlockSize());
}
//-----------------------------------------------------------------------------
PluginSessionClientPtr BridgeSessionManager::createPluginSession(
    const std::string &path, float sampleRate, int blockSize)
{
    BridgeSessionClient::Ptr session = getBridgeClient();
    PluginSessionClientPtr res = session->createPluginSession(
        path, sampleRate, blockSize
    );
    return res;
}
//-----------------------------------------------------------------------------
bool BridgeSessionManager::isBridgeSessionEstabished() const {
    return ___bridge_.get() != NULL;
}
}}} // namespace(s)
