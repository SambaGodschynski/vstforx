/*
 * BridgeSession.cpp
 *
 *  Created on: Mon Dec  2 10:54:09 2013
 *      Author: Johannes Unger
 */

#include "BridgeSession.hpp"
#include <sambag/com/exceptions/IllegalArgumentException.hpp>
#include "SessionManager.hpp"
#include "PluginSession.hpp"
#include <com/FrxConfig.h>
#include <sambag/disco/components/WindowToolkit.hpp>

namespace frx { namespace processing { namespace interprocess {
//=============================================================================
//  Class BridgeSession
//=============================================================================
//-----------------------------------------------------------------------------
BridgeSession::BridgeSession(const std::string &id) : Session(id,
    ChannelSize(OpcM::MaxArgmemSize, OpcM::MaxRetmemSize),
    ChannelSize(BridgeSessionClient::OpcM::MaxArgmemSize, BridgeSessionClient::OpcM::MaxRetmemSize))
{
}
//-----------------------------------------------------------------------------
void BridgeSession::processImpl(Opc opc, void *argmem, void *retmem) {
    bool res;
    try {
        res = Operations::OpcManager::process(opc, this, argmem, retmem);
    } catch(...) {}
    if (!res) {
        using sambag::com::exceptions::IllegalArgumentException;
        SAMBAG_THROW(IllegalArgumentException,
        "BridgeSession::processImpl opc: " + sambag::com::toString(opc) + " not supported");
    }
}
//-----------------------------------------------------------------------------
void BridgeSession::startMainLoop() {
    SAMBAG_LOG_INFO<<getId()<<" main thread started";
    sambag::disco::components::getWindowToolkit()->startMainLoop();
    SAMBAG_LOG_INFO<<getId()<<" main thread ended";
}
//-----------------------------------------------------------------------------
void BridgeSession::stopMainLoop() {
    SAMBAG_LOG_INFO<<getId()<<": closing main thread";
    typedef BridgeSessionClient::Operations::OnBridgeClosing Op;
    try {
        waitForResult( BridgeSessionClient::OpcM::getOPC<Op>() );
    } catch(...) {}
    
    sambag::disco::components::getWindowToolkit()->quit();
}
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
size_t BridgeSession::getNumPluginSessions() const {
    SAMBAG_BEGIN_SYNCHRONIZED(mutex)
        return plugHostMap.size();
    SAMBAG_END_SYNCHRONIZED
}
//-----------------------------------------------------------------------------
void BridgeSession::auto_opc_callback(Operations::CreatePluginSession::ArgPtr arg,
    Operations::CreatePluginSession::RetPtr ret)
{
    SAMBAG_BEGIN_SYNCHRONIZED(mutex)
        SAMBAG_LOG_INFO<<"creating a plugin session for: "<<arg->path;
        std::string id = SessionManager::createUniqueName();
        
        BridgePluginDelegate::Ptr delegate =
            BridgePluginDelegate::create(arg->blockSize, arg->sampleRate, arg->path);
        PluginSessionHost::Ptr ps = PluginSessionHost::create(delegate, this);
        plugHostMap[id] = ps;
        strcpy(ret->id, id.c_str());
        SAMBAG_LOG_INFO<<"plugin created: "<<arg->path;
    SAMBAG_END_SYNCHRONIZED
}
//-----------------------------------------------------------------------------
void BridgeSession::auto_opc_callback(Operations::ClosePluginSession::ArgPtr arg,
    Operations::ClosePluginSession::RetPtr ret)
{
    SAMBAG_BEGIN_SYNCHRONIZED(mutex)
    
    SAMBAG_END_SYNCHRONIZED
}
//=============================================================================
//  Class BridgeSessionClient
//=============================================================================
//-----------------------------------------------------------------------------
BridgeSessionClient::BridgeSessionClient(const SessionId &id) :
    Session(id)
{
}
//-----------------------------------------------------------------------------
BridgeSessionClient::Ptr BridgeSessionClient::create(const SessionId &id) {
    Ptr res( new BridgeSessionClient(id) );
    res->self = res;
    return res;
}
//-----------------------------------------------------------------------------
void BridgeSessionClient::processImpl(Opc opc, void *argmem, void *retmem) {
    if ( !Operations::OpcManager::process(opc, this, argmem, retmem)) {
        using sambag::com::exceptions::IllegalArgumentException;
        SAMBAG_THROW(IllegalArgumentException,
        "BridgeSession::processImpl opc: " + sambag::com::toString(opc) + " not supported");
    }
}
//-----------------------------------------------------------------------------
PluginSessionClientPtr BridgeSessionClient::createPluginSession(
        const std::string &path,
        float sampleRate,
        Integer blockSize)
{
    SAMBAG_LOG_INFO<<"try to establish a plugin session for: "<<path;
    if (path.length() > FRX_SHMSESS_MAX_PATH_LENGTH) {
        using sambag::com::exceptions::IllegalArgumentException;
        SAMBAG_THROW(IllegalArgumentException, "pathlength out of bounds");
    }
    typedef BridgeSession::Operations::CreatePluginSession Op;
    Op::ArgPtr args = static_cast<Op::ArgPtr>(getArgmem());
    strcpy(args->path, path.c_str());
    args->sampleRate = sampleRate;
    args->blockSize = blockSize;
    Op::RetPtr rets = NULL;
    try {
        rets = waitForResult<Op::RetPtr>(
            BridgeSession::OpcM::getOPC<Op>(),
            FRX_BRIDGE_CREATE_PL_SESSION_TIMEOUT
        );
    } catch (const std::exception &ex) {
        SAMBAG_LOG_ERR<<"establishing plugin session failed: "<<ex.what()<<", "<<path;
        throw;
    } catch (...) {
        SAMBAG_LOG_ERR<<"establishing plugin session failed: unknown error, "<<path;
        throw;
    }
    //std::string id(rets->id);
    //PluginSessionClient::Ptr res = PluginSessionClient::create(id);
    SAMBAG_LOG_INFO<<"plugin session estabished: "<<path;
    return PluginSessionClient::Ptr();
}
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
void BridgeSessionClient::auto_opc_callback(Operations::OnBridgeClosing::ArgPtr,
        Operations::OnBridgeClosing::RetPtr)
{
    Ptr holder = self.lock();
    using namespace sambag::com::events;
    EventSender<ClosingEvent>::notifyListeners(
        this, ClosingEvent()
    );
}
}}} // namespace(s)
