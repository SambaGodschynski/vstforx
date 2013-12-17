/*
 * BridgeSession.cpp
 *
 *  Created on: Mon Dec  2 10:54:09 2013
 *      Author: Johannes Unger
 */

#include "BridgeSession.hpp"
#include <sambag/com/exceptions/IllegalArgumentException.hpp>
#include <sambag/com/exceptions/IllegalStateException.hpp>
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
    try {
        Operations::OpcManager::process(opc, this, argmem, retmem);
    } catch(const std::exception &ex) {
        SAMBAG_LOG_ERR<<ex.what()<<" opc("<<opc<<")";
        throw;
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
        // because I did the misstake before:
        // do not use the location as id, because by doing so
        // only one pluginstance would be possible
        std::string result_str = SessionManager::createUniqueName();
        try {
            ret->succeed = false;
            BridgePluginDelegate::Ptr delegate =
                BridgePluginDelegate::create(arg->blockSize, arg->sampleRate, arg->path);
            PluginSessionHost::Ptr ps = PluginSessionHost::create(delegate, this);
            plugHostMap[result_str] = ps;
            ret->succeed = true;
        } catch (const std::exception &ex) {
            result_str = ex.what();
        } catch (...) {
            result_str = "uknown error.";
        }
        shm_cpystr(ret->result, result_str);
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
    try {
        Operations::OpcManager::process(opc, this, argmem, retmem);
    } catch(const std::exception &ex) {
        SAMBAG_LOG_ERR<<ex.what()<<" opc("<<opc<<")";
        throw;
    }
}
//-----------------------------------------------------------------------------
PluginSessionClientPtr BridgeSessionClient::createPluginSession(
        const std::string &path,
        float sampleRate,
        Integer blockSize)
{
    SAMBAG_LOG_INFO<<"try to establish a plugin session for: "<<path;
    typedef BridgeSession::Operations::CreatePluginSession Op;
    Op::ArgPtr args = static_cast<Op::ArgPtr>(getArgmem());
    shm_cpypath(args->path, path);
    args->sampleRate = sampleRate;
    args->blockSize = blockSize;
    Op::RetPtr rets = NULL;
    try {
        rets = waitForResult<Op::RetPtr>(
            BridgeSession::OpcM::getOPC<Op>(),
            FRX_BRIDGE_CREATE_PL_SESSION_TIMEOUT
        );
        if (!rets->succeed) {
            SAMBAG_THROW(
                sambag::com::exceptions::IllegalStateException,
                std::string( rets->result )
            );
        }
    } catch (const std::exception &ex) {
        SAMBAG_LOG_ERR<<"establishing plugin session failed: "<<ex.what()<<", "<<path;
        throw;
    } catch (...) {
        SAMBAG_LOG_ERR<<"establishing plugin session failed: unknown error, "<<path;
        throw;
    }
    std::string id(rets->result);
    PluginSessionClient::Ptr res = PluginSessionClient::create(id);
    SAMBAG_LOG_INFO<<"plugin session estabished: "<<path;
    return res;
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
