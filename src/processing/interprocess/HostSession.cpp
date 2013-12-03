/*
 * HostSession.cpp
 *
 *  Created on: Mon Dec  2 10:54:09 2013
 *      Author: Johannes Unger
 */

#include "HostSession.hpp"
#include <sambag/com/Thread.hpp>
#include <sambag/com/exceptions/IllegalArgumentException.hpp>

namespace frx { namespace processing { namespace interprocess {
//=============================================================================
//  Class HostSession
//=============================================================================
//-----------------------------------------------------------------------------
HostSession::HostSession(const std::string &id) : Session(id,
    ChannelSize(64,64),
    ChannelSize(64,64)),
    isRunning(false)
{
}
//-----------------------------------------------------------------------------
void HostSession::processImpl(Opc opc, void *argmem, void *retmem) {
    if ( !Operations::OpcManager::process(opc, this, argmem, retmem)) {
        using sambag::com::exceptions::IllegalArgumentException;
        SAMBAG_THROW(IllegalArgumentException,
        "HostSession::processImpl opc: " + sambag::com::toString(opc) + " not supported");
    }
}
//-----------------------------------------------------------------------------
void HostSession::startMainLoop() {
    isRunning=true;
    SAMBAG_LOG_INFO<<getId()<<" main thread started";
    while(isRunning) {
        boost::this_thread::sleep(boost::posix_time::millisec(100));
    }
    SAMBAG_LOG_INFO<<getId()<<" main thread ended";
}
//-----------------------------------------------------------------------------
void HostSession::stopMainLoop() {
    SAMBAG_LOG_INFO<<getId()<<": closing main thread";
    isRunning = false;
}
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
void HostSession::auto_opc_callback(Operations::CloseHost::ArgPtr,
    Operations::CloseHost::RetPtr)
{
    stopMainLoop();
}
//-----------------------------------------------------------------------------
void HostSession::auto_opc_callback(Operations::CreatePluginSession::ArgPtr,
    Operations::CreatePluginSession::RetPtr)
{
    
}
}}} // namespace(s)
