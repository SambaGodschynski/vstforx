/*
 * HostSession.hpp
 *
 *  Created on: Mon Dec  2 10:54:09 2013
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_HOSTSESSION_H
#define SAMBAG_HOSTSESSION_H

#include <boost/shared_ptr.hpp>
#include "Session.hpp"
#include <boost/static_assert.hpp>
#include <loki/Typelist.h>

namespace frx { namespace processing { namespace interprocess {
//=============================================================================
/** 
  * @class Bridge Host Session.
  */
class HostSession : public Session
{
//=============================================================================
public:
    //-------------------------------------------------------------------------
    struct Operations {
        struct CloseHost {
            typedef struct Arg {} *ArgPtr;
            typedef struct Ret {} *RetPtr;
        };
        struct CreatePluginSession {
            typedef struct Arg {} *ArgPtr;
            typedef struct Ret { char * id; } *RetPtr;
        };
        typedef LOKI_TYPELIST_2(CloseHost, CreatePluginSession) TypeList;
        typedef helper::AutoOPC<TypeList> OpcManager;
    };
protected:
    //-------------------------------------------------------------------------
    void processImpl(Opc opc, void *argmen, void *retmem);
private:
    //-------------------------------------------------------------------------
    bool isRunning;
public:
    //-------------------------------------------------------------------------
    void startMainLoop();
    //-------------------------------------------------------------------------
    void stopMainLoop();
    //-------------------------------------------------------------------------
    HostSession(const std::string &id);
    ///////////////////////////////////////////////////////////////////////////
    //-------------------------------------------------------------------------
    void auto_opc_callback(Operations::CloseHost::ArgPtr,
        Operations::CloseHost::RetPtr);
    //-------------------------------------------------------------------------
    void auto_opc_callback(Operations::CreatePluginSession::ArgPtr,
        Operations::CreatePluginSession::RetPtr);
}; // HostSession

//=============================================================================
/** 
  * @class HostSessionClient.
  */
class HostSessionClient : public Session
{
//=============================================================================
    
}; // HostSessionClient

}}} // namespace(s)

#endif /* SAMBAG_HOSTSESSION_H */
