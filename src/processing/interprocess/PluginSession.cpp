/*
 * PluginSession.cpp
 *
 *  Created on: Tue Dec  3 13:30:20 2013
 *      Author: Johannes Unger
 */

#include "PluginSession.hpp"
#include <sambag/com/exceptions/IllegalArgumentException.hpp>
#include <algorithm>

namespace frx { namespace processing { namespace interprocess {
namespace {
    template<class OpcM>
    inline Integer _minmem(Integer blockSize, Integer numChannels) {
        return std::min( blockSize*numChannels + OpcM::MaxArgmemSize
        , 256 );
    }
} // namespace
//=============================================================================
//  Class PluginSessionHost
//=============================================================================
//-----------------------------------------------------------------------------
PluginSessionHost::PluginSessionHost(const std::string &id,
    float sampleRate, Integer bs, Integer nc)
    :  Session(id, ChannelSize(_minmem<OpcM>(bs, nc), _minmem<OpcM>(bs, nc)),
                   ChannelSize(PluginSessionHost::OpcM::MaxArgmemSize,
                   PluginSessionHost::OpcM::MaxRetmemSize))
{
    if (sampleRate<=0 || bs<=0 || nc <=0) {
        using sambag::com::exceptions::IllegalArgumentException;
        std::stringstream ss;
        ss<<"BridgeSession illegal audio setup: sr("<<sampleRate<<") ";
        ss<<"bs("<<bs<<") nc("<<nc<<")";
        SAMBAG_THROW(IllegalArgumentException,
        ss.str());
    }
    setMaxSleeping( (Integer)(1000.f/(sampleRate/(float)bs)) );
}
//-----------------------------------------------------------------------------
void PluginSessionHost::processImpl(Opc opc, void *argmem, void *retmem) {
    if ( !Operations::OpcManager::process(opc, this, argmem, retmem)) {
        using sambag::com::exceptions::IllegalArgumentException;
        SAMBAG_THROW(IllegalArgumentException,
        "BridgeSession::processImpl opc: " + sambag::com::toString(opc) + " not supported");
    }
}
//-----------------------------------------------------------------------------
PluginSessionHost::Ptr PluginSessionHost::create(BridgeSession *host,
    const std::string &id, float sr, Integer bs, Integer nc)
{
    Ptr neu = Ptr( new PluginSessionHost(id, sr, bs, nc) );
    neu->host = host;
    return neu;
}
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
void PluginSessionHost::auto_opc_callback(Operations::Close::ArgPtr,
        Operations::Close::RetPtr)
{
}
//-----------------------------------------------------------------------------
void PluginSessionHost::auto_opc_callback(Operations::SetPluginLocation::ArgPtr,
        Operations::SetPluginLocation::RetPtr)
{
}
//-----------------------------------------------------------------------------
void PluginSessionHost::auto_opc_callback(Operations::GetPluginLocation::ArgPtr,
        Operations::GetPluginLocation::RetPtr)
{
}
//=============================================================================
//  Class PluginSessionClient
//=============================================================================
//-----------------------------------------------------------------------------
PluginSessionClient::PluginSessionClient(const std::string &id) : Session(id)
{
}
//-----------------------------------------------------------------------------
void PluginSessionClient::processImpl(Opc opc, void *argmem, void *retmem) {
    if ( !Operations::OpcManager::process(opc, this, argmem, retmem)) {
        using sambag::com::exceptions::IllegalArgumentException;
        SAMBAG_THROW(IllegalArgumentException,
        "BridgeSession::processImpl opc: " + sambag::com::toString(opc) + " not supported");
    }
}
//-------------------------------------------------------------------------
PluginSessionClient::Ptr PluginSessionClient::create(const std::string &id) {
    Ptr res = Ptr( new PluginSessionClient(id) );
    return res;
}
}}} // namespace(s)
