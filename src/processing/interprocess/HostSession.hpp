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
struct HostSessionBase {
    struct CloseHost {
        enum { OpCode = 0 };
        typedef struct Arg {} *ArgPtr;
        typedef struct Ret {} *RetPtr;
    };
};

//=============================================================================
/** 
  * @class Bridge Host Session.
  */
class HostSession : public HostSessionBase, public Session
{
//=============================================================================
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
private:
    ///////////////////////////////////////////////////////////////////////////
    //-------------------------------------------------------------------------
    void doClose(CloseHost::ArgPtr, CloseHost::RetPtr);
}; // HostSession

//=============================================================================
/** 
  * @class HostSessionClient.
  */
class HostSessionClient : public HostSessionBase, public Session
{
//=============================================================================
    
}; // HostSessionClient

}}} // namespace(s)

#endif /* SAMBAG_HOSTSESSION_H */
