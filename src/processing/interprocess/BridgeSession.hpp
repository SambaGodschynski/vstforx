/*
 * BridgeSession.hpp
 *
 *  Created on: Mon Dec  2 10:54:09 2013
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_BridgeSession_H
#define SAMBAG_BridgeSession_H

#include <boost/shared_ptr.hpp>
#include "Session.hpp"
#include <boost/static_assert.hpp>
#include <loki/Typelist.h>
#include <map>
#include <com/FrxConfig.h>
#include <sambag/com/Thread.hpp>
#include <sambag/com/events/Events.hpp>

namespace frx { namespace processing { namespace interprocess {
class PluginSessionHost;
class PluginSessionClient;
typedef boost::shared_ptr<PluginSessionHost> PluginSessionHostPtr;
typedef boost::shared_ptr<PluginSessionClient> PluginSessionClientPtr;
//=============================================================================
/** 
  * @class Bridge Host Session.
  */
class BridgeSession : public Session
{
//=============================================================================
public:
    //-------------------------------------------------------------------------
    typedef std::string SessionId;
    //-------------------------------------------------------------------------
    struct Operations {
        struct CreatePluginSession {
            typedef struct Arg { float sampleRate;
                                 Integer blockSize;
                                 char path[FRX_SHMSESS_MAX_PATH_LENGTH];
                                } *ArgPtr;
            typedef struct Ret {
                char result[FRX_SHMSESS_MAX_STR_LENGTH];
                bool succeed;
            } *RetPtr;
        };
        struct ClosePluginSession {
            typedef struct Arg { char id[FRX_SHMSESS_MAX_STR_LENGTH]; } *ArgPtr;
            typedef struct Ret {} *RetPtr;
        };
        typedef LOKI_TYPELIST_2(
            CreatePluginSession,
            ClosePluginSession
        ) OPs;
        typedef helper::AutoOPC<OPs> OpcManager;
    };
    typedef Operations::OpcManager OpcM;
protected:
    //-------------------------------------------------------------------------
    void processImpl(Opc opc, void *argmen, void *retmem);
    //-------------------------------------------------------------------------
    typedef std::map<std::string, PluginSessionHostPtr> PluginSessionHostMap;
    PluginSessionHostMap plugHostMap;
private:
    //-------------------------------------------------------------------------
    mutable sambag::com::RecursiveMutex mutex;
public:
    //-------------------------------------------------------------------------
    size_t getNumPluginSessions() const;
    //-------------------------------------------------------------------------
    void startMainLoop();
    //-------------------------------------------------------------------------
    void stopMainLoop();
    //-------------------------------------------------------------------------
    BridgeSession(const SessionId &id);
    ///////////////////////////////////////////////////////////////////////////
    //-------------------------------------------------------------------------
    void auto_opc_callback(Operations::CreatePluginSession::ArgPtr,
        Operations::CreatePluginSession::RetPtr);
    //-------------------------------------------------------------------------
    void auto_opc_callback(Operations::ClosePluginSession::ArgPtr,
        Operations::ClosePluginSession::RetPtr);
}; // BridgeSession

//=============================================================================
/** 
  * @class BridgeSessionClient.
  */
struct OnBridgeClosing {};
class BridgeSessionClient : public Session,
    public sambag::com::events::EventSender<OnBridgeClosing>
{
//=============================================================================
public:
    //-------------------------------------------------------------------------
    typedef boost::shared_ptr<BridgeSessionClient> Ptr;
    //-------------------------------------------------------------------------
    typedef boost::weak_ptr<BridgeSessionClient> WPtr;
    //-------------------------------------------------------------------------
    typedef OnBridgeClosing ClosingEvent;
    //-------------------------------------------------------------------------
    typedef std::string SessionId;
protected:
    //-------------------------------------------------------------------------
    BridgeSessionClient(const SessionId &id);
    //-------------------------------------------------------------------------
    void processImpl(Opc opc, void *argmen, void *retmem);
    //-------------------------------------------------------------------------
    WPtr self;
public:
    //-------------------------------------------------------------------------
    struct Operations {
        struct OnBridgeClosing {
            typedef struct Arg {} *ArgPtr;
            typedef struct Ret {} *RetPtr;
        };
        typedef LOKI_TYPELIST_1(
            OnBridgeClosing
        ) OPs;
        typedef helper::AutoOPC<OPs> OpcManager;
    };
    typedef Operations::OpcManager OpcM;
    //-------------------------------------------------------------------------
    PluginSessionClientPtr createPluginSession(
        const std::string &path,
        float sampleRate,
        Integer blockSize
    );
    //-------------------------------------------------------------------------
    static Ptr create(const SessionId &id);
    ///////////////////////////////////////////////////////////////////////////
    //-------------------------------------------------------------------------
    void auto_opc_callback(Operations::OnBridgeClosing::ArgPtr,
        Operations::OnBridgeClosing::RetPtr);

}; // BridgeSessionClient

}}} // namespace(s)

#endif /* SAMBAG_BridgeSession_H */
