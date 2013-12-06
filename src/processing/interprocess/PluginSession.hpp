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
        struct SetPluginLocation {
            typedef struct Arg { char *path; } *ArgPtr;
            typedef struct Ret {} *RetPtr;
        };
        struct GetPluginLocation {
            typedef struct Arg {} *ArgPtr;
            typedef struct Ret { char *path; } *RetPtr;
        };
        typedef LOKI_TYPELIST_3(
            Close,
            SetPluginLocation,
            GetPluginLocation
        ) OPs;
        typedef helper::AutoOPC<OPs> OpcManager;
    };
    typedef Operations::OpcManager OpcM;
    //-------------------------------------------------------------------------
    PluginSessionHost(const std::string &id, float sampleRate,
        Integer blockSize, Integer numChannels);
    //-------------------------------------------------------------------------
    void processImpl(Opc opc, void *argmen, void *retmem);
private:
    //-------------------------------------------------------------------------
    BridgeSession *host;
public:
    //-------------------------------------------------------------------------
    BridgeSession * getBridgeSession() const {
        return host;
    }
    //-------------------------------------------------------------------------
    /**
     * creates new session
     */
    static Ptr create(BridgeSession *host, const std::string &id, float sampleRate,
        Integer blockSize, Integer numChannels);
    //--------------------------------------------------------------------------
    ///////////////////////////////////////////////////////////////////////////
    //-------------------------------------------------------------------------
    void auto_opc_callback(Operations::Close::ArgPtr,
        Operations::Close::RetPtr);
    //-------------------------------------------------------------------------
    void auto_opc_callback(Operations::SetPluginLocation::ArgPtr,
        Operations::SetPluginLocation::RetPtr);
    //-------------------------------------------------------------------------
    void auto_opc_callback(Operations::GetPluginLocation::ArgPtr,
        Operations::GetPluginLocation::RetPtr);
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
