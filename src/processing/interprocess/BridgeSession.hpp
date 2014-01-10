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
class BridgeSessionClient;
typedef boost::shared_ptr<PluginSessionHost> PluginSessionHostPtr;
typedef boost::shared_ptr<PluginSessionClient> PluginSessionClientPtr;
//=============================================================================
/** 
  * @class BridgeSession.
  * @brief The bridge session on host side.
  * Manager for adding and removing of PluginSessions. 
  */
class BridgeSession : public Session
{
//=============================================================================
public:
    //-------------------------------------------------------------------------
    typedef BridgeSessionClient SessionHost; // host for session calls
    //-------------------------------------------------------------------------
    typedef std::string SessionId;
    //-------------------------------------------------------------------------
    FRX_OP_BEGIN_OPERATIONS
        FRX_OP_OPERATION(CreatePluginSession,
            FRX_OP_ARG_3(float sampleRate,
                Integer blockSize,
                char path[FRX_SHMSESS_MAX_PATH_LENGTH]
            ),
            FRX_OP_RET_2 ( char result[FRX_SHMSESS_MAX_STR_LENGTH],
                bool succeed
            )
        );
        FRX_OP_OPERATION ( ClosePluginSession,
            FRX_OP_ARG_1 (char id[FRX_SHMSESS_MAX_STR_LENGTH]),
            FRX_OP_RET()
        );
        typedef LOKI_TYPELIST_2(
            CreatePluginSession,
            ClosePluginSession
        ) OPs;
    FRX_OP_END_OPERATIONS(OPs)
protected:
    //-------------------------------------------------------------------------
    FRX_OP_PROCESS_IMPL
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
    FRX_OP_CALLBACK_METHOD(CreatePluginSession);
    FRX_OP_CALLBACK_METHOD(ClosePluginSession);
}; // BridgeSession

//=============================================================================
/** 
  * @class BridgeSessionClient.
  * @brief The bridge session on client side.
  */
struct OnBridgeClosing {};
class BridgeSessionClient : public Session,
    public sambag::com::events::EventSender<OnBridgeClosing>
{
//=============================================================================
public:
    //-------------------------------------------------------------------------
    typedef BridgeSession SessionHost; // host for session calls
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
    WPtr self;
public:
    //-------------------------------------------------------------------------
    FRX_OP_BEGIN_OPERATIONS
        FRX_OP_OPERATION( OnBridgeClosing,
            FRX_OP_ARG(),
            FRX_OP_RET()
        );
        typedef LOKI_TYPELIST_1(OnBridgeClosing) OPs;
    FRX_OP_END_OPERATIONS_AND_IMPL_PROCESS(OPs)
    //-------------------------------------------------------------------------
    PluginSessionClientPtr createPluginSession(
        const std::string &path,
        float sampleRate,
        Integer blockSize
    );
    //-------------------------------------------------------------------------
    void closePluginSession(PluginSessionClientPtr session);
    //-------------------------------------------------------------------------
    static Ptr create(const SessionId &id);
    ///////////////////////////////////////////////////////////////////////////
    //-------------------------------------------------------------------------
    FRX_OP_CALLBACK_METHOD(OnBridgeClosing);

}; // BridgeSessionClient

}}} // namespace(s)

#endif /* SAMBAG_BridgeSession_H */
