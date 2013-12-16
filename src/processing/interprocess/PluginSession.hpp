/*
 * PluginSession.hpp
 *
 *  Created on: Tue Dec  3 13:30:20 2013
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_PLUGINSESSION_H
#define SAMBAG_PLUGINSESSION_H

#include <boost/shared_ptr.hpp>
#include "Session.hpp"
#include "ShmCom.hpp"
#include "BridgePluginDelegate.hpp"

namespace frx { namespace processing { namespace interprocess {
class BridgeSession;
//=============================================================================
/** 
  * @class PluginSession.
  */
class PluginSessionHost : public Session {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<PluginSessionHost> Ptr;
    //-------------------------------------------------------------------------
    struct Operations {
        struct Close {
            typedef struct Arg {} *ArgPtr;
            typedef struct Ret {} *RetPtr;
        };
        typedef LOKI_TYPELIST_1(
            Close
        ) OPs;
        typedef helper::AutoOPC<OPs> OpcManager;
    };
    typedef Operations::OpcManager OpcM;
    //-------------------------------------------------------------------------
    PluginSessionHost(BridgePluginDelegate::Ptr delegate);
    //-------------------------------------------------------------------------
    void processImpl(Opc opc, void *argmen, void *retmem);
private:
    //-------------------------------------------------------------------------
    BridgeSession *host;
    //-------------------------------------------------------------------------
    BridgePluginDelegate::Ptr delegate;
public:
    //-------------------------------------------------------------------------
    BridgeSession * getBridgeSession() const {
        return host;
    }
    //-------------------------------------------------------------------------
    /**
     * creates new session
     */
    static Ptr create(BridgePluginDelegate::Ptr delegate, BridgeSession *host);
    //--------------------------------------------------------------------------
    ///////////////////////////////////////////////////////////////////////////
    //-------------------------------------------------------------------------
    void auto_opc_callback(Operations::Close::ArgPtr,
        Operations::Close::RetPtr);
}; // PluginSession
//=============================================================================
/** 
  * @class PluginSessionClient.
  */
class PluginSessionClient : public Session {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<PluginSessionClient> Ptr;
    //-------------------------------------------------------------------------
    struct Operations {
        typedef Loki::NullType OPs;
        typedef helper::AutoOPC<OPs> OpcManager;
    };
    typedef Operations::OpcManager OpcM;
    //-------------------------------------------------------------------------
    PluginSessionClient(const std::string &id);
    //-------------------------------------------------------------------------
    void processImpl(Opc opc, void *argmen, void *retmem);
private:
public:
    //-------------------------------------------------------------------------
    /**
     * creates new session
     */
    static Ptr create(const std::string &id);
    //--------------------------------------------------------------------------
    ///////////////////////////////////////////////////////////////////////////
}; // PluginSession
}}} // namespace(s)

#endif /* SAMBAG_PLUGINSESSION_H */
